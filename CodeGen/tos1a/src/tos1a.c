#include <pyblock.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

//#define TOS1A_DEBUG
#ifdef TOS1A_DEBUG
#define printf(...) printf(__VA_ARGS__)
#else
#define printf(...)
#endif

char * doCrc(char * command) {
  int result = 0;

  for (int i = 0; i < strlen(command); i++) {
    result = result ^ command[i];
  }

  char * crc = (char *) malloc(sizeof(char)*2);
  sprintf(crc, "%02X", result);
  return crc;
}

char * createCommandInit() {
  return "$SSE*45\n";
}

char * createCommandExit() {
  return "$SEE*53\n";
}

// mode 0: SFP, 1: SGV
char * createCommandSetup(python_block * block, int mode) {
  char command[100];

  if (mode == 1) {
    double **u = block->u;

    double lamp = u[0][0];
    double vent = u[1][0];
    double temp = u[2][0];

    // minumum is 0 and maximum is 100
    if (lamp < 0) { lamp = 0; }
    if (lamp > 100) { lamp = 100; }
    if (vent < 0) { vent = 0; }
    if (vent > 100) { vent = 100; }
    if (temp < 0) { temp = 0; }
    if (temp > 100) { temp = 100; }

    sprintf(command, "SGV,%2.2f,%2.2f,%2.2f", lamp, vent, temp);
  } else {
    double * realPar = block -> realPar;

    double lamp = realPar[0];
    double vent = realPar[1];
    double temp = realPar[2];

    // minumum is 0.5 and maximum is 10
    if (lamp < 0.5) { lamp = 0.5; }
    if (lamp > 10) { lamp = 10; }
    if (vent < 0.5) { vent = 0.5; }
    if (vent > 10) { vent = 10; }
    if (temp < 0) { temp = 0; }
    if (temp > 100) { temp = 100; }

    sprintf(command, "SFP,%2.2f,%2.2f,%2.2f", lamp, vent, temp);
  }

  char * crc = doCrc(command);

  // return the command with the crc "$%s*%s\n"
  char * commandWithCrc = (char *) malloc(sizeof(char)*(strlen(command)+strlen(crc)+3));
  sprintf(commandWithCrc, "$%s*%s\n", command, crc);

  return commandWithCrc;
}

// command is a string in format $%2.2f,%2.2f,%2.4f,%2.2f,%2.4f,%2.2f,%3.4f,%2.2f,%3.4f,%2.2f,%2.2f,%2.2f,%1.4f,%4.2f,%6.4f*CRC\n
int readCommand(char * command, double * result) {
  // command should be valid
  if (command[0] != '$') {
    return 0;
  }

  // remove the $
  command++;

  // remove the crc
  char * crc = strchr(command, '*');
  *crc = '\0';

  printf("Response: %s\n", command);

  char * token = strtok(command, ",");
  int i = 0;
  while (token != NULL) {
    result[i] = atof(token);
    token = strtok(NULL, ",");
    i++;
  }

  return 1;
}

static void * getData(void * p)
{
  int i;
  python_block *block = (python_block *) p;
  int recv_len, maxlen;
  int fd = block->intPar[0];
  double *y;
  double data[block->nout];

  maxlen = block->nout*sizeof(double);

  while(1){
    recv_len = read(fd, (char *) data, maxlen);

    double results[15];
    int valid = readCommand(data, results);
    if (valid == 0) continue;

    for (i = 0; i < block->nout; i++) {
      y = block->y[i];
      y[0] = results[i];
    }

  }
}

static void init(python_block * block) {
  char * port = (char * ) block->str;
  int * intPar    = block->intPar;
  int fd;
  struct termios ts;
  pthread_t thrd;

  fd = open(port, O_RDWR);
  if (fd == -1) {
    fprintf(stderr, " Error opening USB\n");
    exit(1);
  }

  tcgetattr(fd, & ts);
  cfsetispeed( & ts, B115200);
  ts.c_cflag = (ts.c_cflag & ~CSIZE) | CS8;
  ts.c_iflag &= ~IGNBRK;
  ts.c_lflag = 0;

  ts.c_oflag = 0;
  ts.c_cc[VMIN] = 1;
  ts.c_cc[VTIME] = 100;

  ts.c_iflag &= ~(IXON | IXOFF | IXANY);

  ts.c_cflag |= (CLOCAL | CREAD);

  ts.c_cflag &= ~(PARENB | PARODD);
  ts.c_cflag |= 0;
  ts.c_cflag &= ~CSTOPB;
  ts.c_cflag &= ~CRTSCTS;
  tcsetattr(fd, TCSANOW, & ts);

  intPar[0] = fd;
  pthread_create( & thrd, NULL, getData, (void * ) block);

  char * setupCommand = createCommandSetup(block, 0);
  printf("Setup: %s", setupCommand);
  char * initCommand = createCommandInit();
  printf("Init: %s", initCommand);

  write(fd, setupCommand, strlen(setupCommand));
  write(fd, initCommand, strlen(initCommand));
}

static void inout(python_block * block) {
  /* double * realPar = block->realPar; */
  /* int * intPar    = block->intPar; */
  /* double *y = block->y[0]; */

  int * intPar    = block->intPar;
  char * setupCommand = createCommandSetup(block, 1);
  printf("Send: %s", setupCommand);
  write(intPar[0], setupCommand, strlen(setupCommand));
}

static void update(python_block * block) {
  /* double * realPar = block->realPar; */
  /* int * intPar    = block->intPar; */
  /* double *y = block->y[0]; */
  /* double *u = block->u[0]; */

}

static void end(python_block * block) {
  int * intPar    = block->intPar;

  char * exitCommand = createCommandExit();
  printf("Exit: %s", exitCommand);
  write(intPar[0], exitCommand, strlen(exitCommand));
  close(intPar[0]);

}

void tos1a(int flag, python_block * block) {
  if (flag == CG_OUT) {
    /* get input */
    inout(block);
  } else if (flag == CG_STUPD) {
    update(block);
  } else if (flag == CG_END) {
    /* termination */
    end(block);
  } else if (flag == CG_INIT) {
    /* initialisation */
    init(block);
  }
}
