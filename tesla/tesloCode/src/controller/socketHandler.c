#include "socketHandler.h"
#include "measurement.h"
#include "measurementList.h"

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

long getElapsedMicroSeconds(struct timeval t0, struct timeval t1) {
  const long secs_used = (t1.tv_sec - t0.tv_sec);
  const long micros_used = ((secs_used * 1000000) + t1.tv_usec) - (t0.tv_usec);
  return micros_used;
}

typedef struct {
  int sensorType;
  double avg;
  double stdDev;
  double weight;
} AvgMeasurement;

typedef struct {
  double valueLid;
  double valueAcc;
  double valueGPS;
  struct timeval time;
} FinalMeasurement;

double computeAvg(ConnectionNode *const node, const int delta_t,
                  struct timeval tf) {
  double sum = 0;
  size_t elements = 0;

  pthread_mutex_lock(&node->measurements.lock);
  MeasurementNode *nodeMeasure = node->measurements.first;
  pthread_mutex_unlock(&node->measurements.lock);
  while (nodeMeasure != NULL) {
    if (getElapsedMicroSeconds(nodeMeasure->measure.measurementTime, tf) <
        delta_t) {
      sum += nodeMeasure->measure.sensorReading;
      elements++;
    }
    nodeMeasure = nodeMeasure->next;
  }
  return sum / elements;
}
double computeStdDev(ConnectionNode *const node, const double avg,
                     const int delta_t, struct timeval tf) {
  double sumDev = 0;
  size_t elements = 0;

  pthread_mutex_lock(&node->measurements.lock);
  MeasurementNode *nodeMeasure = node->measurements.first;
  pthread_mutex_unlock(&node->measurements.lock);

  while (nodeMeasure != NULL) {
    if (getElapsedMicroSeconds(nodeMeasure->measure.measurementTime, tf) <
        delta_t) {
      const double error = nodeMeasure->measure.sensorReading - avg;
      sumDev += error * error;
      elements++;
      /** printf("error %d %ld
       * %lf\n",nodeMeasure->measure.sensorType,elements,sumDev); */
    }
    nodeMeasure = nodeMeasure->next;
  }
  return sqrt(sumDev / elements);
}

int getSensorType(ConnectionNode *const node) {
  MeasurementNode *meNode = node->measurements.first;
  if (meNode)
    return meNode->measure.sensorType;
  return -1;
}

double computeTotalError(const AvgMeasurement *const data, const size_t length,
                         const int sensorType) {

  double sum = 0;
  for (size_t i = 0; i < length; i++) {
    if (data[i].sensorType == sensorType) {
      sum += data[i].stdDev;
    }
  }
  return sum;
}

double computeLinearCombination(const AvgMeasurement *const data,
                                const size_t length, const int sensorType) {
  double result = 0;
  for (size_t i = 0; i < length; i++) {
    if (data[i].sensorType == sensorType) {
      result += data[i].avg * data[i].weight;
    }
  }
  /** printf("\nresult %d -> %lf * %lf  =
   * %lf\n",sensorType,data[i].avg,data[i].weight,result); */
  return result;
}

FinalMeasurement computeFinalMeasurement(const ConnectionList *const list,
                                         const int delta_t) {
  FinalMeasurement result;
  gettimeofday(&result.time, NULL);

  const size_t length = list->length;
  AvgMeasurement avgs[length];

  ConnectionNode *node = list->first;
  for (size_t i = 0; i < length; i++, node = node->next) {
    avgs[i].sensorType = getSensorType(node);
    avgs[i].avg = computeAvg(node, delta_t, result.time);
    avgs[i].stdDev = computeStdDev(node, avgs[i].avg, delta_t, result.time);
  }

  const double totalErros[] = {0, computeTotalError(avgs, length, 1),
                               computeTotalError(avgs, length, 2)};

  for (size_t i = 0; i < length; i++) {
    if (avgs[i].sensorType < 1)
      continue;
    avgs[i].weight = 1 - avgs[i].stdDev / totalErros[avgs[i].sensorType];
    /** printf("%ld: %lf\n",i,avgs[i].stdDev); */
  }

  result.valueLid = -1; // TODO:
  result.valueGPS = computeLinearCombination(avgs, length, 1);
  result.valueAcc = computeLinearCombination(avgs, length, 2);

  /** printf("computed: %lf\n",result.valueAcc); */

  return result;
}
int computeDeltaT(const MeasurementList *const list) {
  struct timeval t0 = list->first->measure.measurementTime;
  MeasurementNode *node = list->first;
  if (node == NULL)
    return 500000;
  while (node->next != NULL)
    node = node->next;
  struct timeval t1 = node->measure.measurementTime;

  return getElapsedMicroSeconds(t0, t1) / list->length;
}

typedef struct {
  ConnectionList *list;
  int delta_t;
  FinalMeasurement result;

} threadArgs;
void *threadComputeFinalMeasurement(void *params) {
  threadArgs *threadData = params;
  FinalMeasurement result =
      computeFinalMeasurement(threadData->list, threadData->delta_t);
  threadData->result = result;
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
}
void *threadClean(void *params) {
  threadArgs *threadData = params;
  // now
  struct timeval now;
  gettimeofday(&now, NULL);

  // todo lo que esta almenos 3 delta_t en el pasado se borra
  const int removeDeltaT = threadData->delta_t * 3;
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
    conn = conn->next;
  }
}

void *serveConection(void *param) {
  ConnectionNode *connNode = (ConnectionNode *)param;
  while (1) {
    const socketMeasureReaing measurement = readFromSocket(connNode->clientFD);
    if (measurement.error)
      break;
    /** printMeasurementServer(&measurement.value); */
    addMeasurement(&connNode->measurements, &measurement.value);
    if (measurement.value.sensorType == 0) {
      const int delta_t = computeDeltaT(&connNode->measurements);
      threadArgs tArgs = {&connections, delta_t * 2};
      pthread_t tidCompute;
      pthread_create(&tidCompute, NULL, threadComputeFinalMeasurement, &tArgs);
      pthread_join(tidCompute, NULL);
      // set vaule of lidar
      tArgs.result.valueLid = measurement.value.sensorReading;
      pthread_t tidSave;
      pthread_create(&tidSave, NULL, threadSave, &tArgs);

      pthread_t tidClean;
      pthread_create(&tidClean, NULL, threadClean, &tArgs);
    }
  }
  close(connNode->clientFD);
}

int acceptConnection(const int ServerFD) {
  unsigned int clientLen = sizeof(struct sockaddr_in);
  struct sockaddr_in clientAdrss;

  // accept new cliente
  int client_sockfd =
      accept(ServerFD, (struct sockaddr *)&clientAdrss, &clientLen);
  printf("Nueva conexion aceptada\n");
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
