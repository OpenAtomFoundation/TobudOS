/*
 * Copyright (c) 2017-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __UART_STDOUT_H__
#define __UART_STDOUT_H__

#include <stdint.h>


/**
 * \brief Initializes the STDIO.
 *
 */
void stdio_init(void);

/**
 * \brief Uninitializes the STDIO.
 */
void stdio_uninit(void);

//int _write(int fd, char *str, int len);     //WangJin 2021.03.04
#endif /* __UART_STDOUT_H__ */
