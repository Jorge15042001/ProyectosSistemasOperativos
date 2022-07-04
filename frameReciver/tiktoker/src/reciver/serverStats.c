#define _GNU_SOURCE

#include "serverStats.h"

#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

serverStats createServerStats(const int cap) {
  connectionState *states = malloc(cap * sizeof(connectionState));

  serverStats ss;
  ss.connections = states;
  ss.size = 0;
  ss.cap = cap;
  pthread_mutex_init(&ss.serverStatsMutex, NULL);

  return ss;
}

int registerNewConnection(serverStats *ss) {
  pthread_mutex_lock(&ss->serverStatsMutex);
  if (ss->size == ss->cap) {
    ss->connections =
        realloc(ss->connections, (ss->cap) * sizeof(connectionState) * 2);
    ss->cap *= 2;
  }
  // initialize members
  ss->connections[ss->size].recivedFrames = 0;
  ss->connections[ss->size].totalFrames = 0;
  ss->connections[ss->size].StiilConnected = 1;
  ss->connections[ss->size].connectionType = 0;
  ss->connections[ss->size].buffSize = 0;
  ss->connections[ss->size].buffUsage = 0;

  gettimeofday(&ss->connections[ss->size].startTime, NULL);
  gettimeofday(&ss->connections[ss->size].lastPackageTime, NULL);

  ss->size++;
  pthread_mutex_unlock(&ss->serverStatsMutex);
  // return the index of new connection
  return ss->size - 1;
}

connectionState *getState(serverStats *ss, int i) {
  return ss->connections + i;
}
void cleanServerStats(serverStats *ss) {
  pthread_mutex_destroy(&ss->serverStatsMutex);
  free(ss->connections);
  ss->size = 0;
  ss->cap = 0;
}
int countActiveConnections(serverStats *ss) {
  int count = 0;
  for (int i = 0; i < ss->size; i++) {
    connectionState *cs = getState(ss, i);
    if (cs->StiilConnected == 1)
      count++;
  }
  return count;
}
int countUnActiveConnections(serverStats *ss) {
  int count = 0;
  for (int i = 0; i < ss->size; i++) {
    connectionState *cs = getState(ss, i);
    if (cs->StiilConnected == 0)
      count++;
  }
  return count;
}
void registerLatestFrameTime(connectionState *cs) {
  gettimeofday(&cs->lastPackageTime, NULL);
}

int countConectionsType(serverStats *ss, const int t) {
  int count = 0;
  for (int i = 0; i < ss->size; i++) {
    connectionState *cs = getState(ss, i);
    if (cs->connectionType == t)
      count++;
  }
  return count;
}
int getTotalMemoryBuff(serverStats *ss) {
  int total = 0;
  for (int i = 0; i < ss->size; i++) {
    connectionState *cs = getState(ss, i);
    total += cs->buffSize;
  }
  return total;
}

char *getReport(serverStats *ss) {
  const int buffMem = getTotalMemoryBuff(ss);
  const int setConsCount = countConectionsType(ss, SET_CONNECTION);
  const int monConsCount = countConectionsType(ss, MONITOR_CONNECTION);
  const int frameConsCount = countConectionsType(ss, FRAME_CONNECTION);
  const int activeConnections = countActiveConnections(ss);
  const int unactiveConnections = countUnActiveConnections(ss);
  const int totalConnections = activeConnections + unactiveConnections;

  char *activeReport = malloc(1);
  activeReport[0] = 0;
  for (int i = 0; i < ss->size; i++) {
    connectionState *cs = getState(ss, i);
    if (cs->StiilConnected && cs->connectionType == FRAME_CONNECTION) {
      char *connReport;
      asprintf(
          &connReport,
          "\tFrameThread %3d memory: %d\tusage: %3.2f\t frames: %3d/%3d\tavarage "
          "velocity: %1.3f\tframes/s\t estimated time: %3.1fseconds\n",i,
          cs->buffSize, cs->buffUsage * 100, cs->recivedFrames, cs->totalFrames,
          cs->avgFrameTime,
          (1/cs->avgFrameTime) * ((cs->totalFrames) - (cs->recivedFrames)));
      activeReport =
          realloc(activeReport, strlen(activeReport) + strlen(connReport) + 2);
      strcat(activeReport, connReport);
      free(connReport);
    }
  }
  char *mainReport;
  asprintf(&mainReport,
           "Memory used by buffers: %d bytes\t\tRequest count[frame: %d\tset: "
           "%d\tmonitor: %d]\nTotal connections %d\t\t[active: %d\tunactive: "
           "%d]\n\nActive processes:\n%s\n",
           buffMem, frameConsCount, setConsCount, monConsCount,
           totalConnections, activeConnections, unactiveConnections,
           activeReport);

  free(activeReport);
  return mainReport;
}
