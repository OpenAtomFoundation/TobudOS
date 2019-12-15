/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _AT_CONN_MBOX_H_
#define _AT_CONN_MBOX_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CONN_NUM      3
#define AT_MBOX_TIMEOUT (~0)
#define AT_MBOX_EMPTY   (~0)

typedef struct
{
  void *hdl;
} at_mbox_t;

/**
 * @ingroup sys_mbox
 * Create a new mbox of specified size
 * @param mbox pointer to the mbox to create
 * @param size (minimum) number of messages in this mbox
 * @return 0 if successful, another err_t otherwise
 */
int at_mbox_new(at_mbox_t *mbox, int size, void *buf);
/**
 * @ingroup sys_mbox
 * Post a message to an mbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL)
 */
void at_mbox_post(at_mbox_t *mbox, void *msg);
/**
 * @ingroup sys_mbox
 * Try to post a message to an mbox - may fail if full or ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL)
 */
int at_mbox_trypost(at_mbox_t *mbox, void *msg);
/**
 * @ingroup sys_mbox
 * Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message (0 = wait forever)
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
           or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter!
 */
uint32_t at_mbox_fetch(at_mbox_t *mbox, void **msg, uint32_t timeout);
/**
 * @ingroup sys_mbox
 * Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @return 0 (milliseconds) if a message has been received
 *         or at_MBOX_EMPTY if the mailbox is empty
 */
uint32_t at_mbox_tryfetch(at_mbox_t *mbox, void **msg);

/**
 * @ingroup sys_mbox
 * Delete an mbox
 * @param mbox mbox to delete
 */
void at_mbox_free(at_mbox_t *mbox);

/**
 * @ingroup sys_mbox
 * Check if an mbox is valid/allocated: return 1 for valid, 0 for invalid
 */
int at_mbox_valid(at_mbox_t *mbox);

/**
 * @ingroup sys_mbox
 * Set an mbox invalid so that sys_mbox_valid returns 0
 */
void at_mbox_set_invalid(at_mbox_t *mbox);

/**
 * @ingroup sys_mbox
 * Set an mbox invalid so that sys_mbox_valid returns 0
 */
int at_mbox_empty(at_mbox_t *mb);

#ifdef __cplusplus
}
#endif

#endif /*_at_ARCH_H_*/


