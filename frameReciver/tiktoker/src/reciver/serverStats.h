#pragma once
#include <sys/time.h>
#include <pthread.h>

//lock for reading and writing to serverStats

//constants
static const int MONITOR_CONNECTION = 1;
static const int FRAME_CONNECTION = 2;
static const int SET_CONNECTION = 3; 

typedef struct {
  int recivedFrames;
  int totalFrames;
  float avgFrameTime;
  int StiilConnected;
  int connectionType;
  struct timeval lastPackageTime;
  struct timeval startTime;
  int buffSize;
  float buffUsage;

}connectionState ;

typedef struct{
  pthread_mutex_t serverStatsMutex;
  connectionState * connections;
  int size ;
  int cap;
}serverStats;

serverStats createServerStats(const int cap);
void cleanServerStats (serverStats*);
int registerNewConnection (serverStats*);
connectionState* getState (serverStats*,int);

int countActiveConnections (serverStats*);
void registerLatestFrameTime(connectionState*);
char * getReport(serverStats*);
int countConectionsType(serverStats* ss,const int t);
int getTotalMemoryBuff(serverStats* ss);
int countUnActiveConnections (serverStats* ss);
