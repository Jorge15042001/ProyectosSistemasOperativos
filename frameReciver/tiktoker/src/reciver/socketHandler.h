#pragma once
#include "threadQueue.h"

typedef struct {
  int sockfd;
  threadQueue* tq;
  threadNode* tn;
}threadArgs ;

int startSocket(const int port);
void *serveConection(void *param);
