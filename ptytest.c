#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

static
void get_a_pty(){
  int master = posix_openpt(O_RDWR | O_NOCTTY);
  if (master < 0){
    int error = errno;
    printf("posix_openpt(O_RDWR | O_NOCTTY) failed: %d (%s)\n",
           error, strerror(error)); fflush(stdout);
    return;
  }
  printf("posix_openpt(O_RDWR | O_NOCTTY) succeeded with FD %d", master);
  const char* slaveName = ptsname(master);
  if (0 == slaveName){
    int error = errno;
    printf("; ptsname(%d) failed: %d (%s)\n", master, error, strerror(error));
    fflush(stdout);
    return;
  }
  printf("; slave is %s", slaveName);
  if (0 != grantpt(master)){
    int error = errno;
    printf("; grantpt(%d) failed: %d (%s)\n", master, error, strerror(error));
    fflush(stdout);
    return;
  }
  printf("; grantpt(%d) OK", master);
  if (0 != unlockpt(master)){
    int error = errno;
    printf("; unlockpt(%d) failed: %d (%s)\n", master, error, strerror(error));
    fflush(stdout);
    return;
  }
  printf("; unlockpt(%d) OK", master);
  int slave = open(slaveName, O_RDWR);
  if (slave < 0){
    int error = errno;
    printf("; open(%s) failed: %d (%s)\n", slaveName, error, strerror(error));
    fflush(stdout);
    return;
  }
  printf("; open(%s, O_RDWR) succeeded with FD %d\n", slaveName, slave);
  fflush(stdout);
}

static int MAX_ALLOWED = 30;

int main(int argc, char**argv){
  int howMany = (argc > 1 ? atoi(argv[1]) : 1);
  if ((howMany < 0) || (howMany > MAX_ALLOWED)){
    printf("The number of PTYs requested (%d) must be >0 and <%d\n", howMany, MAX_ALLOWED);
    return 1;
  }
  int i;
  for (i = 0; i < howMany; ++i){
    get_a_pty();
  }

  if (howMany > 0){
    printf("press enter to exit"); fflush(stdout);
    char buf[10];
    fread(buf, 1, 1, stdin);
  }
  return 0;
}
