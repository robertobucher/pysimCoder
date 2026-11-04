#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* Configure TCP */
#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
typedef SOCKET scope_sock_t;
#elif defined(__linux__) || defined(__APPLE__) || defined(__NuttX__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sched.h>
typedef int scope_sock_t;
#define INVALID_SOCKET (-1)
#define closesocket(s) close(s)
#else
#error "led: unsupported platform"
#endif

#include <pyblock.h>

#define BASE_TCP_PORT 51000
#define BACKLOG 1

/* how many values to send for each stream to led */
#define DOUBLE_SIZE sizeof(double)
#define LED_COMMAND_ARGV_NUM 4

struct _led {
  scope_sock_t listen_sock;
  scope_sock_t sock;
  int port;
  size_t buff_pos;
  size_t buff_len;
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

static void start_led(unsigned nin, int port)
{
   char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);

    char nin_str[16];
    snprintf(nin_str, sizeof(nin_str), "%u", nin);

#if defined(_WIN32)
    const char * const cargv[] = {
        "python",
        LED_SCRIPT,
        port_str,
        nin_str,
        NULL
    };

    intptr_t spawn_ret = _spawnvp(_P_NOWAIT, "python", cargv);
    if (spawn_ret == -1) {
        perror("_spawnvp python");
        exit(EXIT_FAILURE);
    }
#else
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        /* child */
        struct sched_param param;
        param.sched_priority = 0;
        if (0 > sched_setscheduler(0, SCHED_OTHER, &param)) {
            perror("sched_setscheduler");
            exit(EXIT_FAILURE);
        }

        const char * const cargv[] = {
            "python3",
            LED_SCRIPT,
            port_str,
            nin_str,
            NULL
        };

        execvp("python3", (char * const *)cargv);
        perror("execvp " LED_SCRIPT);
        _exit(EXIT_FAILURE);
    }
    /* parent */
#endif
}

static int led_init(python_block * blk)
{
  int * intPar = blk->intPar;
  static unsigned num_instances = 0;
  
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
  ld->port = BASE_TCP_PORT + num_instances;
  blk->ptrPar = (void *)ld;
  num_instances++;

#if defined(_WIN32)
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    free(ld->buff);
    free(ld);
    fprintf(stderr, "WSAStartup failed\n");
    exit(EXIT_FAILURE);
  }
#endif

  ld->listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (ld->listen_sock == INVALID_SOCKET) {
    fprintf(stderr, "socket error\n");
    free(ld->buff);
    free(ld);
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  setsockopt(ld->listen_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  address.sin_port = htons(ld->port);

  if (bind(ld->listen_sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
    closesocket(ld->listen_sock);
    free(ld->buff);
    free(ld);
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(ld->listen_sock, BACKLOG) < 0) {
    closesocket(ld->listen_sock);
    free(ld->buff);
    free(ld);
    perror("listen");
    exit(EXIT_FAILURE);
  }

  /* try to start led process passing the TCP port */
  start_led(blk->nin, ld->port);

  /* accept (blocking call) led */
  scope_sock_t conn = accept(ld->listen_sock, NULL, NULL);
  if (conn == INVALID_SOCKET) {
    closesocket(ld->listen_sock);
    free(ld->buff);
    free(ld);
    perror("accept");
    exit(EXIT_FAILURE);
  }
  
  closesocket(ld->listen_sock);
  ld->sock = conn;
  intPar[1] = 0;    
}

static void led_out(python_block * blk)
{
 int * intPar = blk->intPar;
  
 struct _led * ld = (struct _led *)blk->ptrPar;
 scope_sock_t sock = ld->sock;
 unsigned nin = blk->nin;
 double *uin;
 double values[nin];

 if((intPar[1] % intPar[0]) == 0){
    for (unsigned i = 0; nin > i; i++){
      uin = blk->u[i];
      values[i] = uin[0];
    }
    int send_ret = send(sock, (char *)values, sizeof(values), 0);
    if (send_ret < 0) {
      perror("send");
    }
 }
 intPar[1] = intPar[1] + 1;
}

static void led_end(python_block * blk)
{
  struct _led * ld = (struct _led *)blk->ptrPar;
  closesocket(ld->sock);
#if defined(_WIN32)
  WSACleanup();
#endif
  free(ld->buff);
  free(ld);
}
