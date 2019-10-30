#include "socket_wrapper.h"
#include "cmsis_os.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <fcntl.h>

static osMutexId socket_send_lock, socket_recv_lock;

osMutexDef(socket_send_lock);
osMutexDef(socket_recv_lock);

static osSemaphoreId socket_recv_event;
osSemaphoreDef(socket_recv_event);

int socket_init(void)
{
    socket_send_lock = osMutexCreate(osMutex(socket_send_lock));
    socket_recv_lock = osMutexCreate(osMutex(socket_recv_lock));
    socket_recv_event = osSemaphoreCreate(osSemaphore(socket_recv_event),0);

    return (
        (socket_recv_event != NULL) && 
        (socket_recv_lock != NULL) && 
        (socket_send_lock != NULL)
    );
}

void io_signal_handle(int signal)
{
    osSemaphoreRelease(socket_recv_event);
}

int socket_connect(const char *ip, const char *port, sal_proto_t proto)
{
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(atoi(port))};
    int socket_proto = 0;
    struct sigaction sig_install;

    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (TOS_SAL_PROTO_TCP == proto)
    {
        socket_proto = IPPROTO_TCP;
    }
    else if (TOS_SAL_PROTO_UDP == proto)
    {
        socket_proto = IPPROTO_UDP;
    }
    else
    {
        return -1;
    }

    int socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    fcntl( socket_id, __F_SETOWN, getpid() );
    fcntl( socket_id, __F_SETSIG, SIGIO );
    fcntl( socket_id, F_SETFL, O_ASYNC );

    sig_install.sa_flags = 0;
	sig_install.sa_handler = io_signal_handle;
	sigfillset( &sig_install.sa_mask );

    if ( 0 != sigaction( SIGIO, &sig_install, NULL ) )
	{
		printf( "socket problem installing %d\n",SIGIO );
	}

    connect(socket_id, &addr, sizeof(addr));
    return socket_id;
}

int socket_send(int sock, const void *buf, size_t len)
{
    ssize_t send_len = 0, state;
    if (sock < 0)
        return -1;
    osMutexWait(socket_send_lock, TOS_TIME_FOREVER);
    do
    {
        state = send(sock, (buf + send_len), (len - send_len), MSG_DONTWAIT);
        if (state > 0)
        {
            send_len += state;
        }
        if (send_len != len)
        {
            osSemaphoreWait(socket_recv_event,100);
        }
    } while (len != send_len);
    osMutexRelease(socket_send_lock);
    return send_len;
}

int socket_recv(int sock, void *buf, size_t len)
{
    ssize_t recv_len = 0, state;
    if (sock < 0)
        return -1;
    osMutexWait(socket_recv_lock, TOS_TIME_FOREVER);
    do
    {
        state = recv(sock, (buf + recv_len), (len - recv_len), MSG_DONTWAIT);
        if (state > 0)
        {
            recv_len += state;
        }
        if (recv_len != len)
        {
            osSemaphoreWait(socket_recv_event,100);
        }
    } while (len != recv_len);
    osMutexRelease(socket_recv_lock);
    return recv_len;
}

int socket_close(int sock)
{
    close(sock);
}

static sal_module_t linux_sal = {
    .init    = socket_init,
    .connect = socket_connect,
    .send    = socket_send,
    .recv    = socket_recv,
    .close   = socket_close,
    // .sendto             = NULL,
    // .recv_timeout       = NULL,
    // .recvfrom           = NULL,
    // .recvfrom_timeout   = NULL,
    // .parse_domain       = NULL,
};

sal_module_t *get_socket_module(void)
{
    return (&linux_sal);
}
