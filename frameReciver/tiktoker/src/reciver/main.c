#include "socketHandler.h"
#include "threadQueue.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static const int port = 7734;
static unsigned int client_len = sizeof(struct sockaddr_in);

int startSocket(const int port);
void *serveConection(void *param);

int main() {
  const int server_sockfd = startSocket(port);
  /** printf("Servidor corriendo en puerto %d\n",port); */
  threadQueue tq = createThreadQueue(1);
  /** printf("Cola de procesos lista lista \n"); */

  while (1) {
    struct sockaddr_in client_address;
    int client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
    printf("Nueva conexion aceptada\n");

    // thread code
    threadNode *readyTN = getReadyThread(&tq);
    threadArgs targs = {client_sockfd, &tq, readyTN};
    if (pthread_create(&readyTN->thread, NULL, serveConection, &targs) != 0) {
      fprintf(stderr, "No se pudo atender la peticion\n");
    }
    /** pthread_join(readyTN->thread, NULL); */
  }
  printf("terminado servidor\n");
  cleanThreadQueue(&tq);
}
