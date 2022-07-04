#pragma once

#include <pthread.h>
typedef struct threadNode threadNode;

/*
 * struct threadNode
 * it is a node for the queue of connection
 *
 * members:
 *  next: pointer to next item 
 *  thread: pthread_t 
 *
 * */
struct threadNode {
  threadNode *next;
  pthread_t thread;
};


/*
 * struct threadQueue
 *
 * members:
 *  first: top of the queue
 *  length: number of elements in the queue
 * */
typedef struct {
  threadNode *first;
  unsigned int length ;
} threadQueue;

/*
 * function createThreadQueue 
 *
 * returns a thread queue to be used for multithread applications
 * resizes to fit as many threads as needed
 *
 * parameters:
 *  N: initial number of threads
 *
 * returns:
 *  an initialized threadQueue
 * */
threadQueue createThreadQueue(const int N);
/*
 * function cleanThreadQueue
 * releases the resources that holds the threadQueue
 *
 * parameters:
 *  tq: pointer to threadQueue to be clean
 *
 * */
void cleanThreadQueue(threadQueue *tq);
/*
 * function getReadyThread
 * gets one threas ready to be executed, 
 * if no threads are available a new one is created and returned
 *
 * parameters:
 *  tq: from where to take the thread
 *
 * returns:
 *  a thread node with a pthread_t ready to be used
 * */
threadNode *getReadyThread(threadQueue *tq);

/*
 * function addThread
 *
 * adds a threadNode to the queue of ready threads
 *
 * parameters:
 *  tq: threadQueue in which to add the node
 *  tn: threadNode to be added in the queue
 *
 * */
void addThread(threadQueue *tq, threadNode *tn);
