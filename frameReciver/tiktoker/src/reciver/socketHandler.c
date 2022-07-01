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
void *serveConection(void *param) {

  threadArgs *targs_ptr = (threadArgs *)param;
  const int client_sockfd = targs_ptr->sockfd;
  threadNode *tn = targs_ptr->tn;
  circularBuffer buffer = createCircularBuffer(14 * 2);

  printf("fd thread %d \n", client_sockfd);
  char msg[50];
  for (;;) {
    const int res = recv(client_sockfd, msg, 100, 0);
    /** printf("%ld,%s\n",strlen(msg),msg); */
    addCircularBuffer(&buffer, msg, strlen(msg));
    printCircularBuffer(&buffer);

    if (res == 0) {
      printf("Se perdio la conexion\n");
      break;
    }
  }
  addThread(targs_ptr->tq, tn);
  printf("threadQueue tiene %d\n", targs_ptr->tq->length);
  close(client_sockfd);
}
