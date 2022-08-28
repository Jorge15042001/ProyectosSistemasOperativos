#include "socketHandler.h"
#include "measurementList.h"

#include <stdio.h>
#include <pthread.h>


int main() {
  const int ServerFD = startSocket(7734);

  //initialize ConnectionList
  connections.length = 0;
  connections.first = NULL;

  while (1) {
    // on each connection
    const int clientFD = acceptConnection(ServerFD);
    ConnectionNode * sensorConnection = registerNewConnection(&connections,clientFD);
    sensorConnection->isOpen = 1;
    initList(&sensorConnection->measurements);
    // start a new thread to handle the connection
    if (pthread_create(&(sensorConnection->listenNode), NULL, serveConection, sensorConnection) != 0) {
      fprintf(stderr, "No se pudo atender la peticion\n");
    }
  }
}
