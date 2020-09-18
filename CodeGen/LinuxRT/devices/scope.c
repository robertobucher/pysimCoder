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
#define SOCKET_NAME "./scope_sock"
#define SOCK_NAME_MAX_LEN 108
#define BACKLOG 1

/* how many values to send for each stream to plotter */
#define PACKET_NUM 12U
#define DOUBLE_SIZE sizeof(double)

#define PYCONTROL_PATH_ENV_VAR "PYSUPSICTRL"
#define PLOTTER_COMMAND_ARGV_NUM 4
/* when compiling define PLOTTER_SCRIPT */

struct _scope {
  int sock;
  size_t buff_pos;
  size_t buff_len;
  char sock_name[SOCK_NAME_MAX_LEN];
  char * buff;
};

static int scope_init(python_block * blk);
static void scope_out(python_block * blk);
static void scope_end(python_block * blk);

void scope(int flag, python_block * blk) 
{
  switch (flag) {
  case CG_INIT:
    scope_init(blk);
    break;
  case CG_OUT:
    scope_out(blk);
    break;
  case CG_END:
    scope_end(blk);
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

static void start_plotter(unsigned nin, int sock, const char * sock_name)
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
    /* start plotter with sock_name and packet num as args */
    char * packet_num_str = unsigned_to_str(PACKET_NUM);
    if (!packet_num_str) {
      unlink(sock_name);
      fprintf(stderr, "mem error in start_plotter\n");
      exit(EXIT_FAILURE);
    }
    char * nin_str = unsigned_to_str(nin);
    if (!nin_str) {
      free(packet_num_str);
      unlink(sock_name);
      fprintf(stderr, "mem error in start_plotter\n");
      exit(EXIT_FAILURE);
    }
    char * const cargv[PLOTTER_COMMAND_ARGV_NUM + 1] = {
							PLOTTER_SCRIPT,
							(char *)sock_name,
							packet_num_str,
							nin_str,
							0,
    };
    int execv_ret = execv(PLOTTER_SCRIPT, cargv);
    if (0 > execv_ret) {
      unlink(sock_name);
      perror("execv " PLOTTER_SCRIPT);
      exit(EXIT_FAILURE);
    }
    /* if execv successfull will continue running plotter */
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

static void remScope(void)
{
  remove("scope_sock0");
}

static int scope_init(python_block * blk)
{
  int * intPar    = blk->intPar;
  /* allow us to have more than one scope */
  static unsigned num_instances = 0;
  /* remove old scope if exists */
  /* remScope(); */
  /* get _scope struct and append to blk */
  struct _scope * sc = malloc(sizeof(*sc));
  if (!sc) {
    fprintf(stderr, "Memory error in scope_init\n");
    exit(EXIT_FAILURE);
  }
  sc->buff_len = blk->nin * PACKET_NUM * DOUBLE_SIZE;
  sc->buff = malloc(sc->buff_len * sizeof(*sc->buff));
  if (!sc->buff) {
    free(sc);
    fprintf(stderr, "Memory error in scope_init\n");
    exit(EXIT_FAILURE);
  }
  sc->buff_pos = 0;
  snprintf(sc->sock_name,
	   SOCK_NAME_MAX_LEN, "%s%u", SOCKET_NAME, num_instances);
  remove(sc->sock_name);
  blk->ptrPar = (void *)sc;
  /* count instances up */
  num_instances++;

  struct sockaddr_un sockaddr;
  socklen_t addrlen = sizeof(sockaddr);
  memset(&sockaddr, 0, addrlen);
  sockaddr.sun_family = AF_UNIX;
  strncpy(sockaddr.sun_path, sc->sock_name, SOCK_NAME_MAX_LEN);

  /* get unix socket */
  int sock = socket(sockaddr.sun_family, SOCK_STREAM, 0);
  if (0 > sock) {
    free(sc->buff);
    free(sc);
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* try to bind */
  /* make sure socket file does not already exist */
  unlink(SOCKET_NAME);
  if (0 > bind(sock, (const struct sockaddr *)&sockaddr, addrlen)) {
    close(sock);
    unlink(sc->sock_name);
    free(sc->buff);
    free(sc);
    perror("bind");
    exit(EXIT_FAILURE);
  }

  /* listen on socket */
  if (0 > listen(sock, BACKLOG)) {
    close(sock);
    unlink(sc->sock_name);
    free(sc->buff);
    free(sc);
    perror("listen");
    exit(EXIT_FAILURE);
  }

  /* try to start plotter process */
  start_plotter(blk->nin, sock, sc->sock_name);

  /* accept (blocking call) plotter */
  int conn = accept(sock, 0, 0);
  if (0 > conn) {
    close(sock);
    unlink(sc->sock_name);
    free(sc->buff);
    free(sc);
    perror("accept");
    exit(EXIT_FAILURE);
  }
  close(sock);
  sc->sock = conn;
  intPar[1] = 0;	
}

static void scope_out(python_block * blk)
{
  int * intPar    = blk->intPar;
  struct _scope * sc = (struct _scope *)blk->ptrPar;
  int sock = sc->sock;
  char * buff = sc->buff;
  size_t send_ret = 0;
  int send_ret2 = 0;
  size_t buff_len = sc->buff_len;
  unsigned nin = blk->nin;
  /* write values to buffer */
	
  if((intPar[1] % intPar[0]) == 0){
    for (unsigned i = 0; nin > i; i++)
      memcpy((void *)(buff + (sc->buff_pos + i) * DOUBLE_SIZE),
	     blk->u[i], DOUBLE_SIZE);
    sc->buff_pos += nin;
    /* if we are to send this tick, well send buffer contents */
    if (PACKET_NUM * nin == sc->buff_pos) {
      while (buff_len != send_ret) {
	send_ret2 = send(sock, buff, buff_len, 0);
	if (0 > send_ret2) {
	  /* assuming this only happens when */
	  /* plotter quits before we do */
	  perror("send");
	  break;
	} else
	  send_ret += send_ret2;
      }
      send_ret = 0;
      sc->buff_pos = 0;
    }
  }
  intPar[1] = intPar[1]+1;
}

static void scope_end(python_block * blk)
{
  struct _scope * sc = (struct _scope *)blk->ptrPar;
  int sock = sc->sock;
  close(sock);
  unlink(sc->sock_name);
  free(sc->buff);
  free(sc);
}
