#include "threadQueue.h"
#include <stdio.h>
#include <stdlib.h>

threadQueue createThreadQueue(const int N) {
  threadNode *first = NULL;
  threadNode *prev = NULL;
  for (int i = 0; i < N; i++) {
    threadNode *node_ptr = malloc(sizeof(threadNode));
    if (prev == NULL) {
      first = node_ptr;
    } else {
      prev->next = node_ptr;
    }

    prev = node_ptr;
    prev->next = NULL;
  }
  threadQueue res = {first, N};
  return res;
}

void cleanThreadQueue(threadQueue *tq) {
  threadNode *this = tq->first;
  threadNode *next;

  while (this != NULL) {
    next = this->next;
    free(this);
    this = next;
  }
  tq->first = NULL;
  tq->length = 0;
}
threadNode *getReadyThread(threadQueue *tq) {
  if (tq->length == 0) { // no avilable thread
    // create a new thread
    threadNode *newNode = malloc(sizeof(threadNode));
    // and return it
    return newNode;
  }

  threadNode *front = tq->first;
  tq->first = tq->first->next;
  tq->length--;
  return front;
}
void addThread(threadQueue *tq, threadNode *new) {
  threadNode *old = tq->first;
  new->next = old;
  tq->first = new;
  tq->length++;
}
