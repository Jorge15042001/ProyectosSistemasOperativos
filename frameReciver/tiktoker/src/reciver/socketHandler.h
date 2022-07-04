#pragma once
#include "threadQueue.h"
#include "serverStats.h"

typedef struct {
  int sockfd;
  threadQueue* tq;
  threadNode* tn;
  serverStats *stats;
}threadArgs ;

int startSocket(const int port);
void *serveConection(void *param);
