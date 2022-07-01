#include "socketHandler.h"
#include "circularBuffer.h"
#include "threadQueue.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static int TIME_OUT = 10; 
static int TIME_REMOVE = 30; 

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
    fprintf(stderr, "No se pudo iniciar servidor\n");
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

typedef struct {
  const unsigned int seconds;
  const int socketFd;
  int *finished;
  int *msgRecived;

} socketTimeOutArgs;

void *socketTimeOut(void *param) {
  socketTimeOutArgs *st_ptr = (socketTimeOutArgs *)param;
  sleep(st_ptr->seconds);
  //if parent did not recived a message from socket while waiting
  if (*(st_ptr->msgRecived) == 0) {
    close(st_ptr->socketFd);
    printf("socked %d cerrado debido a timeOut\n", st_ptr->socketFd);
  }
  //alert parent thread that the timer went off and the connection was closed
  *(st_ptr->finished) = 1;
}

void *serveConection(void *param) {

  threadArgs *targs_ptr = (threadArgs *)param;
  const int client_sockfd = targs_ptr->sockfd;
  threadNode *tn = targs_ptr->tn;

  // soket time out
  pthread_t timeOut_pid; //
  int msgRecived;
  int timerFinished;
  socketTimeOutArgs skTO_arg = {3, client_sockfd, &timerFinished, &msgRecived};

  circularBuffer buffer = createCircularBuffer(14 * 5);

  printf("fd thread %d \n", client_sockfd);
  char msg[50];
  while (1) {
    const int timerResponse =
        pthread_create(&timeOut_pid, NULL, socketTimeOut, &skTO_arg);
    if (timerResponse != 0) {
      fprintf(stderr, "No se pudo iniciar timeout\n");
      break;
    }
    // wait until a message arive at the socket
    const int res = recv(client_sockfd, msg, 100, 0);
    // alerts a message have arrived
    msgRecived = 1;
    // if the timer went off
    if (timerFinished == 1)
      break;
    // if the timer did not went off, kill the thread
    else {
      pthread_cancel(timeOut_pid);
    }
    // if connection was lost
    if (res == 0) {
      printf("Se perdio la conexion\n");
      break;
    }

    /** printf("%ld,%s\n",strlen(msg),msg); */
    /** printCircularBuffer(&buffer); */

    addCircularBuffer(&buffer, msg, strlen(msg));
    printf("%s\n", msg);
  }
  // add thread to readyqueue
  addThread(targs_ptr->tq, tn);
  // colse the socket connection
  close(client_sockfd);
}
