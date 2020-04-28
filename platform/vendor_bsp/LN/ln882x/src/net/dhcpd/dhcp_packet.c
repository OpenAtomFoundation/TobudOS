#include "proj_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dhcpd/dhcp_packet.h"

#include "utils/debug/log.h"

static char DHCP_MAGIC_COOKIE[4] = {0x63, 0x82, 0x53, 0x63};

//Caller need to free the memory used for the DHCP packet 
struct dhcp_packet *dhcpd_marshall(char *buffer, int length, char *request_base)
{
    struct dhcp_packet *packet = NULL;
    char *packet_begin, magic[4], code;
    int options_offset;
    struct dhcp_option *prev = NULL, *option = NULL;
    uint8_t len, len_buff;
    int offset = 0;

    //first check if the arguments is valid
    if(NULL == buffer)
    {
        LOG(LOG_LVL_ERROR, "BUFFER for marshall is NULL\r\n");
        goto err;
    }

    if(length < BOOTP_ABSOLUTE_MIN_LEN)
    {
        LOG(LOG_LVL_ERROR, "The length of dhcp packet is less than min size\r\n");
        goto err;
    }

    if(length > DHCP_MAX_MTU)
    {
        LOG(LOG_LVL_ERROR, "The length of dhcp packet is more than max MTU\r\n");
        goto err;
    }

    packet = (struct dhcp_packet*)&(request_base[offset]);
    if(NULL == packet)
    {
        LOG(LOG_LVL_EMERG, "Allocate memory failed!\r\n");
        goto err;
    }
    offset += sizeof(struct dhcp_packet);
    memset(packet, 0, sizeof(struct dhcp_packet));

    packet_begin = buffer; 
    //parse static part of packet
    memcpy(&(packet->op), packet_begin, 1);
    memcpy(&(packet->htype), packet_begin + 1, 1);
    memcpy(&(packet->hlen), packet_begin + 2, 1);
    memcpy(&(packet->hops), packet_begin + 3, 1);
    memcpy(packet->xid, packet_begin + 4, 4);
    memcpy(packet->secs, packet_begin + 8, 2);
    memcpy(packet->flags, packet_begin + 10, 2);
    memcpy(packet->ciaddr, packet_begin + 12, 4);
    memcpy(packet->yiaddr, packet_begin + 16, 4);
    memcpy(packet->siaddr, packet_begin + 20, 4);
    memcpy(packet->giaddr, packet_begin + 24, 4);
    memcpy(packet->chaddr, packet_begin + 28, 16);
    memcpy(packet->sname, packet_begin + 44, 64);
    memcpy(packet->file, packet_begin + 108, 128);

    LOG(LOG_LVL_DEBUG, "--------------DUMP DHCP PACKET(receive from STA)-------------\r\n");
    LOG(LOG_LVL_DEBUG, "packet->op=%d\r\n", packet->op);
    LOG(LOG_LVL_DEBUG, "packet->htype=%d\r\n", packet->htype);
    LOG(LOG_LVL_DEBUG, "packet->hlen=%d\r\n", packet->hlen);
    LOG(LOG_LVL_DEBUG, "packet->hops=%d\r\n", packet->hops);
    LOG(LOG_LVL_DEBUG, "packet->xid=%x,%x,%x,%x\r\n", packet->xid[0], packet->xid[1], packet->xid[2], packet->xid[3]);
    LOG(LOG_LVL_DEBUG, "packet->secs=%x,%x\r\n", packet->secs[0], packet->secs[1]);
    LOG(LOG_LVL_DEBUG, "packet->flags=%x,%x\r\n", packet->flags[0], packet->flags[1]);
    LOG(LOG_LVL_DEBUG, "packet->ciaddr=%x,%x,%x,%x\r\n", packet->ciaddr[0], packet->ciaddr[1], packet->ciaddr[2], packet->ciaddr[3]);
    LOG(LOG_LVL_DEBUG, "packet->yiaddr=%x,%x,%x,%x\r\n", packet->yiaddr[0], packet->yiaddr[1], packet->yiaddr[2], packet->yiaddr[3]);
    LOG(LOG_LVL_DEBUG, "packet->siaddr=%x,%x,%x,%x\r\n", packet->siaddr[0], packet->siaddr[1], packet->siaddr[2], packet->siaddr[3]);
    LOG(LOG_LVL_DEBUG, "packet->giaddr=%x,%x,%x,%x\r\n", packet->giaddr[0], packet->giaddr[1], packet->giaddr[2], packet->giaddr[3]);
    LOG(LOG_LVL_DEBUG, "packet->chaddr=%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\r\n", packet->chaddr[0], packet->chaddr[1], packet->chaddr[2], 
            packet->chaddr[3], packet->chaddr[4], packet->chaddr[5], packet->chaddr[6], packet->chaddr[7], packet->chaddr[8], packet->chaddr[9], 
            packet->chaddr[10], packet->chaddr[11], packet->chaddr[12], packet->chaddr[13], packet->chaddr[14], packet->chaddr[15]);
    LOG(LOG_LVL_DEBUG, "packet->sname=%s\r\n", packet->sname);
    LOG(LOG_LVL_DEBUG, "packet->file=%s\r\n", packet->file);
    LOG(LOG_LVL_DEBUG, "---------------------------------------------\r\n");

    //check DHCP magic cookie	
    memcpy(magic, packet_begin + 236, 4);
    if(0 != memcmp(DHCP_MAGIC_COOKIE, magic, 4))
    {
        LOG(LOG_LVL_ERROR, "DHCP packet magic cookie is not matched!\r\n");
        goto err;
    }

    //parse options
    options_offset = 240; //236 + 4
    packet->options = NULL;
    while(1)
    { 
        if(options_offset > length - 1)
        {
            break;
        }

        //code		
        memcpy(&code, packet_begin + options_offset, 1);
        options_offset++;
        LOG(LOG_LVL_DEBUG, "dhcp option code=%d\r\n", code);

        if((char)DHO_PAD == code)
        {
            continue;
        }

        if((char)DHO_END == code)
        {
            LOG(LOG_LVL_DEBUG, "dhcp option end\r\n");
            break;
        }

        //length		
        memcpy(&len_buff, packet_begin + options_offset, 1);
        len = len_buff;
        options_offset++;		
        LOG(LOG_LVL_DEBUG, "dhcp option length=%d\r\n", len);

        if(options_offset + len > length - 1)
        {
            LOG(LOG_LVL_WARN, "The options length is more than packet length, but no end mark.\r\n");
            break;
        }

        //value
        option = (struct dhcp_option*)&(request_base[offset]);
        if(NULL == option)
        {
            LOG(LOG_LVL_EMERG, "Allocate memory failed!\r\n");
            goto err;
        }
        offset += sizeof(struct dhcp_option);
        memset(option, 0, sizeof(struct dhcp_option));

        option->code = code;
        option->length = len_buff;
        option->value = (char*)&(request_base[offset]);
        if(NULL == option->value)
        {
            LOG(LOG_LVL_EMERG, "Allocate memory failed!\r\n");
            goto err;
        }
        offset += len;
        memcpy(option->value, buffer + options_offset, len);
        //hexdump(LOG_LVL_INFO, "option->value", option->value, len);
        option->next = NULL;	
        options_offset += len;

        //Add the option into the packet
        if(NULL == packet->options)
        {
            packet->options = option;
        }	
        if(NULL != prev)
        {
            prev->next = option;
        }

        prev = option;
    }

    if(options_offset < length - 1)
    {
        packet->padding = (char*)&(request_base[offset]);
        if(NULL == packet->padding)
        {
            LOG(LOG_LVL_EMERG, "Allocate memory failed!\r\n");
        }
        else
        {
            offset += length - options_offset;
            memcpy(packet->padding, buffer + options_offset, length - options_offset - 1);
        }
    }
    else
    {
        packet->padding = NULL;
    }

    return packet;

err:
    if(NULL != packet)
    {
        dhcpd_packet_free(packet);
        packet = NULL;
    }
    return NULL;
}

//Use this function to free dhcp packet
void dhcpd_packet_free(struct dhcp_packet *packet)
{
    struct dhcp_option *option = NULL, *current = NULL;

    LOG(LOG_LVL_TRACE, "[%s, %d]packet=0x%p\r\n", __func__, __LINE__, packet);
    if(NULL == packet)
    {
        LOG(LOG_LVL_INFO, "packet pointer is NULL, free_packet==>\r\n");
        return;
    }

    if(NULL != packet->padding)
    {
        LOG(LOG_LVL_TRACE, "packet->padding not NULL, free packet->padding.\r\n");
        //free(packet->padding);
        packet->padding = NULL;
    }

    option = packet->options;
    while(NULL != option)
    {
        if(NULL != option->value)
        {
            //free(option->value);
            option->value = NULL;
        }
        current = option; 
        option = current->next;

        if (current != NULL){
            //free(current);
            current = NULL;
        }
    }

    if (packet != NULL){
        //free(packet);
        packet = NULL;
    }
    return;
}

int dhcpd_serialize(struct dhcp_packet *packet, char buffer[], int length)
{
    int packet_len = BOOTP_ABSOLUTE_MIN_LEN, len = 0, padding_len = 0, options_offset = 240;
    struct dhcp_option *option = NULL;
    char dhcp_option_end = DHO_END;

    if(NULL == packet)
    {
        LOG(LOG_LVL_ERROR, "packet pointer is NULL\r\n");
        return 0;
    }

    //calculate the total size of the packet	
    //magic cookie
    packet_len += sizeof(DHCP_MAGIC_COOKIE);
    //options
    option = packet->options;
    while(NULL != option)
    {
        packet_len += 2;
        packet_len += (int)option->length;
        option = option->next;
    }
    //end option
    packet_len++;

    //calculate padding length	
    if(packet_len < BOOTP_ABSOLUTE_MIN_LEN + DHCP_VEND_SIZE)
    {
        padding_len = DHCP_VEND_SIZE + BOOTP_ABSOLUTE_MIN_LEN - packet_len;
        packet_len = DHCP_VEND_SIZE + BOOTP_ABSOLUTE_MIN_LEN;
    }

    if(packet_len > length)
    {
        LOG(LOG_LVL_ERROR, "Buffer size is less than packet length, buffer size=%d, packet length=%d\r\n", length, packet_len);
        return 0;
    }

    LOG(LOG_LVL_DEBUG, "--------------DUMP DHCP PACKET(send to STA)-------------\r\n");
    LOG(LOG_LVL_DEBUG, "packet->op=%d\r\n", packet->op);
    LOG(LOG_LVL_DEBUG, "packet->htype=%d\r\n", packet->htype);
    LOG(LOG_LVL_DEBUG, "packet->hlen=%d\r\n", packet->hlen);
    LOG(LOG_LVL_DEBUG, "packet->hops=%d\r\n", packet->hops);
    LOG(LOG_LVL_DEBUG, "packet->xid=%x,%x,%x,%x\r\n", packet->xid[0], packet->xid[1], packet->xid[2], packet->xid[3]);
    LOG(LOG_LVL_DEBUG, "packet->secs=%x,%x\r\n", packet->secs[0], packet->secs[1]);
    LOG(LOG_LVL_DEBUG, "packet->flags=%x,%x\r\n", packet->flags[0], packet->flags[1]);
    LOG(LOG_LVL_DEBUG, "packet->ciaddr=%x,%x,%x,%x\r\n", packet->ciaddr[0], packet->ciaddr[1], packet->ciaddr[2], packet->ciaddr[3]);
    LOG(LOG_LVL_DEBUG, "packet->yiaddr=%x,%x,%x,%x\r\n", packet->yiaddr[0], packet->yiaddr[1], packet->yiaddr[2], packet->yiaddr[3]);
    LOG(LOG_LVL_DEBUG, "packet->siaddr=%x,%x,%x,%x\r\n", packet->siaddr[0], packet->siaddr[1], packet->siaddr[2], packet->siaddr[3]);
    LOG(LOG_LVL_DEBUG, "packet->giaddr=%x,%x,%x,%x\r\n", packet->giaddr[0], packet->giaddr[1], packet->giaddr[2], packet->giaddr[3]);
    LOG(LOG_LVL_DEBUG, "packet->chaddr=%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\r\n", packet->chaddr[0], packet->chaddr[1], packet->chaddr[2], 
            packet->chaddr[3], packet->chaddr[4], packet->chaddr[5], packet->chaddr[6], packet->chaddr[7], packet->chaddr[8], packet->chaddr[9], 
            packet->chaddr[10], packet->chaddr[11], packet->chaddr[12], packet->chaddr[13], packet->chaddr[14], packet->chaddr[15]);
    LOG(LOG_LVL_DEBUG, "packet->sname=%s\r\n", packet->sname);
    LOG(LOG_LVL_DEBUG, "packet->file=%s\r\n", packet->file);
    LOG(LOG_LVL_DEBUG, "---------------------------------------------\r\n");

    memcpy(buffer, &(packet->op), 1);
    memcpy(buffer + 1, &(packet->htype), 1);
    memcpy(buffer + 2, &(packet->hlen), 1);
    memcpy(buffer + 3, &(packet->hops), 1);
    memcpy(buffer + 4, packet->xid, 4);
    memcpy(buffer + 8, packet->secs, 2);
    memcpy(buffer + 10, packet->flags, 2);
    memcpy(buffer + 12, packet->ciaddr, 4);
    memcpy(buffer + 16, packet->yiaddr, 4);
    memcpy(buffer + 20, packet->siaddr, 4);
    memcpy(buffer + 24, packet->giaddr, 4);
    memcpy(buffer + 28, packet->chaddr, 16);
    memcpy(buffer + 44, packet->sname, 64);
    memcpy(buffer + 108, packet->file, 128);

    memcpy(buffer + 236, DHCP_MAGIC_COOKIE, 4);

    option = packet->options;
    while(NULL != option)
    {
        LOG(LOG_LVL_DEBUG, "dhcp option code=%d, length=%d\r\n", option->code, option->length);
        memcpy(buffer + options_offset, &(option->code), 1);
        options_offset++;
        memcpy(buffer + options_offset, &(option->length), 1);
        options_offset++;

        len = (int)option->length;
        memcpy(buffer + options_offset, option->value, len);
        options_offset += len;		

        option = option->next;
    }

    memcpy(buffer + options_offset, &dhcp_option_end, 1);
    options_offset++;	

    if(padding_len > 0)
    {
        memset(buffer + options_offset, 0, padding_len);  
    }

    LOG(LOG_LVL_DEBUG, "total %d bytes writen\r\n", packet_len);
    return packet_len; 
}

