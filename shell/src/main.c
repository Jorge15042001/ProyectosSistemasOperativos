#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"

int pipesId[2];

void printPipeContent(int pipeid);

int main(const int argc, const char *const *const argv) {

  // TODO:create pipes
  if (pipe(pipesId) < 0) {
    fprintf(stderr, "Pipe error\n");
    _exit(1);
  }
  // shell process never writes
  close(pipesId[1]);
  int status;

  while (1) {
    String promt = getPromt();
    printf("%s",promt.c_string);
    // Read commannd from stdin
    String commandString = getCommandString();
    // terminate if command == exit
    if (strcmp(commandString.c_string, "exit") == 0) {
      printf("Exitting shell\n");
      _exit(0);
    }
    //  parse command
    ParsedCommand ps = ParseCommand(&commandString);

    //  execute the command
    const pid_t childID = executeCommand(&ps, pipesId[0], pipesId[1]);
    // wait for the child process
    waitpid(childID, &status, 0);

    // print the output of the command
    printPipeContent(pipesId[0]);

    // release resources
    cleanString(&promt);
    cleanString(&commandString);
    cleanParseCommand(&ps);
  }
  close(pipesId[0]);
}

void printPipeContent(int pipeid) {
  const unsigned int buffsize = 32;
  char reading_buf[buffsize];
  while (read(pipeid, reading_buf, buffsize) > 0) {
    write(1, reading_buf, buffsize); // 1 -> stdout
  }
}
