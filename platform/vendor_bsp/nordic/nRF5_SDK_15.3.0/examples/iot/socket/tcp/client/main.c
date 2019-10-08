/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup iot_sdk_app_nrf_socket_tcp_client main.c
 * @{
 * @ingroup iot_sdk_app_nrf
 * @brief This file contains the source code for nRF TCP socket client.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define TCP_DATA_SIZE                 8

int main(void)
{
    struct sockaddr_in6 dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin6_family = AF_INET6;
    // Change this address to that of your server
    (void)inet_pton(AF_INET6, "2001:db8::1", &dest.sin6_addr);
    dest.sin6_port = htons(9000);

    int ret = -1;
    int s = -1;

    while (ret < 0)
    {
        s = socket(AF_INET6, SOCK_STREAM, 0);
        if (s < 0)
        {
            printf("Error creating socket.\r\n");
        }
        else
        {
            printf("Socket created.\r\n");
            printf("Application started.\r\n");

            errno = 0;
            ret = connect(s, (struct sockaddr *)&dest, sizeof(dest));
            if (ret < 0)
            {
                printf("Error connecting to server, errno %d\r\n", errno);
                (void) close(s);
                (void) sleep(2);
            }
        }
    }

    printf("Connected to server!\r\n");

    uint32_t seq_number = 0;
    while (seq_number < 100)
    {
        uint8_t tcp_data[TCP_DATA_SIZE];

        tcp_data[0] = (uint8_t )((seq_number >> 24) & 0x000000FF);
        tcp_data[1] = (uint8_t )((seq_number >> 16) & 0x000000FF);
        tcp_data[2] = (uint8_t )((seq_number >> 8)  & 0x000000FF);
        tcp_data[3] = (uint8_t )(seq_number         & 0x000000FF);

        tcp_data[4] = 'P';
        tcp_data[5] = 'i';
        tcp_data[6] = 'n';
        tcp_data[7] = 'g';

        errno = 0;
        ssize_t nbytes = send(s, tcp_data, TCP_DATA_SIZE, 0);
        if (nbytes != TCP_DATA_SIZE)
        {
            printf("Failed to send data, errno %d\r\n", errno);
        }
        else
        {
            printf("Data Tx, Sequence number 0x%08x\r\n", seq_number);
        }

        (void) sleep(1);
        uint8_t rx_data[TCP_DATA_SIZE];
        errno = 0;
        nbytes = recv(s, rx_data, TCP_DATA_SIZE, 0);
        if (nbytes != TCP_DATA_SIZE)
        {
            printf("Error receiving data (%d bytes), errno %d\r\n", (int)nbytes, errno);
        }
        else
        {
            uint32_t rx_seq_number  = ((rx_data[0] << 24) & 0xFF000000);
            rx_seq_number |= ((rx_data[1] << 16) & 0x00FF0000);
            rx_seq_number |= ((rx_data[2] << 8)  & 0x0000FF00);
            rx_seq_number |= (rx_data[3]         & 0x000000FF);

            if (rx_seq_number != seq_number)
            {
                printf("Mismatch in sequence number.\r\n");
            }
            else
            {
                seq_number++;
            }
        }
    }
    (void) close(s);
    return 0;
}
