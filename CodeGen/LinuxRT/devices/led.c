#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sched.h>

#include <pyblock.h>

/* sth to convert number macros to strings */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/* we are only expecting and wanting one connector so backlog is 1 */
#define SOCKET_NAME "./led_sock"
#define SOCK_NAME_MAX_LEN 108
#define BACKLOG 1

/* how many values to send for each stream to led */
#define DOUBLE_SIZE sizeof(double)

#define PYCONTROL_PATH_ENV_VAR "PYSUPSICTRL"
#define LED_COMMAND_ARGV_NUM 4
/* when compiling define LED_SCRIPT */

struct _led {
  int sock;
  size_t buff_pos;
  size_t buff_len;
  char sock_name[SOCK_NAME_MAX_LEN];
  char * buff;
};

static int led_init(python_block * blk);
static void led_out(python_block * blk);
static void led_end(python_block * blk);

void led(int flag, python_block * blk) 
{
  switch (flag) {
  case CG_INIT:
    led_init(blk);
    break;
  case CG_OUT:
    led_out(blk);
    break;
  case CG_END:
    led_end(blk);
    break;
  }
}

static unsigned number_len(unsigned u)
{
  return floor(log10(u)) + 1U;
}

static char * unsigned_to_str(unsigned u)
{
  char * str = 0;
  size_t str_len = sizeof(*str) * number_len(u) + 1U;
  str = malloc(str_len);
  if (!str)
    return 0;
  snprintf(str, str_len, "%u", u);
  return str;
}

static void start_led(unsigned nin, int sock, const char * sock_name)
{
  /* fork off process that will NOT run as rt */
  pid_t pid = fork();
  if (!pid) {
    /* child */
    struct sched_param param;
    param.sched_priority = 0;
    if (0 > sched_setscheduler(0, SCHED_OTHER, &param)) {
      unlink(sock_name);
      perror("sched_setscheduler");
      exit(EXIT_FAILURE);
    }
    /* start led with sock_name and packet num as args */
    char * nin_str = unsigned_to_str(nin);
    if (!nin_str) {
      unlink(sock_name);
      fprintf(stderr, "mem error in start_led\n");
      exit(EXIT_FAILURE);
    }
    char * const cargv[LED_COMMAND_ARGV_NUM + 1] = {
							LED_SCRIPT,
							(char *)sock_name,
							nin_str,
							0,
    };
    int execv_ret = execv(LED_SCRIPT, cargv);
    if (0 > execv_ret) {
      unlink(sock_name);
      perror("execv " LED_SCRIPT);
      exit(EXIT_FAILURE);
    }
    /* if execv successfull will continue running led */
  } else if (pid)
    /* parent */
    return;
  else {
    close(sock);
    unlink(sock_name);
    perror("fork");
    exit(EXIT_FAILURE);
  }
}

static void remLed(void)
{
  remove("led_sock0");
}

static int led_init(python_block * blk)
{
  int * intPar    = blk->intPar;
  /* allow us to have more than one led */
  static unsigned num_instances = 0;
  /* remove old led if exists */
  /* remLed(); */
  /* get _led struct and append to blk */
  struct _led * ld = malloc(sizeof(*ld));
  if (!ld) {
    fprintf(stderr, "Memory error in led_init\n");
    exit(EXIT_FAILURE);
  }
  ld->buff_len = blk->nin * DOUBLE_SIZE;
  ld->buff = malloc(ld->buff_len);
  if (!ld->buff) {
    free(ld);
    fprintf(stderr, "Memory error in led_init\n");
    exit(EXIT_FAILURE);
  }
  ld->buff_pos = 0;
  snprintf(ld->sock_name,
	   SOCK_NAME_MAX_LEN, "/tmp/%s%u", SOCKET_NAME, num_instances);
  remove(ld->sock_name);
  blk->ptrPar = (void *)ld;
  /* count instances up */
  num_instances++;

  struct sockaddr_un sockaddr;
  socklen_t addrlen = sizeof(sockaddr);
  memset(&sockaddr, 0, addrlen);
  sockaddr.sun_family = AF_UNIX;
  strncpy(sockaddr.sun_path, ld->sock_name, SOCK_NAME_MAX_LEN);

  /* get unix socket */
  int sock = socket(sockaddr.sun_family, SOCK_STREAM, 0);
  if (0 > sock) {
    free(ld->buff);
    free(ld);
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* try to bind */
  /* make sure socket file does not already exist */
  unlink(SOCKET_NAME);
  if (0 > bind(sock, (const struct sockaddr *)&sockaddr, addrlen)) {
    close(sock);
    unlink(ld->sock_name);
    free(ld->buff);
    free(ld);
    perror("bind");
    exit(EXIT_FAILURE);
  }

  /* listen on socket */
  if (0 > listen(sock, BACKLOG)) {
    close(sock);
    unlink(ld->sock_name);
    free(ld->buff);
    free(ld);
    perror("listen");
    exit(EXIT_FAILURE);
  }

  /* try to start led process */
  start_led(blk->nin, sock, ld->sock_name);

  /* accept (blocking call) led */
  int conn = accept(sock, 0, 0);
  if (0 > conn) {
    close(sock);
    unlink(ld->sock_name);
    free(ld->buff);
    free(ld);
    perror("accept");
    exit(EXIT_FAILURE);
  }
  close(sock);
  ld->sock = conn;
  intPar[1] = 0;	
}

static void led_out(python_block * blk)
{
 int * intPar    = blk->intPar;
  
  struct _led * ld = (struct _led *)blk->ptrPar;
  int sock = ld->sock;
  char * buff = ld->buff;
  size_t send_ret = 0;
  int send_ret2 = 0;
  size_t buff_len = ld->buff_len;
  unsigned nin = blk->nin;
  double *uin;
  double values[nin];

  if((intPar[1] % intPar[0]) == 0){
    for (unsigned i = 0; nin > i; i++){
      uin = blk->u[i];
      values[i] = uin[0];
    }
    send_ret = send(sock, values, sizeof(values), 0);
    if (0 > send_ret) {
      /* assuming this only happens when */
      /* led quits before we do */
      perror("send");
    }
  }
  intPar[1] = intPar[1]+1;
}

static void led_end(python_block * blk)
{
  struct _led * ld = (struct _led *)blk->ptrPar;
  int sock = ld->sock;
  close(sock);
  unlink(ld->sock_name);
  free(ld->buff);
  free(ld);
}
