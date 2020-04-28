//#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
#include "types.h"

#include "smartconfig/sc_decode.h"

#define IEEE80211_ADDR_LEN 6

typedef struct mac_frame{
	uint8_t		i_fc[2];
	uint8_t		i_dur[2];
	uint8_t		i_addr1[IEEE80211_ADDR_LEN];
	uint8_t		i_addr2[IEEE80211_ADDR_LEN];
	uint8_t		i_addr3[IEEE80211_ADDR_LEN];
	uint8_t		i_seq[2];
	/* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
	/* see below */
} ieee80211_frame_t;

uint8_t m_offset_len = 0;

#define LEAD_CODE_LEN          5
#define LEAD_CODE_SEQ_BUF_SIZE 8
static uint16_t lead_code_seq_buf[LEAD_CODE_SEQ_BUF_SIZE];
static uint8_t  ssid_pwd[32+1+64+1];
//TODO: If the password length is less than 8, the password is invalid.

#define CHAR_ENCODE_LEN             (10)
#define CHAR_ENCODE_SEQ_BUF_SIZE    (12)
static uint8_t char_encode_seq_buf[CHAR_ENCODE_SEQ_BUF_SIZE];


static uint8_t calcrc_1byte(uint8_t abyte)    
{
	uint8_t i,crc_1byte;     
	crc_1byte = 0;
	for(i = 0; i < 8; i++)    
	{    
		if((crc_1byte ^ abyte) & 0x01)    
		{    
			crc_1byte ^=  0x18;     
			crc_1byte >>= 1;    
			crc_1byte |=  0x80;    
		}          
		else
		{
			crc_1byte >>= 1;   
		}		
		abyte >>= 1;          
	}   
	return crc_1byte;   
}

static uint8_t calcrc_bytes(uint8_t *p,uint8_t len)  
{
	uint8_t crc = 0;  
	while(len--) //len is the total number of bytes to check
	{  
		crc = calcrc_1byte(crc ^ *p++);  
	}  
	return crc;
}

static bool crc8_check(uint8_t * p, uint8_t len)
{
    uint8_t crc8 = 0;
    crc8 = calcrc_bytes(p, (len-1));
    return (crc8 == *(p+len-1));
}


static bool get_lead_code(uint16_t total_len)
{
    uint16_t lead_code[LEAD_CODE_LEN] = {0, 0, 0, 0, 0};
    static uint32_t count = 0;
    uint8_t pos = 0;
    bool ret = false;
    
    pos = (count++) % LEAD_CODE_SEQ_BUF_SIZE;
    lead_code_seq_buf[pos] = total_len;

    (pos >= 4) ? (lead_code[0] = lead_code_seq_buf[pos-4]) : (lead_code[0] = lead_code_seq_buf[pos+LEAD_CODE_SEQ_BUF_SIZE-4]);
    (pos >= 3) ? (lead_code[1] = lead_code_seq_buf[pos-3]) : (lead_code[1] = lead_code_seq_buf[pos+LEAD_CODE_SEQ_BUF_SIZE-3]);
    (pos >= 2) ? (lead_code[2] = lead_code_seq_buf[pos-2]) : (lead_code[2] = lead_code_seq_buf[pos+LEAD_CODE_SEQ_BUF_SIZE-2]);
    (pos >= 1) ? (lead_code[3] = lead_code_seq_buf[pos-1]) : (lead_code[3] = lead_code_seq_buf[pos+LEAD_CODE_SEQ_BUF_SIZE-1]);
    (pos >= 0) ? (lead_code[4] = lead_code_seq_buf[pos-0]) : (lead_code[4] = lead_code_seq_buf[pos+LEAD_CODE_SEQ_BUF_SIZE-0]);
    
    if(((lead_code[1] - lead_code[0]) == 1) && \
       ((lead_code[2] - lead_code[1]) == 1) && \
       ((lead_code[3] - lead_code[2]) == 1) && \
       ((lead_code[4] - lead_code[3]) == 1))
    {
        m_offset_len = lead_code[0] - 0;
        memset(ssid_pwd, 0, sizeof(ssid_pwd));
        ret = true;
    }
    else
    {
        ret = false;
    }
	return ret;
}


static bool data_decode(uint8_t len)
{
    uint8_t char_encode[CHAR_ENCODE_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static uint32_t count = 0;
    uint8_t pos = 0;
    uint8_t char_pos = 0, val = 0, crc8 = 0;
    bool ret = false;
    
    pos = (count++) % CHAR_ENCODE_SEQ_BUF_SIZE;
    char_encode_seq_buf[pos] = len;
    
    (pos >= 9) ? (char_encode[0] = char_encode_seq_buf[pos-9]) : (char_encode[0] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-9]);
    if(char_encode[0] != 0){
        return false;
    }
    (pos >= 8) ? (char_encode[1] = char_encode_seq_buf[pos-8]) : (char_encode[1] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-8]);
    if(char_encode[1] != 0){
        return false;
    }
    
    (pos >= 7) ? (char_encode[2] = char_encode_seq_buf[pos-7]) : (char_encode[2] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-7]);
    (pos >= 6) ? (char_encode[3] = char_encode_seq_buf[pos-6]) : (char_encode[3] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-6]);
    if(17 != (char_encode[2] + char_encode[3])){
        return false;
    }

    (pos >= 5) ? (char_encode[4] = char_encode_seq_buf[pos-5]) : (char_encode[4] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-5]);
    (pos >= 4) ? (char_encode[5] = char_encode_seq_buf[pos-4]) : (char_encode[5] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-4]);
    if(17 != (char_encode[4] + char_encode[5])){
        return false;
    }
    else
    {
        char_pos =  ((char_encode[2]-1) & 0xf);
        char_pos |= ((char_encode[4]-1) & 0xf) << 4;
    }
    
    (pos >= 3) ? (char_encode[6] = char_encode_seq_buf[pos-3]) : (char_encode[6] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-3]);
    (pos >= 2) ? (char_encode[7] = char_encode_seq_buf[pos-2]) : (char_encode[7] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-2]);
    if(17 != (char_encode[6] + char_encode[7])){
        return false;
    }

    (pos >= 1) ? (char_encode[8] = char_encode_seq_buf[pos-1]) : (char_encode[8] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-1]);
    (pos >= 0) ? (char_encode[9] = char_encode_seq_buf[pos-0]) : (char_encode[9] = char_encode_seq_buf[pos+CHAR_ENCODE_SEQ_BUF_SIZE-0]);
    if(17 != (char_encode[8] + char_encode[9])){
        return false;
    }
    else
    {
        val =  ((char_encode[6]-1) & 0xf);
        val |= ((char_encode[8]-1) & 0xf) << 4;
    }

    ssid_pwd[char_pos] = val;
    static uint8_t ssid_pwd_len = 0;
    if((char_pos+1) >= ssid_pwd_len){
        ssid_pwd_len = char_pos + 1;//update mooncake len
    }
    
    if(ssid_pwd_len > 6){
        if(crc8_check(ssid_pwd,ssid_pwd_len)){
            return true;
        }
    }
    return false;
}

decode_status_enum_t packet_deoced(sc_private_t *sc_priv, uint8_t *data, uint16_t total_len)
{
	ieee80211_frame_t *pframe = (ieee80211_frame_t *)data;

    if(((pframe->i_fc[0] == 0x88) || (pframe->i_fc[0] == 0x08)) && \
      (((pframe->i_fc[1]&0x03) == 0x01) || ((pframe->i_fc[1]&0x03) == 0x02)))
    {
        if (sc_priv->lead_code.lead_complete_flag == 0) 
        {
            sc_priv->Sc_Status = SC_SEARCH_CHAN;
            if(true == get_lead_code(total_len))
            {
                sc_priv->lead_code.lead_complete_flag = 1;
                sc_priv->Sc_Status = SC_LOCKED_CHAN;
            }
            return sc_priv->Sc_Status;
        }
        
        if (sc_priv->Sc_Status == SC_LOCKED_CHAN) 
        {
            if(data_decode(total_len - m_offset_len))
            {
                sc_priv->Sc_Status = SC_COMPLETED;
            }
        }
    }
	return sc_priv->Sc_Status;
}

uint16_t sc_read_locked_channel(sc_private_t *sc_priv)
{
	return sc_priv->lead_code.channel;
}

