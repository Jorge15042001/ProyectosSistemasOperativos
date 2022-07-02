#include "serverStats.h"

#include <malloc.h>

serverStats createServerStats(const int cap){
  connectionState * states = malloc(cap);
  serverStats ss = {states,0,cap};
  return ss;
}

connectionState* addNewState (serverStats* ss){
  if (ss->size == ss->cap){
    ss->connections = realloc(ss->connections, ss->cap*2);
    ss->cap *= 2;
  }
  ss->size ++;
  return ss->connections+ss->size-1;
}

connectionState* getState (serverStats* ss,int i){
  return ss->connections+i;
}
void cleanServerStats (serverStats* ss){
  free(ss);
}
int countActiveConnections (serverStats* ss){
  int count = 0;
  for (int i = 0;i<ss->size; i++){
    connectionState* cs = getState(ss, i);
    if (cs->StiilConnected== 1) count ++;
  }
  return count;
}
