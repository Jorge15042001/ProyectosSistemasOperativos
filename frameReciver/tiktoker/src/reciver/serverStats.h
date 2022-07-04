#pragma once
#include <sys/time.h>
#include <pthread.h>

//lock for reading and writing to serverStats

//constants
//enums for hadling types of connections
static const int MONITOR_CONNECTION = 1;
static const int FRAME_CONNECTION = 2;
static const int SET_CONNECTION = 3; 

/*
 * struct connectionState
 * This is used to handle the state of each connection the server receives
 *
 * members:
 *  recivedFrames: frames that have been recived By the server
 *  totalFrames: total number of frames to be received
 *  avgFrameTime: velocity of frames
 *  StiilConnected: shows if the connection is still active
 *  connectionType: indicates what type of connection is
 *  lastPackageTime: indicates the time the last package arrived
 *  startTime: indicates the time the connection started
 *  buffSize: indicates the memory being used by the connection
 *  buffUsage: indicates the percentage of memory being used
*/
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
/*
 * struct serverStats
 *
 * holds the data required to run statistics of the server
 *
 * members:
 *  serverStatsMutex: control access to the share state
 *  connections: array with the list of all connections the server has received while running
 *  size: number of connections 
 *  cap: max number of connections before resizing
 *
 * */

typedef struct{
  pthread_mutex_t serverStatsMutex;
  connectionState * connections;
  int size ;
  int cap;
}serverStats;

/*
 * function createServerStats
 * initializes and returns serverStats struct with space for (cap) connections
 *
 * prameters:
 *  cap: initial number of connections, the buffer will be resized if necessary
 * */
serverStats createServerStats(const int cap);

/*
 * function cleanServerStats
 * releases memory 
 *
 * prameters:
 *  ss: serverStats to be cleaned
 * */
void cleanServerStats (serverStats*);
/*
 * function registerNewConnection 
 *
 * registers that a new connection have been initiated
 *
 * prameters:
 *  serverStats: serverStats in which the connection will be register
 *
 * returns:
 *  index of connectionState struct that holds the connection data
 *
 * */
int registerNewConnection (serverStats*);
/*
 * function getState
 * returns pointer to the connectionState stored at an specific location
 *
 * prameters:
 *  serverStats: serverStats in which the connection is registered
 *  index of connectionState struct that holds the connection data
 *
 * returns:
 *  pointer to the object
 *
 * */
connectionState* getState (serverStats*,int);

/*
 * function countActiveConnections
 * returns the total number of open connections in the server
 *
 * prameters:
 *  serverStats: serverStats in which to count the connections
 *
 * returns:
 *  number of open connections in the server
 *
 * */
int countActiveConnections (serverStats*);
/*
 * function registerLatestFrameTime
 * register the arrival time of last package in for a given connection
 *
 * prameters:
 * connectionState: pointer to connectionState to store the data in
 *
 * */
void registerLatestFrameTime(connectionState*);
/*
 * function getReport
 * makes a text report for the current state of the server
 *
 * prameters:
 *  serverStats: serverStats to make the report
 *
 * returns:
 *  a char* with the text report, this pointer must be free
 * */
char * getReport(serverStats*);
/*
 * function countConectionsType
 * counts the total number of connections for a given type
 *  
 * prameters:
 *  serverStats: serverStats in which to count connections of type
 *  t: type 
 *
 * returns:
 *  total number of connections for type t
 *
 * */
int countConectionsType(serverStats* ss,const int t);
/*
 * function getTotalMemoryBuff
 * computes the memory that is being used by the server
 *
 * parameters:
 *  serverStats: serverStats to compute memory usage
 *  
 * returns:
 *  total number of bytes used in the server
 *
 * */
int getTotalMemoryBuff(serverStats* ss);
/*
 * function countUnActiveConnections
 * returns the total number of close connections in the server
 *
 * prameters:
 *  serverStats: serverStats in which to count the connections
 *
 * returns:
 *  number of close connections in the server
 *
 * */

int countUnActiveConnections (serverStats* ss);
