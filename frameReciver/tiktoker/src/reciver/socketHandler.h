#pragma once
#include "threadQueue.h"
#include "serverStats.h"

/*
 * struct threadArgs
 * struct that is passed to a thread, to attend each connection
 *
 * members:
 *  sockfd, clientSocket to read from
 *  tq: queue of threads 
 *  tn: threadNode in which the connection in being processed
 *  stats: server statistics
 * */

typedef struct {
  int sockfd;
  threadQueue* tq;
  threadNode* tn;
  serverStats *stats;
}threadArgs ;

/*
 * function startSocket
 * start a socket connection in localhost at a given port
 *
 * parameters:
 *  port: port in which to run the server
 *
 * returns:
 *  the socket in which the server has started
 * */

int startSocket(const int port);

/*
 * function serveConection
 *
 * thread function that attends connections
 *
 * parameters:
 *  param: struct threadArgs with parameters for the thread
 *
 * */
void *serveConection(void *param);
