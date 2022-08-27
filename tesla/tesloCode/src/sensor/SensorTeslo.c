#include "ziggurat.h"
#include <arpa/inet.h>
#include <math.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define DEBUG 1

typedef struct {
  int sensorId;
  int sensorType;
  int sensorValue;
} MeasurementServer;

int main(int argc, char *argv[]) {
  float fn[128], Tfreq;
  int ID_sensor, Stype, Davg, Dsd, Tsd, sensor_reading;
  uint32_t kn[128];
  unsigned int delay;
  uint32_t seed;
  float value;
  float wn[128];

  if (argc != 7) {
    fprintf(
        stderr,
        "usage: tiktoker <Tmin:integer> <Tmax:integer value> <Prob:1|0> \n");
    return -1;
  }

  ID_sensor = atoi(argv[1]);
  Stype = atoi(argv[2]);
  Davg = atoi(argv[3]);
  Dsd = atoi(argv[4]);
  Tfreq = atof(argv[5]);
  Tsd = atoi(argv[6]);

  if (ID_sensor < 0 || Stype < 0 || Davg < 0 || Dsd < 0 || Tfreq < 0 ||
      Tsd < 0) {
    fprintf(stderr, "Arguments must be non-negative\n");
    return -1;
  }

  r4_nor_setup(kn, fn, wn);
  srand(time(NULL)); // Initialization, should only be called once.
  seed = rand();

  /*
   * IPC communication, usiing sockets
   * */
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("Socket create failed.\n");
    return -1;
  }
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(7734);
  int len = sizeof(address);

  int result = connect(sockfd, (struct sockaddr *)&address, len);
  if (result == -1) {
    perror("Error has occurred");
    exit(-1);
  }

  while (1) {
    value = r4_nor(&seed, kn, fn, wn);
    if (value < 0)
      value = value * (-1);
    sensor_reading = floor(value * Dsd + Davg);
    delay = floor(1000000 / (value * Tsd + Tfreq));
    usleep(delay);

    // DEBUG <- 0 TO TEST AWTU
    /** if (DEBUG) */
    /**   printf( */
    /**       "data to be send id-sensor %d, type %d, record %d, delay %d ms. \n", */
    /**       ID_sensor, Stype, sensor_reading, delay / 1000); */
    // INSERTAR IPC PARA ENVIO DE MENSAJES AQUI!!
    MeasurementServer measure = {ID_sensor, Stype, sensor_reading};
    write(sockfd, &measure, sizeof(MeasurementServer));
  }
  

  return EXIT_SUCCESS;
}
