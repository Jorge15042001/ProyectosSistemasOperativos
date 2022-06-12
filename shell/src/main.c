#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"

int pipesId[2];

void printPipeContent(int pipeID) {
  const unsigned int buffSize = 32;
  char reading_buf[buffSize];
  while (read(pipeID, reading_buf, buffSize) > 0) {
    write(1, reading_buf, buffSize); // 1 -> stdout
  }
}

int main(const int argc, const char *const *const argv) {

  // TODO:create pipes
  if (pipe(pipesId) < 0) {
    fprintf(stderr, "Pipe error\n");
    _exit(1);
  }

  close(pipesId[1]);// shell process never writes
  int status;


  while (1) {
  // TODO: show command promt
    //Read commannd from stdin
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
    cleanString(&commandString);
    cleanParseCommand(&ps);
  }
  close(pipesId[0]);
}
