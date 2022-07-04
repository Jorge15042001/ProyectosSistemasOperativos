#include "socketHandler.h"
#include "circularBuffer.h"
#include "serverStats.h"
#include "threadQueue.h"

#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// TODO: add lock to this var
static int TIME_OUT = 10;
static int TIME_REMOVE = 30;

// Constants
static const int FRAME_LEN = 7;

static const char *PATH_FRAME = "/frame";
static const char *PATH_SET = "/set";
static const char *PATH_MONITOR = "/monitor";

int startSocket(const int port) {

  struct sockaddr_in server_address;
  // Remove any old socket and create an unnamed socket for the server.
  const int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htons(INADDR_ANY);
  server_address.sin_port = htons(port);
  const int server_len = sizeof(server_address);

  const int rcodeBind =
      bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
  if (rcodeBind == -1) {
    fprintf(stderr, "No se pudo iniciar servidor[binding error]\n");
    exit(-1);
  }

  // Create a connection queue and wait for clients
  const int rcodeListen = listen(server_sockfd, 100);
  if (rcodeListen == -1) {
    fprintf(stderr, "No se pudo iniciar servidor\n");
    exit(-1);
  }

  printf("Servidor iniciado con exito: 127.0.0.1:%d\n", port);
  return server_sockfd;
}

int processSetRequest(char *msg) {
  char *copyMsg = malloc(strlen(msg) + 1);
  strcpy(copyMsg, msg);

  strtok_r(copyMsg, "/", &copyMsg);
  char *var = strtok_r(copyMsg, "/", &copyMsg);
  char *val = strtok_r(copyMsg, "/", &copyMsg);
  const int val_int = atoi(val);
  printf("setting %s to %d\n", var, val_int);

  if (strcmp(var, "timeout") == 0) {
    TIME_OUT = val_int;
    return 1;
  }
  if (strcmp(var, "timeremove") == 0) {
    TIME_REMOVE = val_int;
    return 1;
  }
  return 0;
}
void processNewFrame(connectionState *cs, circularBuffer *buf, char *msg) {
  if (cs->recivedFrames == 0) {
    cs->avgFrameTime = 2.0;
    cs->connectionType = FRAME_CONNECTION;
    cs->totalFrames = 100;
  } else {
    const int dTime = cs->lastPackageTime.tv_sec - cs->startTime.tv_sec;
    const float avgFrameT = (float)cs->recivedFrames / dTime;
    const int estimFrmsIn30S = (int)ceil(avgFrameT * 30);
    // if the buffer doesnot have enough space for 30seconds of video
    if ((int)(buf->capacity / FRAME_LEN) - estimFrmsIn30S <= 0) {
      resizeCircularBuffer(buf, (estimFrmsIn30S + 5) * FRAME_LEN);
    }
    cs->avgFrameTime = avgFrameT;
  }
  // regiter new frame to couter
  cs->recivedFrames = atoi(msg + strlen(PATH_FRAME) + 1);
  addCircularBuffer(buf, msg + strlen(PATH_FRAME) + 1, strlen(msg));
  cs->buffSize = buf->capacity;
  cs->buffUsage = (float)(buf->size) / (buf->capacity);
}

void *serveConection(void *param) {

  threadArgs *targs_ptr = (threadArgs *)param;
  const int client_sockfd = targs_ptr->sockfd;
  serverStats *stats = targs_ptr->stats;
  threadNode *tn = targs_ptr->tn;

  // memory for reading the data;
  circularBuffer buffer = createCircularBuffer(FRAME_LEN * 3);
  // report new connection to server stats
  // TODO: not able to run after realloc
  const int conStateIndex = registerNewConnection(stats);
  char msg[50];

  while (1) {
    // clean buffer
    memset(msg, 0, 50);
    // timeout
    struct timeval tv = {TIME_OUT, 0};
    // file sets for select
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET((unsigned int)client_sockfd, &rfds);

    // recive frame or timeout
    const int retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
    // register lastframe arrival time
    pthread_mutex_lock(&stats->serverStatsMutex);
    registerLatestFrameTime(
        getState(stats, conStateIndex)); // registe last frame time
    pthread_mutex_unlock(&stats->serverStatsMutex);
    if (retval == -1) {
      fprintf(stderr, "Select error\n");
      break;
    }
    if (retval == 0) {
      printf("Conexion timeout\n");
      break;
    }
    // read the message
    const int connectioError = recv(client_sockfd, msg, 50, 0);
    if (!connectioError) {
      printf("Se perdio la conexion\n");
      break;
    }
    // finde connection type and react accordingly
    pthread_mutex_lock(&stats->serverStatsMutex);
    connectionState *conState = getState(stats, conStateIndex);
    if (strncmp(msg, PATH_FRAME, strlen(PATH_FRAME)) == 0) {
      processNewFrame(conState, &buffer, msg);
      pthread_mutex_unlock(&stats->serverStatsMutex);
    } else if (strncmp(msg, PATH_MONITOR, strlen(PATH_MONITOR)) == 0) {
      conState->connectionType = MONITOR_CONNECTION;
      char *report = getReport(stats);
      write(client_sockfd, report, strlen(report));
      free(report);
      pthread_mutex_unlock(&stats->serverStatsMutex);
      break;

    } else if (strncmp(msg, PATH_SET, strlen(PATH_SET)) == 0) {
      conState->connectionType = SET_CONNECTION;
      const int success = processSetRequest(msg);
      write(client_sockfd, &success, sizeof(success));
      pthread_mutex_unlock(&stats->serverStatsMutex);
      break;
    } else {
      fprintf(stderr, "No se reconoce el path %s\n", msg);
      printf("connectionID %d path %s\n", conStateIndex, msg);
      pthread_mutex_unlock(&stats->serverStatsMutex);
      break;
    }
  }
  // clean the buffer
  cleanCircularBuffer(&buffer);
  pthread_mutex_lock(&stats->serverStatsMutex);
  connectionState *conState = getState(stats, conStateIndex);
  conState->buffSize = buffer.capacity;
  // report the server the connection has ended
  conState->StiilConnected = 0;
  pthread_mutex_unlock(&stats->serverStatsMutex);
  // release thread, add it to the ready queue
  addThread(targs_ptr->tq, tn);
  // colse the socket connection
  close(client_sockfd);
}
