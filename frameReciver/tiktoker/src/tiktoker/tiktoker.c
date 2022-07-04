#define _GNU_SOURCE

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

const int Fmax = 100;

int main(int argc, char *argv[]) {
  float fn[128];
  int i;
  int Tmean, Tsleep, Tdiff, Tmin, Tmax, Prob;
  uint32_t kn[128];
  uint32_t seed;
  float value, Tsd;
  float wn[128];

  if (argc != 4) {
    fprintf(
        stderr,
        "usage: tiktoker <Tmin:integer> <Tmax:integer value> <Prob:double> \n");
    return -1;
  }

  Tmin = atoi(argv[1]);
  Tmax = atoi(argv[2]);
  Prob = atoi(argv[3]);

  if (Tmin < 0 || Tmax < 0 || Prob < 0) {
    fprintf(stderr, "Arguments must be non-negative\n");
    return -1;
  }

  if (!(Prob == 0 || Prob == 1)) {
    fprintf(stderr, "Chance must be  zero or one \n");
    return -1;
  }

  if (Tmin >= Tmax) {
    fprintf(stderr, "Tmin must be lower than Tmax \n");
    return -1;
  }

  Tdiff = Tmax - Tmin;
  Tmean = Tmin + floor(Tdiff / 2.0);
  Tsd = sqrt(Tmean / 3.);
  /** printf(" %14f %d %d \n", Tsd, Tdiff, Tmean); */

  /** printf("\n"); */
  /** printf("Tiktoker\n"); */

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

  for (i = 1; i <= Fmax; i++) { // Remover mensajes para probar
    value = r4_nor(&seed, kn, fn, wn);
    Tsleep = floor(value * Tsd + Tmean);
    if (Tsleep <= 0) Tsleep = Tmin;
    sleep(Tsleep);
    // INSERTAR IPC PARA ENVIO DE MENSAJES AQUI!!
    char *msg;
    int msgStatus = asprintf(&msg, "/frame/%03d/%03d", i, Fmax);
    if (msgStatus == -1) {
      fprintf(stderr, "Error generating message\n");
      printf("early exit due to error\n");
      exit(-1);
    }
    if (strlen(msg)==0)printf("sending empty frame\n");
    const int rc = write(sockfd, msg, strlen(msg));
    free(msg);

    /** printf("prob %f \n", (1.0 * (rand()) / RAND_MAX)); */
    if ((Prob == 1) && (1.0 * (rand()) / RAND_MAX) > 0.99) {
      printf("Video conection lost \n");
      return (EXIT_SUCCESS); // No more frames
    }
  }
  return (EXIT_SUCCESS);
}
