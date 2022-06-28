#include "ziggurat.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int Fmax = 100;

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
  Tmean = Tmin + floor(Tdiff / 2);
  Tsd = sqrt(Tmean / 3);
  printf(" %14f %d %d \n", Tsd, Tdiff, Tmean);

  printf("\n");
  printf("Tiktoker\n");

  r4_nor_setup(kn, fn, wn);

  srand(time(NULL)); // Initialization, should only be called once.
  seed = rand();

  for (i = 1; i <= Fmax; i++) { // Remover mensajes para probar
    value = r4_nor(&seed, kn, fn, wn);
    Tsleep = floor(value * Tsd + Tmean);
    printf("frame %d in %d seconds \n", i, Tsleep);
    sleep(Tsleep);
    // INSERTAR IPC PARA ENVIO DE MENSAJES AQUI!!
    printf("frame %d sent to Tiktok \n", i);
    printf("prob %f \n", (1.0 * (rand()) / RAND_MAX));
    if ((Prob == 1) && (1.0 * (rand()) / RAND_MAX) > 0.99) {
      printf("Video conection lost \n");
      return (EXIT_SUCCESS); // No more frames
    }
  }
  return (EXIT_SUCCESS);
}
