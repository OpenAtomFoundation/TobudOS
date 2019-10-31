#include "infra_types.h"
#include "soc_api.h"

#include "kal_public_defs.h"
#include "kal_public_api.h"
#include "kal_general_types.h"
#include "soc_api.h"

extern kal_uint32 g_ali_nwk_account_id;

unsigned int get_cur_time(void)
{
    unsigned int tick, cur_time;
    kal_get_time(&tick);
    cur_time = kal_ticks_to_milli_secs(tick);
    return cur_time;
}

void InitTimer(OsTimer* timer)
{
  timer->end_time = 0;
  timer->over_flow = FALSE;
}

void countdown_ms(OsTimer* timer, unsigned int timeout)
{
  unsigned int current_time = get_cur_time();
  timer->end_time = current_time + timeout;
  if(timer->end_time < current_time) {
     timer->over_flow = TRUE;
  }
}

char expired(OsTimer* timer)
{
  int left = 0;
  if (timer->over_flow) {
    left = 0xFFFFFFFF - get_cur_time() + timer->end_time;
  }
  else {
    left = timer->end_time - get_cur_time();
  }
  return (left < 0);
}

int HAL_TCP_Destroy(uintptr_t fd)
{
	HAL_Printf("tcp destroy,sock: %d\n",fd);
	if(-1 != fd)
		soc_close(fd);
    
	return 0;
}

uintptr_t HAL_TCP_Establish(const char *host, uint16_t port)
{
	int rc = -1, sock = 0;
	/*set the socket as no blocking*/
    kal_bool option = KAL_TRUE;
	kal_uint8 socket_opt = 1;
	sockaddr_struct addr;
	char * p;
	kal_uint8 idx = 0;
	kal_uint8 tmp = 0;

	p = (char *)host;
	/* parse ip addr string */
	while(*p != '\0')
	{
		tmp = 0;
		while((*p >= '0') && (*p <= '9'))
		{
			tmp = tmp*10 + *p - '0';
			p++;
		}

		if((*p != '.') && (*p != '\0'))
		{
			ERROR_TRACE("init_socket.hostname error:%s", host);
			return -1;
		}
	
		addr.addr[idx++] = tmp;
		
		if((*p == '\0') || (idx > 3))
			break;
		
		p++;
	}
	
	addr.addr_len = 4;
    addr.port = port;
    addr.sock_type = SOC_SOCK_STREAM;

	DEBUG_TRACE("[SOCKET]hostname:%s[%d.%d.%d.%d],port:%d,addr_len=%d", host, addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3], addr.port, addr.addr_len);
	
    sock = soc_create(SOC_PF_INET, SOC_SOCK_STREAM, 0, MOD_MQTT, g_ali_nwk_account_id);

	ERROR_TRACE("soc_create: %d\n",sock);
	
	if (sock >= 0)
	{
		
        rc=soc_setsockopt(sock, SOC_NBIO, &option, sizeof(option));
        ERROR_TRACE("soc_setsockopt1 return %d\n",rc);

		socket_opt = SOC_READ | SOC_WRITE | SOC_ACCEPT | SOC_CONNECT | SOC_CLOSE;
		rc = soc_setsockopt(sock, SOC_ASYNC, &socket_opt, sizeof(kal_uint8));
		if (rc < 0)
		{
			ERROR_TRACE("soc_setsockopt async return %d\n",rc);
			return -1;
		}
		ERROR_TRACE("soc_setsockopt2 return %d\n",rc);

		rc = soc_connect(sock, &addr);
		if (rc == SOC_SUCCESS) {
			ERROR_TRACE("soc_connect succcess");
		}else if (rc == SOC_WOULDBLOCK) {
			ERROR_TRACE("soc_connect block");
		}else{
			ERROR_TRACE("soc_connect failed, rc: %d",rc);
			return -1;
		}
    }
	else{
        ERROR_TRACE("cmns create sock failed %d\n",sock);
        rc = -1;
	}
    return (uintptr_t)sock;
}

int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
	soc_fd_set readfds;
	OsTimer rtimer;
	int bytes = 0;
	int rc = 0;
	soc_timeval_struct tv;
	int slectrc=0;
	kal_bool is_ready = 0;

	if(0 == timeout_ms){
		timeout_ms = 10;
	}
	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;
	InitTimer(&rtimer);
	countdown_ms(&rtimer, timeout_ms);

	do{
		SOC_FD_ZERO(&readfds);
		SOC_FD_SET(fd, &readfds);

		slectrc = soc_select(fd+1, &readfds, NULL, NULL, &tv);
		if(slectrc>=1 && SOC_FD_ISSET(fd, &readfds)){
			do{
				rc = soc_recv(fd, buf+bytes, len-bytes, 0);
				if(rc > 0){
					bytes += rc;
				}
				else if(rc < 0){
					// SOC_WOULDBLOCK should be continue?
					if(SOC_WOULDBLOCK != rc){
						rc = -1;
					}
					else if(SOC_WOULDBLOCK == rc){
						if(bytes>0 && bytes<len){
							tv.tv_sec = timeout_ms / 1000;
							tv.tv_usec = (timeout_ms % 1000) * 1000;
							countdown_ms(&rtimer, timeout_ms);
						}
					}
					break;
				}
				else {// FIN from the peer side
					rc = -1;
					break;
				}
			}while((bytes<len) && !expired(&rtimer));
		}
		else if(0 == slectrc){
			if(bytes>0 && bytes<len){
				tv.tv_sec = timeout_ms / 1000;
				tv.tv_usec = (timeout_ms % 1000) * 1000;
				countdown_ms(&rtimer, timeout_ms);
			}
		}
		else{
			//soc_select return value < 0, it means connect is broken
			bytes = 0;
			break;
		}
		//DEBUG_TRACE("linkit_read soc already read %d\n", bytes);

		tv.tv_sec = timeout_ms / 1000;
		tv.tv_usec = (timeout_ms % 1000) * 1000;
	}while(!expired(&rtimer) && !(rc==-1) && (bytes<len));
	return bytes;
}

int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms)
{
	int rc = 0;
	soc_timeval_struct tv;
	soc_fd_set writefds;

	tv.tv_sec = 0;  /* 30 Secs Timeout */
	tv.tv_usec = timeout_ms * 1000;  // Not init'ing this can cause strange errors

	SOC_FD_ZERO(&writefds);
	SOC_FD_SET(fd, &writefds);
	if(soc_select(fd+1, NULL, &writefds, NULL, &tv) >= 0){
		if(SOC_FD_ISSET(fd, &writefds)){
			rc = soc_send(fd, buf, len, 0);
		}
	}
	return rc;
}