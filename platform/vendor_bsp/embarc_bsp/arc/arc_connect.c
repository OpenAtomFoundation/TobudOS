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

/**
 * @file
 * @ingroup ARC_HAL_MISC_CONNECT
 * @brief ar connect driver
 *
 * This module contains functions for arc connect module.
 */

#include "arc/arc_connect.h"
#include "arc/arc_mp.h"

static ARC_SPINLOCK_T arc_connect_lock;

/**
 * @brief Execute arc connect command
 *
 * @param op Pointer to arc connect operation
 * @return Result of arc connect command
 */
static uint32_t arc_connect_cmd_execute(ARC_CONNECT_OP_T *op)
{
	uint32_t regval = 0;

	if (!op) {
		return 0;
	}

	regval = op->cmd | ((op->param) << 8);

	arc_spinlock_get(&arc_connect_lock);

	switch (op->type) {
	case ARC_CONNECT_CMD_TYPE_CMD_ONLY:
		arc_aux_write(AUX_CONNECT_CMD, regval);
		break;
	case ARC_CONNECT_CMD_TYPE_CMD_RETURN:
		arc_aux_write(AUX_CONNECT_CMD, regval);
		break;
	case ARC_CONNECT_CMD_TYPE_CMD_WDATA:
		arc_aux_write(AUX_CONNECT_WDATA, op->wdata);
		arc_aux_write(AUX_CONNECT_CMD, regval);
		break;
	case ARC_CONNECT_CMD_TYPE_CMD_WDATA_RETURN:
		arc_aux_write(AUX_CONNECT_WDATA, op->wdata);
		arc_aux_write(AUX_CONNECT_CMD, regval);
		break;
	default:
		break;
	}

	regval = arc_aux_read(AUX_CONNECT_READBACK);

	arc_spinlock_release(&arc_connect_lock);

	return regval;
}

/**
 * @brief Get the core id in arc connect
 *
 * @return Core id
 */
uint32_t arc_connect_check_core_id(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_CHECK_CORE_ID, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief Enable interrupt distribute unit
 *
 */
void arc_connect_idu_enable(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_IDU_ENABLE, 0);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief Disable interrupt distribute unit
 *
 */
void arc_connect_idu_disable(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_IDU_DISABLE, 0);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief Read interrupt distribute enable status
 *
 * @return Enable status
 */
uint32_t arc_connect_idu_read_enable(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_READ_ENABLE, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief Set the mode of irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 * @param trigger_mode Trigger mode, level or pulse
 * @param distri_mode Distribute mode
 */
void arc_connect_idu_set_mode(uint32_t irq_num, uint16_t trigger_mode, uint16_t distri_mode)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_IDU_SET_MODE,	\
				     irq_num, (distri_mode | (trigger_mode << 4)));

	arc_connect_cmd_execute(&op);
}

/**
 * @brief Read the mode of irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 *
 */
uint32_t arc_connect_idu_read_mode(uint32_t irq_num)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_READ_ENABLE, irq_num);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief Set the target core to handle the irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 * @param target_core Target core
 */
void arc_connect_idu_set_dest(uint32_t irq_num, uint32_t target_core)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_IDU_SET_DEST, irq_num, target_core);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief Read the target core to handle the irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 * @return Target core
 */
uint32_t arc_connect_idu_read_dest(uint32_t irq_num)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_READ_DEST, irq_num);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief Generate the irq connected to idu in software
 *
 * @param irq_num Number of irq connected to idu
 */
void arc_connect_idu_gen_cirq(uint32_t irq_num)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_IDU_GEN_CIRQ, irq_num);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief Acknowledge irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 */
void arc_connect_idu_ack_cirq(uint32_t irq_num)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_IDU_ACK_CIRQ, irq_num);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief Check the status of irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 * @return Status of irq connected to idu
 */
uint32_t arc_connect_idu_check_status(uint32_t irq_num)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_CHECK_STATUS, irq_num);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief Check the source of irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 * @return Source of irq connected to idu
 */
uint32_t arc_connect_idu_check_source(uint32_t irq_num)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_CHECK_SOURCE, irq_num);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief Set the interrupt mask of irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 * @param mask Interrupt mask
 */
void arc_connect_idu_set_mask(uint32_t irq_num, uint32_t mask)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_IDU_SET_MASK, irq_num, mask);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief Read the interrupt mask of irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 * @return Interrupt mask
 */
uint32_t arc_connect_idu_read_mask(uint32_t irq_num)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_READ_MASK, irq_num);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief Check the first core to handle the irq connected to idu
 *
 * @param irq_num Number of irq connected to idu
 * @return Core number
 */
uint32_t arc_connect_idu_check_first(uint32_t irq_num)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_CHECK_FIRST, irq_num);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief Configure the irq connected to idu
 *
 * @param core Target core to handle the irq
 * @param irq_num Number of irq connected to idu
 * @param trigger_mode Trigger mode
 * @param distri_mode Distribute mode
 */
void arc_connect_idu_config_irq(uint32_t core, uint32_t irq_num, uint16_t trigger_mode, uint16_t distri_mode)
{
	arc_connect_idu_disable();
	arc_connect_idu_set_mode(irq_num, trigger_mode, distri_mode);
	arc_connect_idu_set_dest(irq_num, core);
	arc_connect_idu_set_mask(irq_num, 0x0);
	arc_connect_idu_enable();
}

/**
 * @brief generate inter-core interrupt
 * the calling core will raise an interrupt to the target core
 * @param core_id the target core
 */
void arc_connect_ici_generate(uint32_t core_id)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_INTRPT_GENERATE_IRQ, core_id);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief acknowledge the raised inter-core interrupt
 *
 * @param core_id the core rasing the inter-core interrupt
 */
void arc_connect_ici_ack(uint32_t core_id)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_INTRPT_GENERATE_ACK, core_id);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read the status of inter-core interrupt
 *
 * @param core_id the target core
 * @return 1 the inter-core interrupt is pending, 0 the inter-core interrupt
 *         is acknowledged
 */
uint32_t arc_connect_ici_read_status(uint32_t core_id)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_INTRPT_READ_STATUS, core_id);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief check the source of inter-core interrupt
 *
 * @return the source of inter-core interrupt
 */
uint32_t arc_connect_ici_check_src(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_INTRPT_CHECK_SOURCE, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief taka a inter-core semaphore
 *
 * @param sem_id semaphore id
 * @return 0 failed, 1 success
 */
uint32_t arc_connect_ics_take(uint32_t sem_id)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_SEMA_CLAIM_AND_READ, sem_id);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief release a inter-core semaphore
 *
 * @param sem_id semaphore id
 */
void arc_connect_ics_release(uint32_t sem_id)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_SEMA_RELEASE, sem_id);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief compulsively release and make available a semaphore
 *
 * @param sem_id semaphore id
 */
void arc_connect_ics_force_release(uint32_t sem_id)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_SEMA_FORCE_RELEASE, sem_id);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief set the address of message-passing sram for subsequent read or
          write sram operations
 *
 * @param addr sram address
 */
void arc_connect_icm_addr_set(uint32_t addr)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_SET_ADDR, addr);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read the value of internal MSG_ADDR reg
 *
 * @return value of MSG_ADDR reg
 */
uint32_t arc_connect_icm_addr_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_READ_ADDR, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief set the offset address of message-passing sram for subsequent read or
          write sram operations
 *
 * @param offset address offset
 */
void arc_connect_icm_addr_offset_set(uint32_t offset)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_SET_ADDR_OFFSET, offset);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read the value of address offset reg
 *
 * @return value of address offset reg
 */
uint32_t arc_connect_icm_addr_offset_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_READ_ADDR_OFFSET, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief write data to the message-passing sram
 *
 * @param data data to write
 */
void arc_connect_icm_msg_write(uint32_t data)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_WRITE, 0, data);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief incremental write to the message-passing sram
 *
 * @param data data to write
 */
void arc_connect_icm_msg_inc_write(uint32_t data)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_WRITE_INC, 0, data);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief write data into the message passing sram with the address
 *  specified by an immediate value.
 *
 * @param addr the specified address
 * @param data data to write
 */
void arc_connect_icm_msg_imm_write(uint32_t addr, uint32_t data)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_WRITE_IMM, addr, data);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read data from the message passing sram
 *
 * @return read data
 */
uint32_t arc_connect_icm_msg_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_READ, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief incremental read data from the message passing sram
 *
 * @return read data
 */
uint32_t arc_connect_icm_msg_inc_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_READ_INC, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief read data at the specified address from the message passing sram
 *
 * @param addr the specified address
 * @return read data
 */
uint32_t arc_connect_icm_msg_imm_read(uint32_t addr)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_READ_IMM, addr);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief enable or disable ECC or parity protection on the message data
 *
 * @param val 0 = enable protection, 1= disable protection
 */
void arc_connect_icm_ecc_ctrl_set(uint32_t val)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_SET_ECC_CTRL, 0, val);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read the status ofECC or parity protection on the message data
 *
 * @return 0 = enable protection, 1= disable protection
 */
uint32_t arc_connect_icm_ecc_ctrl_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_MSG_SRAM_READ_ECC_CTRL, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief generate reset request to cores in the system.
 *
 * @param cores the target cores
 */
void arc_connect_debug_reset(uint32_t cores)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_RESET, 0, cores);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief generate halt request to cores in the system.
 *
 * @param cores the target cores
 */
void arc_connect_debug_halt(uint32_t cores)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_HALT, 0, cores);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief generate run request to cores in the system.
 *
 * @param cores the target cores
 */
void arc_connect_debug_run(uint32_t cores)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_RUN, 0, cores);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief set the internal MASK reg in ICD
 * the MASK register determines whether a global halt is triggered
 * if a core is halted in response to one of the following events:
 * core halt, actionpoint halt, self-halt, and breakpoint halt
 * @param cores the cores for which the MASK register should be updated
 * @param mask mask bits
 */
void arc_connect_debug_mask_set(uint32_t cores, uint32_t mask)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_SET_MASK, mask, cores);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read the internal MASK reg in ICD
 *
 * @param cores the cores for which the MASK register should be read
 * @return mask bits
 */
uint32_t arc_connect_debug_mask_read(uint32_t cores)
{

	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_RUN, 0, cores);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief select cores that should be halted if the core issuing the command is halted
 *
 * @param cores cores to select
 */
void arc_connect_debug_select_set(uint32_t cores)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_SET_SELECT, 0, cores);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief ead the internal SELECT register in ICD
 *
 * @return SELECT register value
 */
uint32_t arc_connect_debug_select_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_READ_SELECT, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief read the status, halt or run,of all cores
 *
 * @return bits: 1 running, 0 halted
 */
uint32_t arc_connect_debug_en_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_READ_EN, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief check the last command sent to ICD.
 *
 * @return ICD command
 */
uint32_t arc_connect_debug_cmd_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_READ_CMD, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief read the value of internal MCD_CORE register in ICD
 *
 * @return MCD_CORE register
 */
uint32_t arc_connect_debug_core_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_DEBUG_READ_CORE, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief clear the global free running counter
 *
 */
void arc_connect_gfrc_clear(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_GFRC_CLEAR, 0);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read low 32-bit of gfrc
 *
 * @return low 32-bit of gfrc
 */
uint32_t arc_connect_gfrc_lo_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_GFRC_READ_LO, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief read high 32-bit of gfrc
 *
 * @return high 32-bit of gfrc
 */
uint32_t arc_connect_gfrc_hi_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_GFRC_READ_HI, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief enable gfrc
 *
 */
void arc_connect_gfrc_enable(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_GFRC_ENABLE, 0);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief disable gfrc
 *
 */
void arc_connect_gfrc_disable(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_GFRC_DISABLE, 0);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief set the relevant cores to halt the GFRC
 *
 * @param cores the relevant cores
 */
void arc_connect_gfrc_core_set(uint32_t cores)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_GFRC_SET_CORE, 0, cores);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read the status of gfrc
 *
 * @return gfrc status
 */
uint32_t arc_connect_gfrc_halt_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_GFRC_READ_HALT, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief read the internal CORE register
 *
 * @return CORE register value
 */
uint32_t arc_connect_gfrc_core_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_GFRC_READ_CORE, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief set the power mode for specific ARConnect group
 *
 * @param group  the ARConnect group
 * @param cmd power mode
 */
void arc_connect_pdm_pm_set(uint32_t group, uint32_t cmd)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_PDM_SET_PM, group, cmd);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read the internal PM register of the specified ARConnect group.
 *
 * @param group the ARConnect group
 * @return status value
 */
uint32_t arc_connect_pdm_pdstatus_read(uint32_t group)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_PDM_READ_PSTATUS, group);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief set PUCNT
 *
 * @param cnt cnt to set
 */
void arc_connect_pmu_pucnt_set(uint32_t cnt)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_PMU_SET_PUCNT, 0, cnt);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read the PUNCNT
 *
 * @return PUNCNT's value
 */
uint32_t arc_connect_pmu_pucnt_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_PMU_READ_PUCNT, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief set RSTCNT
 *
 * @param cnt cnt to set
 */
void arc_connect_pmu_rstcnt_set(uint32_t cnt)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_PMU_SET_RSTCNT, 0, cnt);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read RSTCNT
 *
 * @return RSTCNT's value
 */
uint32_t arc_connect_pmu_rstcnt_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_PMU_READ_RSTCNT, 0);

	return arc_connect_cmd_execute(&op);
}

/**
 * @brief set PDCCNT
 *
 * @param cnt cnt to set
 */
void arc_connect_pmu_pdccnt_set(uint32_t cnt)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_PMU_SET_PDCNT, 0, cnt);

	arc_connect_cmd_execute(&op);
}

/**
 * @brief read PDCCNT
 *
 * @return PDCCNT's vaule
 */
uint32_t arc_connect_pmu_pdccnt_read(void)
{
	ARC_CONNECT_OP_T op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_PMU_READ_PDCNT, 0);

	return arc_connect_cmd_execute(&op);
}
