#pragma once

#include <pthread.h>
typedef struct threadNode threadNode;

struct threadNode {
  threadNode *next;
  pthread_t thread;
};

typedef struct {
  threadNode *first;
  unsigned int length ;

} threadQueue;

threadQueue createThreadQueue(const int N);
void cleanThreadQueue(threadQueue *tq);
threadNode *getReadyThread(threadQueue *tq);
void addThread(threadQueue *tq, threadNode *tn);
