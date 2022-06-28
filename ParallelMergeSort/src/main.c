#include <stdio.h>
#include <stdlib.h>

#include "mergSort.h"

int *readFile(const char *filename, int * nItems) {
  FILE *const fp = fopen(filename, "r");
  if (fp == NULL){
    fprintf(stderr, "Could not read file %s",filename);
    exit(-1);
  }

  int capacity = 10;
  int * values = malloc(capacity * sizeof(int));
  int * values_it = values;

  int inc;
  int value;
  while(EOF!=(inc=fscanf(fp,"%d,", &value)) && inc == 1){
    if (values_it == (values+capacity)){//array is full
      const int delta = values_it - values;
      capacity *= 2;
      values = realloc(values,capacity*sizeof(int));
      values_it = values + delta;
    }
    *values_it = value;
    values_it ++;

  }


  *nItems = values_it - values;
  return values;
}
int main() {

  int nItems;
  int * array = readFile("input.csv", &nItems);
  
  int * arraySorted = mergeSort(array, nItems);
  
  //print arraySorted
  printArray(arraySorted, nItems);

  free(array);
  free(arraySorted);
  return 0;
}
