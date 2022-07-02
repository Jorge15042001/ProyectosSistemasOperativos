#include "socketHandler.h"
#include "circularBuffer.h"
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

static int TIME_OUT = 10;
static int TIME_REMOVE = 30;
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
  const int rcodeListen = listen(server_sockfd, 5);
  if (rcodeListen == -1) {
    fprintf(stderr, "No se pudo iniciar servidor\n");
    exit(-1);
  }

  printf("Servidor iniciado con exito: [%d]\n", server_sockfd);
  return server_sockfd;
}

void reciveNewFrame(const int estimatedFramesIn30Seconds,
                    circularBuffer *buffer, char *msg) {

  // if the buffer doesnot have enough space for 30seconds of video 
  if (abs((int)(buffer->capacity / FRAME_LEN) - estimatedFramesIn30Seconds) > 5) {
    resizeCircularBuffer(buffer, (estimatedFramesIn30Seconds + 3) * FRAME_LEN);
  }
  addCircularBuffer(buffer, msg+strlen(PATH_FRAME)+1, strlen(msg));
}


void *serveConection(void *param) {

  threadArgs *targs_ptr = (threadArgs *)param;
  const int client_sockfd = targs_ptr->sockfd;
  threadNode *tn = targs_ptr->tn;

  // soket time out
  circularBuffer buffer = createCircularBuffer(14 * 5);

  int waitingTime = 0;
  int recivedPackages = 0;
  /** printf("started socket thread %d \n", client_sockfd); */
  char msg[50];
  while (1) {
    // start timer
    struct timeval start;
    gettimeofday(&start, NULL);
    // timeout
    struct timeval tv = {6, 0};

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET((unsigned int)client_sockfd, &rfds);

    const int retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
    if (retval == -1) {
      fprintf(stderr, "Select error\n");
      break;
    }
    if (retval == 0) {
      printf("Conexion timeout\n");
      break;
    }
    // wait until a message arive at the socket
    const int res = recv(client_sockfd, msg, 100, 0);
    // end timer
    struct timeval end;
    gettimeofday(&end, NULL);

    if (res == 0) {
      printf("Se perdio la conexion\n");
      break;
    }
    recivedPackages++;
    const long secs_used = (end.tv_sec - start.tv_sec)==0? 1:(end.tv_sec - start.tv_sec);
    waitingTime += secs_used;

    const int estimatedFramesIn30Seconds = (int)ceil((float)(recivedPackages) / waitingTime* 30);

    if (strncmp(msg, PATH_FRAME, strlen(PATH_FRAME)) == 0) {
      reciveNewFrame(estimatedFramesIn30Seconds,  &buffer, msg);
    } else if (strncmp(msg, PATH_MONITOR, strlen(PATH_MONITOR)) == 0) {
    } else if (strncmp(msg, PATH_SET, strlen(PATH_SET)) == 0) {
    }
  }
  printf("End serving socket\n");
  // add thread to readyqueue
  addThread(targs_ptr->tq, tn);
  // colse the socket connection
  close(client_sockfd);
  cleanCircularBuffer(&buffer);
}
