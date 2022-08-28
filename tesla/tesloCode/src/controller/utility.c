#include "utility.h"
#include "measurement.h"
#include <stdio.h>

long getElapsedMicroSeconds(struct timeval t0, struct timeval t1) {
  const long secs_used = (t1.tv_sec - t0.tv_sec);
  const long micros_used = ((secs_used * 1000000) + t1.tv_usec) - (t0.tv_usec);
  return micros_used;
}

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
    }
    nodeMeasure = nodeMeasure->next;
  }
  return sqrt(sumDev / elements);
}
size_t computeMeasurementCount(ConnectionNode *const node,
                               const int delta_t, const struct timeval tf) {
  
  size_t elements = 0;
  pthread_mutex_lock(&node->measurements.lock);
  MeasurementNode *nodeMeasure = node->measurements.first;
  pthread_mutex_unlock(&node->measurements.lock);
  while (nodeMeasure != NULL) {
    if (getElapsedMicroSeconds(nodeMeasure->measure.measurementTime, tf) <
        delta_t) {
      elements++;
    }
    nodeMeasure = nodeMeasure->next;
  }
  return elements;
}

int getSensorType(ConnectionNode *const node) {
  MeasurementNode *meNode = node->measurements.first;
  if (meNode)
    return meNode->measure.sensorType;
  return -1;
}

double computeTotalError(const MesurementSummary *const data,
                         const size_t length, const int sensorType) {
  double sum = 0;
  for (size_t i = 0; i < length; i++) {
    if (data[i].sensorType == sensorType && data[i].measCount > 4) {
      sum += data[i].stdDev;
    }
  }
  return sum;
}

double computeLinearCombination(const MesurementSummary *const data,
                                const size_t length, const int sensorType) {
  double result = 0;
  for (size_t i = 0; i < length; i++) {
    if (data[i].sensorType == sensorType && data[i].measCount > 4) {
      result += data[i].avg * data[i].weight;
    }
  }
  return result;
}

double computeWeight(const MesurementSummary *const summary, const size_t i,
                     const double *const totalErros) {
  if (summary[i].sensorType < 1)
    return -1;
  const double weight =
      1 - summary[i].stdDev / totalErros[summary[i].sensorType];
  // esto solo ocurre si hay una solo sensor para el tipo de sensor
  if (summary[i].measCount >=1 )
    return 1;
  return weight;
}

int isCompleteData(const MesurementSummary *const summary, const size_t length){
  int validCounter [3] = {0,0,0};
  for (size_t i = 0; i < length ; i ++){
    if (summary[i].measCount > 4) validCounter[summary[i].sensorType]++;
  }
  if (validCounter[1]>=1 && validCounter[2]>= 1)return 1;
  return 0;
}

FinalMeasurement computeFinalMeasurement(const ConnectionList *const list,
                                         const int delta_t) {
  FinalMeasurement result;
  gettimeofday(&result.time, NULL);

  const size_t length = list->length;
  MesurementSummary summary[length];

  ConnectionNode *node = list->first;
  for (size_t i = 0; i < length; i++, node = node->next) {
    summary[i].sensorType = getSensorType(node);
    summary[i].avg = computeAvg(node, delta_t, result.time);
    summary[i].stdDev =
        computeStdDev(node, summary[i].avg, delta_t, result.time);
    summary[i].measCount = computeMeasurementCount(node, delta_t, result.time);
  }

  double totalErros[3];
  totalErros[1] = computeTotalError(summary, length, 1);
  totalErros[2] = computeTotalError(summary, length, 2);

  for (size_t i = 0; i < length; i++)
    summary[i].weight = computeWeight(summary, i, totalErros);

  result.valueGPS = computeLinearCombination(summary, length, 1);
  result.valueAcc = computeLinearCombination(summary, length, 2);

  result.isValid = isCompleteData(summary,length);

  return result;
}
long computeDeltaT(const MeasurementList *const list) {
  struct timeval t0 = list->first->measure.measurementTime;
  if (list->length <= 1)
    return 1000000;

  MeasurementNode *node = list->first;
  if (node == NULL)
    return 1000000;

  while (node->next != NULL)
    node = node->next;
  struct timeval t1 = node->measure.measurementTime;

  return getElapsedMicroSeconds(t1, t0) / (list->length-1);
}
