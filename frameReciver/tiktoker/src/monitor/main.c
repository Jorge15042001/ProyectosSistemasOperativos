#define _GNU_SOURCE

#include <bits/types/struct_timeval.h>
#include <sys/select.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct {
  int watch;
  int setTimeOut;
  int setTimeRemove;
  int timeOut;
  int timeRemove;
} monitorArgs;

monitorArgs parseArgs(int argc, char **argv) {
  monitorArgs args;
  args.watch = 0;
  args.setTimeOut = 0;
  args.setTimeRemove = 0;
  args.timeRemove = 0;
  args.timeOut = 0;

  int tmpTimeOut;
  int tmpTimeRemove;

  int opt;
  while ((opt = getopt(argc, argv, "wt:r:")) != -1) {
    switch (opt) {
    case 'w':
      args.watch = 1;
      break;
    case 't':
      tmpTimeOut = atoi(optarg);

      if (tmpTimeOut == 0)
        break;

      args.setTimeOut = 1;
      args.timeOut = tmpTimeOut;
      break;
    case 'r':
      tmpTimeRemove = atoi(optarg);

      if (tmpTimeRemove == 0)
        break;

      args.setTimeRemove = 1;
      args.timeRemove = tmpTimeRemove;
      break;
    }
  }
  return args;
}

void watchServer() {
  const char *monitorMsg = "/monitor";
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    fprintf(stderr, "Socket creation failed.\n");
    return;
  }
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(7734);
  int len = sizeof(address);

  int result = connect(sockfd, (struct sockaddr *)&address, len);
  if (result == -1) {
    fprintf(stderr, "Error has occurred can't connect\n");
    close(sockfd);
    return;
  }
  const int rc = write(sockfd, monitorMsg, strlen(monitorMsg));
  if (rc == -1) {
    fprintf(stderr, "Failed to send message \n");
    close(sockfd);
    return;
  }
  int size;

  fd_set fds;
  FD_ZERO(&fds);
  FD_SET((unsigned int)sockfd, &fds);
  struct timeval tv = {3, 0};
  const int retval = select(FD_SETSIZE, &fds, NULL, NULL, &tv);
  if (retval == -1){
    fprintf(stderr,"select error\n");
  }
  else if (retval == 0) {
    fprintf(stderr, "Time out\n");
  } else {
    ioctl(sockfd, FIONREAD, &size);
    char *msg = malloc(size + 1);
    memset(msg, 0, size + 1);
    const int rc2 = read(sockfd, msg, size);
    printf("%s\n", msg);
    free(msg);
  }
  close(sockfd);
}

void setServerVar(char *var, int value) {
  const char *setMesgFormat = "/set/%s/%d";

  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    fprintf(stderr, "Socket creation failed.\n");
    return;
  }
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(7734);
  int len = sizeof(address);

  int result = connect(sockfd, (struct sockaddr *)&address, len);
  if (result == -1) {
    fprintf(stderr, "Error has occurred can't connect\n");
    close(sockfd);
    return;
  }

  char *setMessage;
  asprintf(&setMessage, setMesgFormat, var, value);
  const int rc = write(sockfd, setMessage, strlen(setMessage));
  if (rc == -1) {
    fprintf(stderr, "Failed to send message \n");
    close(sockfd);
    return;
  }
  free(setMessage);

  int resp;
  const int rc2 = read(sockfd, &resp, sizeof(resp));
  printf("setting %s to %d ->return %s\n", var, value,
         resp == 1 ? "OK" : "FAIL");

  close(sockfd);
}

int main(int argc, char **argv) {
  monitorArgs args = parseArgs(argc, argv);

  if (args.watch)
    watchServer();
  if (args.setTimeOut)
    setServerVar("timeout", args.timeOut);
  if (args.setTimeRemove)
    setServerVar("timeremove", args.timeRemove);
}
