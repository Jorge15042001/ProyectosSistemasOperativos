#include "circularBuffer.h"

#include <malloc.h>
#include <stdio.h>

circularBuffer createCircularBuffer(const unsigned int capacity) {
  char *buffer = malloc(capacity);
  circularBuffer cb = {0, 0, 0, capacity, buffer};
  return cb;
}
void cleanCircularBuffer(circularBuffer *cb){
  free(cb->buffer);
  cb->buffer = NULL;
  cb->size =0;
  cb->writeIt = 0;
  cb->readIt = 0;
  cb->capacity = 0;
}

void addCircularBuffer(circularBuffer *cb, char *data, const unsigned int N) {
  // TODO: valide N is grater and cb->size
  for (int i = 0; i < N; i++) {
    cb->buffer[cb->writeIt] = data[i];
    cb->writeIt = (cb->writeIt + 1) % cb->capacity;

    cb->size ++;

    if (cb->size >= cb->capacity){
      cb->readIt = cb->writeIt;
      cb->size = cb->capacity;

    }
  }
}

char getCharAt(circularBuffer*cb , const unsigned int index){
  return cb->buffer[(cb->readIt+index)%cb->capacity];
}

void resizeCircularBuffer(circularBuffer*cb, const unsigned int newCapacity){
  const unsigned int oldCapacity = cb->capacity;
  const unsigned int oldSize = cb->size;

  if (newCapacity == oldCapacity)return;

  printf("Resizing: start\n");
  char * newBuff =  malloc(newCapacity);

  if (newCapacity > oldSize){
    //copy all elements appended from old buffer to new buffer
    for (int i = 0 ; i < oldSize; i++){
      newBuff[i] = getCharAt(cb,i);
    }
    cb->size = oldSize;
    cb->writeIt=oldSize;

  }
  else {
    //copy some elements from old buffer to new buffer
    //copy as many elements as possible
    //older elements are removed newers are copied
    const unsigned int oldestCopyIndex = oldSize - newCapacity;
    for (int i = 0 ; i < newCapacity; i++){
      newBuff[i]= getCharAt(cb, oldCapacity+i);
    }
    cb->size = newCapacity;
    cb->writeIt = 0;
  }

  free(cb->buffer);
  cb->buffer = newBuff;
  cb->capacity = newCapacity;
  cb->readIt = 0;

  printf("Resizing: done\n");


}

void printCircularBuffer(circularBuffer*cb ){
  printf("Buffer[%d/%d]\n",cb->size,cb->capacity);
  /** printf("{"); */
  /** for (int i = 0 ; i < cb->size ; i++){ */
  /**   printf("%c",getCharAt(cb, i)); */
  /** } */
  /** printf("}\n"); */
}
