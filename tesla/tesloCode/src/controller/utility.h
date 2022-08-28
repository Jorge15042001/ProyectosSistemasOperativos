#pragma once

#include "measurement.h"
#include "socketHandler.h"

#include <math.h>
#include <sys/time.h>

long getElapsedMicroSeconds(struct timeval t0, struct timeval t1);
double computeAvg(ConnectionNode *const node, const int delta_t,
                  struct timeval tf);
double computeStdDev(ConnectionNode *const node, const double avg,
                     const int delta_t, struct timeval tf);
size_t computeMeasurementCount(ConnectionNode *const list,
                               const int delta_t, const struct timeval tf);
int getSensorType(ConnectionNode *const node);
double computeTotalError(const MesurementSummary *const data,
                         const size_t length, const int sensorType);
double computeLinearCombination(const MesurementSummary *const data,
                                const size_t length, const int sensorType);
double computeWeight(const MesurementSummary *const summary, const size_t i,
                     const double *const totalErros);

FinalMeasurement computeFinalMeasurement(const ConnectionList *const list,
                                         const int delta_t);
long computeDeltaT(const MeasurementList *const list);
