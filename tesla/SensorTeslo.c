# include <stdlib.h>
# include <math.h>
# include <stdio.h>
# include <time.h>
# include <stdint.h>
# include <unistd.h>
# include "ziggurat.h"

#define DEBUG 1

int main(int argc, char *argv[])
{
float fn[128],Tfreq;
int ID_sensor,Stype,Davg,Dsd,Tsd,sensor_reading;
uint32_t kn[128];
unsigned int delay;
uint32_t seed;
float value;
float wn[128];

if (argc != 7) {
	fprintf(stderr,"usage: tiktoker <Tmin:integer> <Tmax:integer value> <Prob:1|0> \n");	
	return -1;
}

ID_sensor=atoi(argv[1]);
Stype=atoi(argv[2]);
Davg=atoi(argv[3]);
Dsd=atoi(argv[4]);
Tfreq=atof(argv[5]);
Tsd=atoi(argv[6]);


if ( ID_sensor< 0 || Stype < 0 || Davg < 0 || Dsd < 0|| Tfreq < 0|| Tsd < 0) {
	fprintf(stderr,"Arguments must be non-negative\n");	
	return -1;
}

  r4_nor_setup ( kn, fn, wn );
  srand(time(NULL));   // Initialization, should only be called once.
  seed = rand(); 
  
  while(1){
        value = r4_nor ( &seed, kn, fn, wn );
        if (value < 0) value=value*(-1);
        sensor_reading= floor(value*Dsd+Davg);
      	delay=floor(1000000/(value*Tsd+Tfreq));
  	usleep(delay);
  	
  	// DEBUG <- 0 TO TEST AWTU
  	if (DEBUG) printf ( "data to be send id-sensor %d, type %d, record %d, delay %d ms. \n",ID_sensor,Stype,sensor_reading,delay/1000);
  	// INSERTAR IPC PARA ENVIO DE MENSAJES AQUI!!
       
    }
  
  return EXIT_SUCCESS;

}




