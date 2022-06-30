#include "string.h"
#include "threadQueue.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

const int port = 7734;

typedef struct {
  int sockfd;
  threadQueue* tq;
  threadNode* tn;
}threadArgs ;

int startSocket(const int port);
void *serveConection(void *param);

int main() {
  const int server_sockfd = startSocket(port);
  /** printf("Servidor corriendo en puerto %d\n",port); */
  threadQueue tq= createThreadQueue(10);
  /** printf("Cola de procesos lista lista \n"); */

  while (1) {
    // Declaring process variables.
    struct sockaddr_in client_address;
    unsigned int client_len = sizeof(client_address);
    /** int client_sockfd; */
    int client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
    printf("client_sockfd %d\n",client_sockfd);
    
    /** printf("after accept()... client_sockfd = %d\n", client_sockfd); */

    // thread code
    threadNode * readyTN = getReadyThread(&tq);
    threadArgs targs = {client_sockfd,&tq, readyTN};
    if (pthread_create(&readyTN->thread, NULL, serveConection, &targs) != 0) {
      fprintf(stderr, "No se pudo atender la peticion\n");
    }
    /** pthread_join(readyTN->thread, NULL); */

  }
  printf("terminado servidor\n");
  cleanThreadQueue(&tq);
}
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

  printf("Servidor iniciado con exito: [%d]\n",server_sockfd);
  return server_sockfd;
}
void *serveConection(void *param) {

  threadArgs* targs_ptr = (threadArgs*) param;
  const int client_sockfd = targs_ptr->sockfd;
  threadNode* tn = targs_ptr->tn;

    printf("fd thread %d \n",client_sockfd);
    int it = 0;
    char msg [100] ;
  for (;;) {
    const int res = recv(client_sockfd, msg,100 , 0);
    printf("%d %d %s\n",it++,res,msg);
    if (res == 0){
      printf("Se perdio la conexion\n");
      break;
    }
  }
  addThread(targs_ptr->tq, tn);
  close(client_sockfd);


}
