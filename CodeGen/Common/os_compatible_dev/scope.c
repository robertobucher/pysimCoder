#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* If ran under Windows (MinGW, MSYS, CYGWIN), UDP sockets are utilized.
 * If ran under Unix, we use UNIX sockets.
 */
#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
/* no UNIX sockets, send UDP datagrams to 127.0.0.1:SCOPE_UDP_PORT+<instance> */
#define SCOPE_UDP
#define SCOPE_UDP_PORT 50000
typedef SOCKET scope_sock_t;
#elif defined(__linux__) || defined(__APPLE__) || defined(__NuttX__)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
typedef int scope_sock_t;
#else
#error "scope: unsupported platform"
#endif

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

struct _scope {
  scope_sock_t sock;
#ifdef SCOPE_UDP
  struct sockaddr_in addr;
#endif
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

static int scope_init(python_block * blk)
{
  int * intPar    = blk->intPar;
  static unsigned num_instances = 0;
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
	   SOCK_NAME_MAX_LEN, "/tmp/%s%u", SOCKET_NAME, num_instances);
  remove(sc->sock_name);
  blk->ptrPar = (void *)sc;
  /* count instances up */
  num_instances++;

#ifdef SCOPE_UDP
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    free(sc->buff);
    free(sc);
    fprintf(stderr, "WSAStartup failed\n");
    exit(EXIT_FAILURE);
  }
  sc->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sc->sock == INVALID_SOCKET) {
    fprintf(stderr, "socket: WSA error %d\n", WSAGetLastError());
    WSACleanup();
    free(sc->buff);
    free(sc);
    exit(EXIT_FAILURE);
  }
  memset(&sc->addr, 0, sizeof(sc->addr));
  sc->addr.sin_family = AF_INET;
  /* num_instances was already counted up */
  sc->addr.sin_port = htons(SCOPE_UDP_PORT + num_instances - 1);
  sc->addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
#else
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
#endif
  intPar[2] = 0;	
}

static void scope_out(python_block * blk)
{
  int * intPar    = blk->intPar;
  struct _scope * sc = (struct _scope *)blk->ptrPar;
  scope_sock_t sock = sc->sock;
  char * buff = sc->buff;
  size_t send_ret = 0;
  int send_ret2 = 0;
  size_t buff_len = sc->buff_len;
  unsigned nin = blk->nin;
  /* write values to buffer */
	
  if((intPar[2] % intPar[1]) == 0){
    for (unsigned i = 0; nin > i; i++)
      memcpy((void *)(buff + (sc->buff_pos + i) * DOUBLE_SIZE),
	     blk->u[i], DOUBLE_SIZE);
    sc->buff_pos += nin;
    /* if we are to send this tick, well send buffer contents */
    if (PACKET_NUM * nin == sc->buff_pos) {
#ifdef SCOPE_UDP
      if (sendto(sock, buff, (int)buff_len, 0,
                 (const struct sockaddr *)&sc->addr, sizeof(sc->addr)) == SOCKET_ERROR)
        fprintf(stderr, "sendto: WSA error %d\n", WSAGetLastError());
#else
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
#endif
      send_ret = 0;
      sc->buff_pos = 0;
    }
  }
  intPar[2] = intPar[2]+1;
}

static void scope_end(python_block * blk)
{
  struct _scope * sc = (struct _scope *)blk->ptrPar;
  scope_sock_t sock = sc->sock;
#ifdef SCOPE_UDP
  closesocket(sock);
  WSACleanup();
#else
  close(sock);
  unlink(sc->sock_name);
#endif
  free(sc->buff);
  free(sc);
}
