#pragma once
/*
 * Struct circularBuffer 
 *
 * it is a buffer that automatically overwrites old elements
 * when a new element needs to be inserted
 *
 * members:
 *  readIt: oldest element in the buffer
 *  writeIt: position where the newest element will be written
 *  size: number of elements in the buffer
 *  capacity: max number of elements 
 *  buffer: blob of data
 * */

typedef struct{
  int readIt;
  int writeIt;
  unsigned int size;
  unsigned int capacity;
  char* buffer;
} circularBuffer;

/*
 * function createCircularBuffer
 * create a circular buffer with an specific capacity
 *
 * parameters:
 *  capacity: max number of elements to be stored in the buffer
 * */
circularBuffer createCircularBuffer(const unsigned int capacity) ;
/*
 * function cleanCircularBuffer
 * releases de memory allocated by the buffer
 *
 * parameters:
 *  cb: address of circularBuffer to be clean
 * */
void cleanCircularBuffer(circularBuffer* cb);
/*
 * function addCircularBuffer
 * add element to the buffer
 * if the buffer has no available space the oldest element is automatically
 * removed to fit the new element
 *
 * parameters:
 *  cb: address of circularBuffer
 *  data: address of memory to be added
 *  N: number of bytes to add to the buffer
 * */
void addCircularBuffer(circularBuffer *cb, char *data, const unsigned int N) ;
/*
 * function getCharAt
 * returns the char at a given position in the buffer
 *
 * parameters:
 *  cb: address of circularBuffer
 *  index: position of the element
 *
 * */
char getCharAt(circularBuffer*cb , const unsigned int index);
/*
 * function resizeCircularBuffer
 * changes the capacity of the buffer,
 * if new capacity is grater that old capacity, all elements will be copied in the new buffer
 * if not, only the oldest elements that fit in new available memory will remain in buffer
 *
 * parameters:
 *  cb: address of circularBuffer
 *  newCapacity: newCapacity of the buffer
 *
 * */
void resizeCircularBuffer(circularBuffer*cb, const unsigned int newCapacity);
/*
 * function printCircularBuffer
 * prints visual representation of a circularBuffer
 *
 * parameters:
 *  cb: address of circularBuffer
 *
 * */
void printCircularBuffer(circularBuffer*cb );

