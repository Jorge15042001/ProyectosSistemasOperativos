#pragma once
#include "measurement.h"

#include <pthread.h>

typedef struct MeasurementNode MeasurementNode;
struct MeasurementNode{
  MeasurementServer measure;
  MeasurementNode* next;
};


typedef struct{
  MeasurementNode * first;
  size_t length ;
  pthread_mutex_t lock;
} MeasurementList;


void initList(MeasurementList *const list);
void printMeasurementList (MeasurementList *const list);
void addMeasurement(MeasurementList *const list,const MeasurementServer *const measure);
void removeEverythingAfter(MeasurementList *const list,MeasurementNode *const measure);
