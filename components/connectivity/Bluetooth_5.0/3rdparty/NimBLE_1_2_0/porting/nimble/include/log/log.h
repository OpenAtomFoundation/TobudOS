/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

static inline void
log_dummy(void *log, ...)
{
    (void)log;
}

#define LOG_DEBUG(_log, _mod, ...)      log_dummy(_log, ## __VA_ARGS__)
#define LOG_INFO(_log, _mod, ...)       log_dummy(_log, ## __VA_ARGS__)
#define LOG_WARN(_log, _mod, ...)       log_dummy(_log, ## __VA_ARGS__)
#define LOG_ERROR(_log, _mod, ...)      log_dummy(_log, ## __VA_ARGS__)
#define LOG_CRITICAL(_log, _mod, ...)   log_dummy(_log, ## __VA_ARGS__)

struct log {
};

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H__ */
