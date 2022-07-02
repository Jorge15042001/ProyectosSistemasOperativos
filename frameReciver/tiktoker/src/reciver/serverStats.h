#pragma once

typedef struct {
  int recivedFrames;
  int totalFrames;
  float avgFrameTime;
  int StiilConnected;
}connectionState ;

typedef struct{
  connectionState * connections;
  int size ;
  int cap;
}serverStats;

serverStats createServerStats(const int cap);
connectionState* addNewState (serverStats*);
connectionState* getState (serverStats*,int);
void cleanServerStats (serverStats*);

int countActiveConnections (serverStats*);
