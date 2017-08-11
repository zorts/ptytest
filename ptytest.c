#define _XOPEN_SOURCE 600
#define _POSIX_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

static
bool get_a_pty(const char* which){
  int flags = O_RDWR | O_NOCTTY;
  int master = -1;
  if (which){
    master = open(which, flags);
    if (master < 0){
      int error = errno;
      printf("open(%s, O_RDWR | O_NOCTTY) failed: %d (%s)\n",
             which, error, strerror(error)); 
      fflush(stdout);
      return false;
    } else{
      printf("open(%s, O_RDWR | O_NOCTTY) succeeded with FD %d", master);
    }
  } else{
    master = posix_openpt(O_RDWR | O_NOCTTY);
    if (master < 0){
      int error = errno;
      printf("posix_openpt(O_RDWR | O_NOCTTY) failed: %d (%s)\n",
             error, strerror(error));
      fflush(stdout);
      return false;
    } else{
      printf("posix_openpt(O_RDWR | O_NOCTTY) succeeded with FD %d", master);
    }
  }

  const char* slaveName = ptsname(master);
  if (0 == slaveName){
    int error = errno;
    printf("; ptsname(%d) failed: %d (%s)\n", master, error, strerror(error));
    fflush(stdout);
    return false;
  }
  printf("; slave is %s", slaveName);
  if (0 != grantpt(master)){
    int error = errno;
    printf("; grantpt(%d) failed: %d (%s)\n", master, error, strerror(error));
    fflush(stdout);
    return false;
  }
  printf("; grantpt(%d) OK", master);
  if (0 != unlockpt(master)){
    int error = errno;
    printf("; unlockpt(%d) failed: %d (%s)\n", master, error, strerror(error));
    fflush(stdout);
    return false;
  }
  printf("; unlockpt(%d) OK", master);
  int slave = open(slaveName, O_RDWR);
  if (slave < 0){
    int error = errno;
    printf("; open(%s) failed: %d (%s)\n", slaveName, error, strerror(error));
    fflush(stdout);
    return false;
  }
  printf("; open(%s, O_RDWR) succeeded with FD %d\n", slaveName, slave);
  fflush(stdout);
  return true;
}

static void usage(const char* pgmName){
  fprintf(stderr, 
          "usage: %s\n"
          "       [-s xx]      try to seize the PTY with the suffix 'xx'\n"
          "       [-g n]       get the requested number of PTYs\n"
          "       [-w]         once the requested PTY(s) are acquired, wait until killed\n"
          "       [-h]         produce this message and exit\n"
          "\n"
          , pgmName);
}

static int MAX_ALLOWED = 30;
static char template[] = "/dev/ptyp00%s";

int main(int argc, char**argv){
  opterr = 0; /* disable auto error reporting */
  char opt = 0;
  /* These copies are needed because optind and optarg aren't
     necessarily visible to debuggers, and you often want them. */
  int myoptind = 1;
  char* myoptarg = 0;

  bool seize = false;
  bool get = false;
  bool wait = false;
  int howMany = 0;
  char name[100];
  
  while (((char) -1) != (opt = (char) getopt(argc, argv, "s:g:wh"))){
    myoptind = optind;
    myoptarg = optarg;

    switch(opt){

    case 'h':
      usage(argv[0]);
      return 1;

    case 's':
      if (strlen(myoptarg) != 2){
        fprintf(stderr, 
                "option -s %s invalid; the suffix must be exactly 2 characters long\n",
                myoptarg);
        usage(argv[0]);
        return 1;
      }
      sprintf(name, template, myoptarg);
      seize = true;
      break;

    case 'g':
      howMany = atoi(myoptarg);
      if ((howMany < 0) || (howMany > MAX_ALLOWED)){
        fprintf(stderr,
                "the number of PTYs requested (%d) must be >0 and <%d\n",
                howMany, MAX_ALLOWED);
        usage(argv[0]);
        return 1;
      }
      get = true;
      break;

    case 'w':
      wait = true;
      break;

    default:
      {
        usage(argv[0]);
        return 1;
      }
    }
  }

  if (myoptind < argc){
    fprintf(stderr, "unused arguments:");
    while (myoptind < argc){
      fprintf(stderr, " %s", argv[myoptind++]);
    }
    fprintf(stderr, "\n");
    usage(argv[0]);
    return 1;
  }

  if (!(seize ^ get)){
    usage(argv[0]);
    return 1;
  }

  if (get){
    int i;
    for (i = 0; i < howMany; ++i){
      if (!get_a_pty(0)){
        fprintf(stderr,
                "PTY acquisition failed after getting %d PTYs\n", i);
        return 1;
      }
    }
  } else{
    if (!get_a_pty(name)){
        fprintf(stderr,
                "unable to seize PTY %s\n", name);
        return 1;
    }
  }
  if (wait){
    pause();
  }
  return 0;
}
