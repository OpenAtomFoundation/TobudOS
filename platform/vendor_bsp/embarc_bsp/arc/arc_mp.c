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
#include "arc/arc_mp.h"

static volatile struct {
	void (*fn)(uint32_t, void *);
	void *arg;
} arc_cpu_init[ARC_FEATURE_MP_NUM_CPUS];

/*
 * arc_cpu_wake_flag is used to sync up master core and slave cores
 * Slave core will spin for arc_cpu_wake_flag until master core sets
 * it to the core id of slave core. Then, slave core clears it to notify
 * master core that it's waken
 *
 */
volatile uint32_t arc_cpu_wake_flag;

volatile uint8_t *arc_cpu_sp;

/**
 * @brief Start slave cpu
 *  master core call this function to start slave cpu
 * @param cpu_num Slave cpu number
 * @param stack The pointer to stack area of slave cpu
 * @param sz Stack size
 * @param fn Function that slave cpu execute
 * @param arg Argument for slave cpu
 */
void arc_start_slave_cpu(uint32_t cpu_num, uint8_t *stack, uint32_t sz,
			 void (*fn)(uint32_t, void *), void *arg)
{
	arc_cpu_init[cpu_num].fn = fn;
	arc_cpu_init[cpu_num].arg = arg;

	/* set the initial sp of target sp through arc_cpu_sp
	 * arc_cpu_wake_flag will protect arc_cpu_sp that
	 * only one slave cpu can read it per time
	 */
	arc_cpu_sp = stack + sz;

	arc_cpu_wake_flag = cpu_num;

	/* wait slave cpu to start */
	while (arc_cpu_wake_flag != 0) {
		;
	}
}

/**
 * @brief The C entry of slave cpu
 *
 * @param cpu_num Slave cpu id
 */
void arc_slave_start(uint32_t cpu_num)
{
	void (*fn)(uint32_t, void *);

	fn = arc_cpu_init[cpu_num].fn;

	fn(cpu_num, arc_cpu_init[cpu_num].arg);
}
