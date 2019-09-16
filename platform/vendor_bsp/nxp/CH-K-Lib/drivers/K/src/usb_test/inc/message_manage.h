#ifndef __MESSAGE_MANAGE_H__
#define __MESSAGE_MANAGE_H__

#ifdef __cplusplus
extern "C" {
#endif
    

#include <stdint.h>

typedef struct
{
    uint8_t   m_Command;
    uint8_t   m_MessageType;
    uint16_t  m_MsgLen;
    void*     pMessage;
}MessageType_t;




extern void fn_queue_init(void);
extern uint8_t fn_msg_exist(void);
extern MessageType_t *fn_msg_pop(void);
extern uint8_t fn_msg_push(MessageType_t pMsg);

#ifdef __cplusplus
}
#endif


#endif
