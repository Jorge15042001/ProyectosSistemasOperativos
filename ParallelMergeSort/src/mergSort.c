#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "mergSort.h"

// TODO: remove
#include <stdio.h> //TODO: rmeove
void printArray(int *blob, int length) {
  printf("[");
  for (int i = 0; i < length; i++) {
    printf("%d, ", blob[i]);
  }
  printf("]\n");
}

/*
 * struct: arrayView
 *
 * Simplifies managing arrays of data
 *
 * members:
 * *  start: pointer to the beginning of the section
 * *  length: number of elements in the section
 * */

typedef struct {
  int *mainArrayStart;
  int *start;
  unsigned int length;

} arrayView;

/*
 * function: merge
 *
 * perform the merging operation for the implementation of merge sort algorithm
 *
 * members:
 * *  tempArray: arrayView for the temporal memory nedeed to do the sorting
 * *  left, rigth: arrayViews to be merged
 * */
arrayView merge(arrayView tempArray, arrayView left, arrayView right) {

  //position
  const unsigned int stride = left.start - left.mainArrayStart;
  // number of elements after merging
  const unsigned int nElements = left.length + right.length;

  // start of temporal memory section available for this instance of merge
  // function
  int *const tempMemory = tempArray.start + stride;
  int *sorted_it = tempMemory;

  arrayView leftCopy = left;
  arrayView rightCopy = right;

  while (leftCopy.length > 0 && rightCopy.length > 0) {
    if (*leftCopy.start <= *rightCopy.start) {
      // copy from left to result
      *sorted_it = *leftCopy.start;
      sorted_it++;
      // remove first element from left
      leftCopy.length--;
      leftCopy.start++;
    } else {
      // copy from right to result
      *sorted_it = *rightCopy.start;
      sorted_it++;
      // remove first element from right
      rightCopy.length--;
      rightCopy.start++;
    }
  }
  // copy all remaining element on the left into the result
  for (int i = 0; i < leftCopy.length; i++) {
    *sorted_it = leftCopy.start[i];
    sorted_it++;
  }
  // copy all remaining element on the right into the result
  for (int i = 0; i < rightCopy.length; i++) {
    *sorted_it = rightCopy.start[i];
    sorted_it++;
  }
  // copy from temporal array to main array
  sorted_it = tempMemory;
  for (int i = 0; i < nElements; i++) {
    left.start[i] = sorted_it[i];
  }

  arrayView response = {left.mainArrayStart, left.start, nElements};
  return response;
}

/*
 * struct: mergeSortArgs
 *
 * struct with arguments for mergeSort_internal_multiThreaded function
 *
 * members:
 * *  tempArray: arrayView for a temporal array used by merge sort
 * *  data: arrayView for the main array that holds the data to be sorted
 * */
typedef struct {
  arrayView tempArray;
  arrayView data;
} mergeSortArgs;

/*
 * function: mergeSort_internal_multiThreaded
 *
 * Multithraded version of merge sort algorithm
 *
 * parameters:
 * *  data: arrayView created from blob of data send to
 *    caller function.
 *
 * */
void *mergeSort_internal_multiThreaded(void *args) {
  arrayView tempArray = ((mergeSortArgs *)args)->tempArray;
  arrayView data = ((mergeSortArgs *)args)->data;

  if (data.length <= 1) {
    pthread_exit(0);
  }

  const unsigned int middle = data.length / 2;

  const arrayView leftView = {data.mainArrayStart, data.start, middle};
  const arrayView rightView = {data.mainArrayStart, data.start + middle,
                               data.length - middle};

  // divide
  pthread_t leftThread;
  pthread_t rightThread;

  mergeSortArgs leftArgs = {tempArray, leftView};
  mergeSortArgs rightArgs = {tempArray, rightView};

  pthread_create(&leftThread, NULL, mergeSort_internal_multiThreaded,
                 (void *)&leftArgs);
  pthread_create(&rightThread, NULL, mergeSort_internal_multiThreaded,
                 (void *)&rightArgs);
  pthread_join(leftThread, NULL);
  pthread_join(rightThread, NULL);
  // conquer

  const arrayView response = merge(tempArray, leftView, rightView);

}


int *mergeSort(int *blob, const int length) {
  if (length == 0) {
    return 0;
  }
  if (length == 1) {
    int *dataCopy = (int *)malloc(sizeof(int));
    dataCopy[0] = blob[0];
    return dataCopy;
  }
  const arrayView d;

  int *blobCopy = (int *)malloc(length * sizeof(int));
  int *temp = (int *)malloc(length * sizeof(int));

  // copy the data from blob to sorted
  memcpy(blobCopy, blob, sizeof(int) * length);
  const arrayView data = {blobCopy, blobCopy, length};
  const arrayView tmpData = {temp, temp, length};
  mergeSortArgs msa = {tmpData, data};
  mergeSort_internal_multiThreaded(&msa);

  // release temporal memory
  free(temp);
  // sorted array must be release by the user
  return data.start;
}
