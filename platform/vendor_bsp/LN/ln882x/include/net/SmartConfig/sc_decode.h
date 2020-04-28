
#ifndef __SC_DECODE_H__
#define __SC_DECODE_H__

#include "types.h"

typedef enum {
	SC_END     = 0,
	SC_SEARCH_CHAN,
	SC_LOCKED_CHAN,
	SC_COMPLETED,
} decode_status_enum_t;


typedef struct {
	int channel;
	uint8_t lead_complete_flag;
} sc_lead_code_t;


typedef struct {
	sc_lead_code_t   lead_code;
	decode_status_enum_t sc_ctrl;
	decode_status_enum_t Sc_Status;
} sc_private_t;

decode_status_enum_t packet_deoced(sc_private_t *sc_priv, uint8_t *data, uint16_t total_len);
uint16_t sc_read_locked_channel(sc_private_t *sc_priv);

#endif /* __SC_DECODE_H__ */
