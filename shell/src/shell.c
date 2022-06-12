#include <malloc.h>
#include <alloca.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "shell.h"

unsigned int countRepetions(const String *const s, const char c) {
  unsigned int count = 0;

  for (int i = 0; i < s->length; i++) {
    if (s->c_string[i] == c) {
      count++;
    }
  }
  return count;
}

void cleanString(String *const s) {
  free(s->c_string);
  s->length = 0;
}

String getCommandString() {
  const unsigned int len_max = 128;
  unsigned int current_size = 0;

  char *pStr = malloc(len_max);
  current_size = len_max;

  if (pStr != NULL) {
    int c = EOF;
    unsigned int i = 0;
    // accept user input until hit enter or end of file
    while ((c = getchar()) != '\n' && c != EOF) {
      pStr[i] = (char)c;
      i++;
      // if i reached maximize size then realloc size
      if (i == current_size) {
        current_size += len_max;
        pStr = realloc(pStr, current_size);
      }
    }

    pStr[i] = '\0';
  }

  const String command = {pStr, strlen(pStr)};

  return command;
}
ParsedCommand ParseCommand(const String *const command) {

  // find the number of word* in the string
  const unsigned int nTokens = countRepetions(command, ' ');
  char **tokens =
      (char **)malloc((nTokens + 2) * sizeof(char **)); // space to store tokens

  // copy the string because strtok destroys it

  char *commandStrCopy = (char *)alloca((command->length + 1)*sizeof(char));
  strcpy(commandStrCopy, command->c_string); // copy the command string

  unsigned int argumentsCount = 0;

  char *token;

  token = strtok(commandStrCopy, " ");
  while (token != NULL) {
    tokens[argumentsCount] = malloc(strlen(token)+1);
    strcpy(tokens[argumentsCount], token);
    argumentsCount++;

    token = strtok(NULL, " ");
  }
  tokens[argumentsCount]=NULL;//null terminated argv

  for (long i = 0;i<100000000;i++){}

  // release resources
  /** free(commandStrCopy); */
  ParsedCommand pc = {tokens[0], tokens,argumentsCount};
  return pc;
}
void cleanParseCommand(ParsedCommand *const ps) { 
  // free all memory given to individual tokens

  for (int i=0; i<ps->argc;i++){
    free(ps->arguments[i]);
  }

  //free the memory given to the array 
  free(ps->arguments); 
}

pid_t executeCommand(ParsedCommand *const ps, int pipeInput, int pipeOutput){
  pid_t childId = fork();

  if (childId == -1){
    fprintf(stderr,"Fork error\n") ;
    _exit(2);

  }

  if (childId == 0){//childe process
    close(pipeInput);// child does not read
    dup2(pipeOutput, 1);// redirect staout
    execvp(ps->executable, ps->arguments);//
    fprintf(stderr, "execvp error\n");
    _exit(3);
  }

  return childId;


}
