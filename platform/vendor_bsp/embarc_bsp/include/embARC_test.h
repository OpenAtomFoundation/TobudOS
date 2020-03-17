/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
   --------------------------------------------- */
/* Copyright 2011,2012 Bas van den Berg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 * @ingroup EMBARC_TEST
 * @brief a light unit test framework based on CTest and Contiki App Unittest
 */

/**
 * @addtogroup	EMBARC_TEST
 * @{
 */
#ifndef H_EMBARC_TEST
#define H_EMBARC_TEST
#include "embARC_toolchain.h"

#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TEST_SETUP_FUNC_T)(void *);              /*!< setup function before test case */
typedef void (*TEST_TEARDOWN_FUNC_T)(void *);           /*!< teardown function after test case */

/**
 * @brief   Unit test structure
 */
struct unit_test {
	const char *ssname;             /*!< test suite name */
	const char *ttname;             /*!< test case name */
	void (*run)();                  /*!< test case function */
	int32_t skip;                   /*!< whether to skip ? */

	void *data;                     /*!< data to test function */
	TEST_SETUP_FUNC_T setup;        /*!< setup function */
	TEST_TEARDOWN_FUNC_T teardown;  /*!< teardown function */

	uint32_t magic;                 /*!< magic number to identify test case */
};

#define US_FNAME(sname, tname) unit_test_##sname##_##tname##_run        /*!< generate test case function name */
#define US_TNAME(sname, tname) unit_test_##sname##_##tname              /*!< generate structure name */
#define US_UNIT_TEST_MAGIC (0xdeadbeef)

/** all unit test data will be placed in a special data section .data.unittest */
#define US_UNIT_TEST_SECTION __attribute__ ((used, section(".embarc_unittest")))

#define US_UNIT_TEST_STRUCT(sname, tname, _skip, __data, __setup, __teardown)	 \
	static struct unit_test US_TNAME(sname, tname) US_UNIT_TEST_SECTION  = { \
		.ssname =#sname,						 \
		.ttname =#tname,						 \
		.run = US_FNAME(sname, tname),					 \
		.skip = _skip,							 \
		.data = __data,							 \
		.setup = (TEST_SETUP_FUNC_T)__setup,				 \
		.teardown = (TEST_TEARDOWN_FUNC_T)__teardown,			 \
		.magic = US_UNIT_TEST_MAGIC };

/** data wrapper of test suite */
#define UNIT_TEST_DATA(sname) struct sname##_data

/** setup function of test suite */
#define UNIT_TEST_SETUP(sname) \
	void EMBARC_WEAK sname##_setup(struct sname##_data *data)
/** teardown function of test suite */
#define UNIT_TEST_TEARDOWN(sname) \
	void EMBARC_WEAK sname##_teardown(struct sname##_data *data)

/** naming macros of setup and tear down functions */
#define SETUP_FNAME(sname) sname##_setup
#define TEARDOWN_FNAME(sname) sname##_teardown

#define US_UNIT_TEST_INTERNAL(sname, tname, _skip)		   \
	static void US_FNAME(sname, tname)();			   \
	US_UNIT_TEST_STRUCT(sname, tname, _skip, NULL, NULL, NULL) \
	static void US_FNAME(sname, tname)()

#define US_UNIT_TEST2_INTERNAL(sname, tname, _skip)									 \
	static struct sname##_data __unit_test_##sname##_data;								 \
	UNIT_TEST_SETUP(sname);												 \
	UNIT_TEST_TEARDOWN(sname);											 \
	static void US_FNAME(sname, tname)(struct sname##_data *data);							 \
	US_UNIT_TEST_STRUCT(sname, tname, _skip, &__unit_test_##sname##_data, SETUP_FNAME(sname), TEARDOWN_FNAME(sname)) \
	static void US_FNAME(sname, tname)(struct sname##_data *data)

#define UNIT_TEST(sname, tname) US_UNIT_TEST_INTERNAL(sname, tname, 0)
#define UNIT_TEST_SKIP(sname, tname) US_UNIT_TEST_INTERNAL(sname, tname, 1)

#define UNIT_TEST2(sname, tname) US_UNIT_TEST2_INTERNAL(sname, tname, 0)
#define UNIT_TEST2_SKIP(sname, tname) US_UNIT_TEST2_INTERNAL(sname, tname, 1)

/** check macros for unit test */
extern void unit_test_assert_str(const char *expect, const char *real, const char *caller, int32_t line);
#define UNIT_TEST_ASSERT_STR(expect, real) unit_test_assert_str(expect, real, __FILE__, __LINE__)

extern void unit_test_assert_data(		\
	const uint8_t *expect, int32_t expsize,	\
	const uint8_t *real, int32_t realsize,	\
	const char *caller, int32_t line);
#define UNIT_TEST_ASSERT_DATA(expect, expsize, real, realsize) \
	unit_test_assert_data(expect, expsize, real, realsize, __FILE__, __LINE__)

extern void unit_test_assert_equal(int32_t expect, int32_t real, const char *caller, int32_t line);
#define UNIT_TEST_ASSERT_EQUAL(expect, real) unit_test_assert_equal(expect, real, __FILE__, __LINE__)

extern void unit_test_assert_not_equal(int32_t expect, int32_t real, const char *caller, int32_t line);
#define UNIT_TEST_ASSERT_NOT_EQUAL(expect, real) unit_test_assert_not_equal(expect, real, __FILE__, __LINE__)

extern void unit_test_assert_null(void *real, const char *caller, int32_t line);
#define UNIT_TEST_ASSERT_NULL(real) unit_test_assert_null((void *)real, __FILE__, __LINE__)

extern void unit_test_assert_not_null(const void *real, const char *caller, int32_t line);
#define UNIT_TEST_ASSERT_NOT_NULL(real) unit_test_assert_not_null(real, __FILE__, __LINE__)

extern void unit_test_assert_true(int32_t real, const char *caller, int32_t line);
#define UNIT_TEST_ASSERT_TRUE(real) unit_test_assert_true(real, __FILE__, __LINE__)

extern void unit_test_assert_false(int32_t real, const char *caller, int32_t line);
#define UNIT_TEST_ASSERT_FALSE(real) unit_test_assert_false(real, __FILE__, __LINE__)

extern void unit_test_assert_fail(const char *caller, int32_t line);
#define UNIT_TEST_ASSERT_FAIL() unit_test_assert_fail(__FILE__, __LINE__)

extern int32_t unit_test_run(const char *test_suite);

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

#endif  /*H_EMBARC_TEST */
/** @} end of group EMBARC_TEST */
