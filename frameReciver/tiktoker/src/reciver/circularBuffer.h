#pragma once

typedef struct{
  int readIt;
  int writeIt;
  unsigned int size;
  unsigned int capacity;
  char* buffer;
} circularBuffer;

circularBuffer createCircularBuffer(const unsigned int capacity) ;
void cleanCircularBuffer(circularBuffer* cb);
void addCircularBuffer(circularBuffer *cb, char *data, const unsigned int N) ;
char getCharAt(circularBuffer*cb , const unsigned int index);
void resizeCircularBuffer(circularBuffer*cb, const unsigned int newCapacity);
void printCircularBuffer(circularBuffer*cb );

