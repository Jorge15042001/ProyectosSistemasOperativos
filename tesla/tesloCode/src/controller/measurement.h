#pragma once
#include <sys/time.h>

/*
 * struct Measurement
 *
 * this struct represents each reading that the server stores from the sensors
 *
 * fields:
 *  sensorId: unique identifier for the id
 *  sensorType: unique identifier for the type of sensor
 *  sensorReading: value read by the sensor
 */

typedef struct{
  int sensorId;
  int sensorType;
  int sensorReading;
  struct timeval measurementTime;
} MeasurementServer;

/*
 * function printMeasurementServer
 *
 * prints a MeasurementServer instance 
 *
 * parameters:
 *  MeasurementServer instance
 */
void printMeasurementServer(const MeasurementServer * const);

/*
 * struct MeasurementSocket
 *
 * this struct represents each reading that the server receives from the sensors
 *
 * fields:
 *  sensorId: unique identifier for the id
 *  sensorType: unique identifier for the type of sensor
 */

typedef struct{
  int sensorId;
  int sensorType;
  int sensorReading;
} MeasurementSocket;
