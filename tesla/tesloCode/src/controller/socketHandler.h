#pragma once

#include "measurement.h"
#include "measurementList.h"

#include <pthread.h>


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

/*
 * function acceptConnection
 *
 * accepts a new connection and returns its fileDiscriptor
 *
 * parameters:
 *  ServerFD: int FileDescriptor of the server
 *
 * */
int acceptConnection (const int ServerFD);


/*
 * struct ConnectionNode
 *
 * represents each connection that the server has started
 *
 * fields:
 *  listenNode: pthread_t of the thread that is atteding the connection
 *  next: reference to the next connection
 * */

typedef struct ConnectionNode ConnectionNode;
struct ConnectionNode{
  pthread_t listenNode;
  int clientFD;
  ConnectionNode * next;
  MeasurementList measurements;
};

/*
 * struct ConnectionList
 *
 * list of connections the server has
 *
 * fields:
 *  first: reference to the first ConnectionNode node
 *  length: number of connections in the list
 * */
typedef struct {
  ConnectionNode * first;
  size_t length;
}ConnectionList;

/*
 * function registerNewConnection
 *
 * adds a new connection in the list of connections and 
 * retunrs a pointer to the new connection
 *
 * parameters:
 *  ConnectionList: connctionList in which to register the new connection
 *  clientFD: fileDiscriptor of the acccepted connection
 *
 * */
ConnectionNode* registerNewConnection(ConnectionList *const, const int clientFD);


extern ConnectionList connections;
