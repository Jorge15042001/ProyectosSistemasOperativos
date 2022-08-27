#include "measurementList.h"
#include "measurement.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void initList(MeasurementList*const list){
  pthread_mutex_init(&list->lock,NULL);
  list->first = NULL;
  list->length = 0;
}
void printMeasurementList (MeasurementList *const list){
  printf("MeasurementList{ size: %ld \n",list->length);
  MeasurementNode * temp= list->first;
  while (temp != NULL) {
    printf("\t");
    printMeasurementServer(&temp->measure);
    temp= temp->next;
  }
  printf("}\n");
}
void addMeasurement(MeasurementList *const list,const MeasurementServer *const measure){
  MeasurementNode *newNode = malloc(sizeof(MeasurementNode));

  /** newNode->measure = *measure; */
  newNode->measure.sensorId = measure->sensorId;
  newNode->measure.sensorType = measure->sensorType;
  newNode->measure.sensorReading = measure->sensorReading;
  newNode->measure.measurementTime = measure->measurementTime;

  MeasurementNode *temp  = list->first;
  newNode->next = temp;
  list->first = newNode;
  list->length ++;
}
void removeEverythingAfter(MeasurementList *const list,MeasurementNode *const measure){
  MeasurementNode * start = list->first;
  if (measure == NULL || measure->next == NULL) return;

  //put the list in valid state
  MeasurementNode * temp = measure;
  temp->next = NULL;
  size_t length = 0;
  while (start != NULL) {
    length ++;
    start = start ->next;
  }

  //set new length
  list->length = length;

  //remove and clean memory
  MeasurementNode * toRemove = temp->next;
  while (toRemove != NULL) {
    MeasurementNode *removing = toRemove;
    toRemove = toRemove ->next;
    free(removing);
  }
}
