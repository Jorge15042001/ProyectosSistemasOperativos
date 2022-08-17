# Design


## Clients:

all clients are individual processes that connect to the main controller via sockets

## Server:

* DataListener:
  * There is one data listener per sensor, 
  * The DataListerner stores the message it receives with the time it arrives in a buffer

* Calculator:
  * This process is activated when a new lydar measurement have arrived
  * Reads from the buffers of DataListerner to perform its calculations -> produces a result
  * stores the data in disk 

* Cleaner:
  * Removes the data that is too old


### Synchronization problem

Read, write and remove operations to the buffers of each DataListerner must be Synchronize


