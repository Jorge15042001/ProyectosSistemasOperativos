#include "serverStats.h"
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

int main() {
  threadQueue tq = createThreadQueue(8);
  serverStats serverState = createServerStats(8);
  const int server_sockfd = startSocket(port);

  while (1) {
    struct sockaddr_in client_address;
    int client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
    printf("Nueva conexion aceptada\n");

    // thread code
    threadNode *readyTN = getReadyThread(&tq);
    threadArgs targs = {client_sockfd, &tq, readyTN,&serverState};
    if (pthread_create(&readyTN->thread, NULL, serveConection, &targs) != 0) {
      fprintf(stderr, "No se pudo atender la peticion\n");
    }
  }
  printf("terminado servidor\n");
  cleanThreadQueue(&tq);
  cleanServerStats(&serverState);
}
