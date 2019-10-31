/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _AT_API_H_
#define _AT_API_H_

/**
 * AT module initialization. 
 * Call this function before example starts.
 * at_parser or at_tcp will be initialized if enabled.
 * @param[in] none
 * 
 * @return 0 - success, -1 - failure
 */
int IOT_ATM_Init(void);


/**
 * Hand received data to ATM layer.
 * Call this function in low-layer HAL.
 * @param[in] param pointer to input struct.
 *            See struct at_conn_input and struct at_mqtt_input
 *            in at_wrapper.h
 *
 * @return  0 - success, -1 - failure
 */
int IOT_ATM_Input(void * param);

#endif
