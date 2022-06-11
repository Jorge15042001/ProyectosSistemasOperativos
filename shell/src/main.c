#include <stdio.h>

#include "shell.h"

int main(const int argc, const char * const * const argv){
  //TODO: show command promt

  //TODO: read command from stdin
  String commandString = getCommandString();
  //TODO: parse command
  ParsedCommand ps = ParseCommand(&commandString);

  //TODO: execute the command
  //TODO: print the output of the command
  //TODO: implement pipe and dup

  //TODO: release resources
  cleanString(&commandString);
  cleanParseCommand(&ps);
}

