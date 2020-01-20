#include "tos_k.h"

#include <stdint.h>
#include <stdio.h>

#include <lwip/sockets.h>

#include "iperf.h"

#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/api.h"
#include "ethernetif.h"

#define IPERF_BUFSZ         (4 * 1024)

void iperf_server(void *thread_param)
{
  struct netconn *conn, *newconn;
  err_t err;
  void* recv_data;
  MX_LWIP_Init();
  recv_data = (void *)tos_mmheap_alloc(IPERF_BUFSZ);
  if (recv_data == NULL)
  {
      printf("No memory\n");
  }

  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, IP_ADDR_ANY, LOCAL_PORT);

  printf("本地端口号是%d\n",LOCAL_PORT);
  LWIP_ERROR("tcpecho: invalid conn", (conn != NULL), return;);

  /* Tell connection to go into listening mode. */
  netconn_listen(conn);

  while (1) {

    /* Grab new connection. */
    err = netconn_accept(conn, &newconn);
    /*printf("accepted new connection %p\n", newconn);*/
    /* Process the new connection. */
    if (err == ERR_OK) {
      struct netbuf *buf;
//      void *data;
      u16_t len;

      while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
        printf("Recved\n");
        do {
             netbuf_data(buf, &recv_data, &len);
             printf("%s \n",(char *)recv_data);
             err = netconn_write(newconn, recv_data, len, NETCONN_COPY);
        } while (netbuf_next(buf) >= 0);
        netbuf_delete(buf);
      }
      printf("Got EOF, looping\n");
      /* Close connection and discard connection identifier. */
      netconn_close(newconn);
      netconn_delete(newconn);
    }
  }
}

void
iperf_server_init(void)
{
  sys_thread_new("iperf_server", iperf_server, NULL, 2048, 4);
}

