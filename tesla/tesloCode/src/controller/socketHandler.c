#include "socketHandler.h"
#include "measurement.h"
#include "measurementList.h"
#include "utility.h"

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
#include <time.h>
#include <unistd.h>

ConnectionList connections;

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

typedef struct {
  MeasurementServer value;
  int error;
} socketMeasureReaing;

socketMeasureReaing readFromSocket(const int clientFD) {
  socketMeasureReaing response;
  response.error = 0;

  const int recvBytes =
      recv(clientFD, &response.value, sizeof(MeasurementSocket), 0);

  if (recvBytes != sizeof(MeasurementSocket))
    response.error = 1;
  gettimeofday(&response.value.measurementTime, NULL);
  return response;
}

typedef struct {
  ConnectionList *list;
  long delta_t;
  FinalMeasurement result;

} threadArgs;
void *threadComputeFinalMeasurement(void *params) {
  threadArgs *threadData = params;
  FinalMeasurement result =
      computeFinalMeasurement(threadData->list, threadData->delta_t*2);
  threadData->result = result;
  printf("Finalizado calculo de la medicion\n");
  pthread_exit(0);
}

void *threadSave(void *params) {
  threadArgs *threadData = params;
  FILE *log = fopen("log.txt", "a+");
  time_t Tnow = time(NULL);
  struct tm now = *localtime(&Tnow);

  fprintf(log, "%02d:%02d:%04d:%02d:%02d:%02d {(%lf,%lf,%lf)}\n", now.tm_mday,
          now.tm_mon, now.tm_year, now.tm_hour, now.tm_min, now.tm_sec,
          threadData->result.valueLid, threadData->result.valueGPS,
          threadData->result.valueAcc);
  fclose(log);

  printf("Medicion {%lf, %lf, %lf} guardado en disco\n",threadData->result.valueLid,threadData->result.valueGPS,threadData->result.valueAcc);
  pthread_exit(0);
}
void *threadClean(void *params) {
  threadArgs *threadData = params;
  // now
  struct timeval now;
  gettimeofday(&now, NULL);

  // todo lo que esta almenos 3 delta_t en el pasado se borra
  const int removeDeltaT = threadData->delta_t * 6;
  ConnectionNode *conn = threadData->list->first;
  // para cada una de las connexiones
  while (conn != NULL) {
    // encontar el primer elemento a borrar
    MeasurementNode *measure = conn->measurements.first;
    MeasurementNode *removeAfter = NULL;
    while (measure != NULL) {
      if (getElapsedMicroSeconds(measure->measure.measurementTime, now) <
          removeDeltaT)
        removeAfter = measure;
      measure = measure->next;
    }
    removeEverythingAfter(&conn->measurements, removeAfter);
    // cerrar conexion si no ha llegado ningun paquete en mucho tiempo
    if ( conn->isOpen && conn->measurements.first && getElapsedMicroSeconds(conn->measurements.first->measure.measurementTime , now)>removeDeltaT){
      printf("Cerrada la conexion para sensor %d -> por inanctividad\n",conn->measurements.first->measure.sensorId );
      close(conn->clientFD);
      conn->isOpen = 0;
    }

    conn = conn->next;
  }
  printf("Proceso de limpieza terminado\n");
  pthread_exit(0);
}

void *serveConection(void *param) {
  ConnectionNode *connNode = (ConnectionNode *)param;
  printf("Nuevo sensor Detectado\n");
  while (1) {

    const socketMeasureReaing measurement = readFromSocket(connNode->clientFD);
    if (measurement.error)
      break;
    printf("Nuevo dato recibido del sensor %03d de tipo %d -> %5d\n",measurement.value.sensorId,measurement.value.sensorType,measurement.value.sensorReading);
    pthread_mutex_lock(&connNode->measurements.lock);
    addMeasurement(&connNode->measurements, &measurement.value);
    pthread_mutex_unlock(&connNode->measurements.lock);

    if (measurement.value.sensorType == 0) {
      printf("Nuevo dato de LIDAR recibido\n");
      const long delta_t = computeDeltaT(&connNode->measurements);
      usleep(delta_t/2);

      threadArgs tArgs;
      tArgs.list = &connections;
      tArgs.delta_t = delta_t/2;

      printf("Iniciando calculo de la medicion\n");
      pthread_t tidCompute;
      pthread_create(&tidCompute, NULL, threadComputeFinalMeasurement, &tArgs);
      pthread_join(tidCompute, NULL);
      // set vaule of lidar
      tArgs.result.valueLid = measurement.value.sensorReading;

      if (tArgs.result.isValid){
        printf("Iniciando proceso de almacenamiento de datos\n");
        pthread_t tidSave;
        pthread_create(&tidSave, NULL, threadSave, &tArgs);
      }
      else {
        printf("La medicion no valida ignorando dato LIDAR\n");
      }
      printf("Iniciando proceso de limpieza\n");
      pthread_t tidClean;
      pthread_create(&tidClean, NULL, threadClean, &tArgs);
    }
  }
  close(connNode->clientFD);
  connNode->isOpen = 0;
  printf("Cerrada la conexion para sensor %d -> por desconexion \n",connNode->measurements.first->measure.sensorId );
  pthread_exit(0);
}

int acceptConnection(const int ServerFD) {
  unsigned int clientLen = sizeof(struct sockaddr_in);
  struct sockaddr_in clientAdrss;

  // accept new cliente
  int client_sockfd =
      accept(ServerFD, (struct sockaddr *)&clientAdrss, &clientLen);
  return client_sockfd;
}
ConnectionNode *registerNewConnection(ConnectionList *const connList,
                                      const int clientFD) {
  ConnectionNode *newConnection = malloc(sizeof(ConnectionNode));
  newConnection->clientFD = clientFD;
  // add connection at the font of the list of connections
  ConnectionNode *temp = connList->first;
  newConnection->next = temp;
  connList->first = newConnection;

  connList->length++;
  return newConnection;
}
