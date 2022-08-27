#include "measurement.h"
#include <stdio.h>



void printMeasurementServer(const MeasurementServer * const measurement){
  printf("MeasurementServer{id:%3d, type:%3d, value:%8d}\n",measurement->sensorId,measurement->sensorType,measurement->sensorReading);

}
