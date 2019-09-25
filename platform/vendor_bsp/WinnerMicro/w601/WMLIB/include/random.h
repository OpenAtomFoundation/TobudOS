/*
 * @file random.h
 * @brief Random number generator
 * @copyright (c) 2010-2011, Jouni Malinen <j@w1.fi>
 *
 * 
 * @note This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * @note Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * @note See README and COPYING for more details.
 */

#ifndef RANDOM_H
#define RANDOM_H

void random_deinit(void);

/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup Random_APIs Random APIs
 * @brief Random APIs
 */

/**
 * @addtogroup Random_APIs
 * @{
 */

/**
 * @brief          This function is used to add number to random pool
 *
 * @param[in]      *buf    number to add
 * @param[in]      len     number count
 *
 * @return         None
 *
 * @note           None
 */
void random_add_randomness(const void *buf, size_t len);
/**
 * @brief          This function is used to get random number with len from
                   random pool
 *
 * @param[in]      *buf used to save random number
 * @param[in]      len  length of random number
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int random_get_bytes(void *buf, size_t len);

/**
 * @}
 */

/**
 * @}
 */

#define random_mark_pool_ready() do { } while (0)
/**   random pool ready   */
#define random_pool_ready() 1

#endif /* RANDOM_H */

