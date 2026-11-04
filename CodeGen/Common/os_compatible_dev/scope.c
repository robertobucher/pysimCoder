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
typedef int scope_sock_t;
#define INVALID_SOCKET (-1)
#define socket_close(s) close(s)
#else
#error "scope: unsupported platform"
#endif

#include <pyblock.h>

#define BASE_TCP_PORT 50000
#define BACKLOG 1

#define PACKET_NUM 12U
#define DOUBLE_SIZE sizeof(double)

struct _scope {
  scope_sock_t listen_sock;
  scope_sock_t sock;
  int port;
  size_t buff_pos;
  size_t buff_len;
  char * buff;
};

static int scope_init(python_block * blk);
static void scope_out(python_block * blk);
static void scope_end(python_block * blk);

double get_Tsamp();

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

static int scope_init(python_block * blk)
{
  int * intPar = blk->intPar;
  static unsigned num_instances = 0;
  
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
  sc->port = BASE_TCP_PORT + num_instances;
  blk->ptrPar = (void *)sc;
  num_instances++;

#if defined(_WIN32)
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    free(sc->buff);
    free(sc);
    fprintf(stderr, "WSAStartup failed\n");
    exit(EXIT_FAILURE);
  }
#endif

  // Create TCP (AF_INET)
  sc->listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sc->listen_sock == INVALID_SOCKET) {
    fprintf(stderr, "socket error\n");
    free(sc->buff);
    free(sc);
    exit(EXIT_FAILURE);
  }

  // Avoid "Address already in use"
  int opt = 1;
  setsockopt(sc->listen_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  address.sin_port = htons(sc->port);

  if (bind(sc->listen_sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
    closesocket(sc->listen_sock);
    free(sc->buff);
    free(sc);
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(sc->listen_sock, BACKLOG) < 0) {
    closesocket(sc->listen_sock);
    free(sc->buff);
    free(sc);
    perror("listen");
    exit(EXIT_FAILURE);
  }
// 1. Prepariamo le stringhe per i parametri (come facevi prima)
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", sc->port); // La porta TCP al posto di sock_name

    char * packet_num_str = unsigned_to_str(PACKET_NUM);
    char * nin_str = unsigned_to_str(blk->nin); // Assicurati che nin sia definito/disponibile in questo scope
    
    char dtime[10];
    if(intPar[0]) 
        sprintf(dtime, "%9.6lf", get_Tsamp());
    else        
        sprintf(dtime, "1");

    // 2. Array degli argomenti cross-platform
    // Su Windows con _spawnvp, il primo elemento è l'interprete ("python" o "python3"), 
    // seguito dallo script e dai suoi argomenti.
    // Su Linux/Mac con fork/execvp, il primo elemento è sempre l'interprete.
#if defined(_WIN32)
    const char * const cargv[] = {
        "python",
        PLOTTER_SCRIPT,
        port_str,
        packet_num_str,
        nin_str,
        dtime,
        NULL
    };
#else
    const char * const cargv[] = {
        "python3",
        PLOTTER_SCRIPT,
        port_str,
        packet_num_str,
        nin_str,
        dtime,
        NULL
    };
#endif

#if defined(_WIN32)
    // --- Windows: _spawnvp in background (P_NOWAIT) ---
    intptr_t spawn_ret = _spawnvp(_P_NOWAIT, "python", cargv);
    if (spawn_ret == -1) {
        perror("_spawnvp python");
        closesocket(sc->listen_sock);
        free(sc->buff);
        free(sc);
        exit(EXIT_FAILURE);
    }
#else
    // --- Linux / macOS: fork + execvp ---
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(sc->listen_sock);
        free(sc->buff);
        free(sc);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Processo figlio
        execvp(cargv[0], (char * const *)cargv);
        perror("execvp");
        _exit(EXIT_FAILURE);
    }
    // Il processo padre prosegue e va a fare la accept() sulla socket TCP
#endif
      
  scope_sock_t conn = accept(sc->listen_sock, NULL, NULL);
  if (conn == INVALID_SOCKET) {
    closesocket(sc->listen_sock);
    free(sc->buff);
    free(sc);
    perror("accept");
    exit(EXIT_FAILURE);
  }
  
  // close listening socket, mantein the connection
  closesocket(sc->listen_sock);
  sc->sock = conn;

  intPar[2] = 0;    
}

static void scope_out(python_block * blk)
{
  int * intPar = blk->intPar;
  struct _scope * sc = (struct _scope *)blk->ptrPar;
  scope_sock_t sock = sc->sock;
  char * buff = sc->buff;
  size_t send_ret = 0;
  int send_ret2 = 0;
  size_t buff_len = sc->buff_len;
  unsigned nin = blk->nin;
  
  if((intPar[2] % intPar[1]) == 0){
    for (unsigned i = 0; nin > i; i++)
      memcpy((void *)(buff + (sc->buff_pos + i) * DOUBLE_SIZE),
             blk->u[i], DOUBLE_SIZE);
    sc->buff_pos += nin;
    
    if (PACKET_NUM * nin == sc->buff_pos) {
      while (buff_len != send_ret) {
        send_ret2 = send(sock, buff + send_ret, (int)(buff_len - send_ret), 0);
        if (send_ret2 <= 0) {
          perror("send");
          break;
        } else {
          send_ret += send_ret2;
        }
      }
      send_ret = 0;
      sc->buff_pos = 0;
    }
  }
  intPar[2] = intPar[2] + 1;
}

static void scope_end(python_block * blk)
{
  struct _scope * sc = (struct _scope *)blk->ptrPar;
  closesocket(sc->sock);
#if defined(_WIN32)
  WSACleanup();
#endif
  free(sc->buff);
  free(sc);
}
