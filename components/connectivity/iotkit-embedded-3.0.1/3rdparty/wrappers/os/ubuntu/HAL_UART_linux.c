#include <stdint.h>
#include <errno.h>
#if defined(__UBUNTU_SDK_DEMO__)
    #include <execinfo.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#include "infra_config.h"
#include "at_parser.h"
#include "at_wrapper.h"

static int at_uart_fd = -1;

static int read_and_discard_all_data(const int fd)
{
    int was_msg_already_printed = 0;
    int errno_code;

    while (1) {
        char buffer[1024];
        const ssize_t read_count = read(fd, buffer, sizeof(buffer));

        if (read_count == 0) {
            /* "EOF" or "connection closed at the other end"*/
            return 0;
        }

        if (read_count > 0) {
            if (!was_msg_already_printed) {
                printf("Some stale data was discarded.\r\n");
                was_msg_already_printed = 1;
            }

            continue;
        }

        assert(read_count == -1);  /* According to the specification. */

        errno_code = errno;

        if (errno_code == EINTR) {
            continue;
        }

        if (errno_code == EAGAIN ||
            errno_code == EWOULDBLOCK) {
            /**
             * We know that the file descriptor has been opened with
             * O_NONBLOCK or O_NDELAY, and these codes mean that there
             * is no data to read at present.
             */
            return 0;
        }

        /* Some other error has occurred. */
        return -1;
    }
}

int32_t HAL_AT_Uart_Init(uart_dev_t *uart)
{
    int fd;
    struct termios t_opt;
    speed_t baud;

    if (uart->port != AT_UART_PORT) {
        return 0;
    }

    if ((at_uart_fd = open(AT_UART_LINUX_DEV,
                           O_RDWR | O_NOCTTY | O_NDELAY)) == -1) {
        printf("open at uart failed\r\n");
        return -1;
    }

    switch (uart->config.baud_rate) {
        case 115200:
            baud = B115200;
            break;
        case 921600:
            baud = B921600;
            break;
        default:
            baud = B115200;
            break;
    }

    fd = at_uart_fd;
    /* set the serial port parameters */
    fcntl(fd, F_SETFL, 0);
    if (0 != tcgetattr(fd, &t_opt)) {
        return -1;
    }

    if (0 != cfsetispeed(&t_opt, baud)) {
        return -1;
    }

    if (0 != cfsetospeed(&t_opt, baud)) {
        return -1;
    }

    /* 8N1, flow control, etc. */
    t_opt.c_cflag |= (CLOCAL | CREAD);
    if (uart->config.parity == NO_PARITY) {
        t_opt.c_cflag &= ~PARENB;
    }
    if (uart->config.stop_bits == STOP_BITS_1) {
        t_opt.c_cflag &= ~CSTOPB;
    } else {
        t_opt.c_cflag |= CSTOPB;
    }
    t_opt.c_cflag &= ~CSIZE;
    switch (uart->config.data_width) {
        case DATA_WIDTH_5BIT:
            t_opt.c_cflag |= CS5;
            break;
        case DATA_WIDTH_6BIT:
            t_opt.c_cflag |= CS6;
            break;
        case DATA_WIDTH_7BIT:
            t_opt.c_cflag |= CS7;
            break;
        case DATA_WIDTH_8BIT:
            t_opt.c_cflag |= CS8;
            break;
        default:
            t_opt.c_cflag |= CS8;
            break;
    }
    t_opt.c_lflag &= ~(ECHO | ECHOE | ISIG | ICANON);

    /**
     * AT is going to use a binary protocol, so make sure to
     * turn off any CR/LF translation and the like.
     */
    t_opt.c_iflag &= ~(IXON | IXOFF | IXANY | INLCR | ICRNL);

    t_opt.c_oflag &= ~OPOST;
    t_opt.c_cc[VMIN] = 0;
    t_opt.c_cc[VTIME] = 5;

    if (0 != tcsetattr(fd, TCSANOW, &t_opt)) {
        return -1;
    }

    printf("open at uart succeed\r\n");

    /* clear uart buffer */
    read_and_discard_all_data(fd);

    return 0;
}

int32_t HAL_AT_Uart_Deinit(uart_dev_t *uart)
{
    if (uart->port == AT_UART_PORT) {
        close(at_uart_fd);
    }
    return 0;
}

int32_t HAL_AT_Uart_Send(uart_dev_t *uart, const void *data,
                         uint32_t size, uint32_t timeout)
{
    uint32_t ret, rmd = size;

    if (uart->port == AT_UART_PORT) {
        while (rmd > 0) {
            ret = write(at_uart_fd, data + size - rmd, rmd);
            if (ret == -1) {
                printf("write uart fd failed on error: %d.\r\n", errno);
                return -1;
            }
            rmd -= ret;
        }
    } else {
        if (write(1, data, size) < 0) {
            printf("write failed\n");
        }
    }

    return 0;
}

int32_t HAL_AT_Uart_Recv(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    int fd, n;

    if (uart->port == AT_UART_PORT) {
        fd = at_uart_fd;
    } else {
        fd = 1;
    }

    if ((n = read(fd, data, expect_size)) == -1) {
        return -1;
    }

    if (uart->port != AT_UART_PORT && *(char *)data == '\n') {
        *(char *)data = '\r';
    }
    if (recv_size) {
        *recv_size = n;
    }

    return 0;
}
