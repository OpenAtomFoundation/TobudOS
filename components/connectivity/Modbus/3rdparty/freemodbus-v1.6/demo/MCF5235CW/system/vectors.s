/*
    FreeRTOS MCF5235 port - Copyright (C) 2006 Christian Walter.
    Copyright @ 1995-2005 Freescale Semiconductor, Inc. All rights reserved

    FreeRTOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FreeRTOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FreeRTOS; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes FreeRTOS, without being obliged to provide
    the source code for any proprietary components.  See the licensing section
    of http://www.FreeRTOS.org for full details of how and when the exception
    can be applied.

    ***************************************************************************
    See http://www.FreeRTOS.org for documentation, latest information, license
    and contact details.  Please ensure to read the configuration and relevant
    port sections of the online documentation.
    ***************************************************************************
*/

	.global VECTOR_TABLE
	.global _VECTOR_TABLE
	.global start

	.extern ___SP_INIT
	.extern _asm_startmeup
	.extern _asm_exception_handler
	.extern _irq_handler
	.extern _prvvMBPortSerialISR
	.extern _prvvMBPortTimerISR
	.text

#define _irq_handler _asm_exception_handler

/*
 * Exception Vector Table
 */
VECTOR_TABLE:
_VECTOR_TABLE:
INITSP:		.long	___SP_INIT				/* Initial SP			*/
INITPC:		.long	start				    /* Initial PC			*/
vector02:	.long	_asm_exception_handler	/* Access Error			*/
vector03:	.long	_asm_exception_handler	/* Address Error		*/
vector04:	.long	_asm_exception_handler	/* Illegal Instruction	*/
vector05:	.long	_asm_exception_handler	/* Reserved				*/
vector06:	.long	_asm_exception_handler	/* Reserved				*/
vector07:	.long	_asm_exception_handler	/* Reserved				*/
vector08:	.long	_asm_exception_handler	/* Privilege Violation	*/
vector09:	.long	_asm_exception_handler	/* Trace				*/
vector0A:	.long	_asm_exception_handler	/* Unimplemented A-Line	*/
vector0B:	.long	_asm_exception_handler	/* Unimplemented F-Line	*/
vector0C:	.long	_asm_exception_handler	/* Debug Interrupt		*/
vector0D:	.long	_asm_exception_handler	/* Reserved				*/
vector0E:	.long	_asm_exception_handler	/* Format Error			*/
vector0F:	.long	_asm_exception_handler	/* Unitialized Int.		*/
vector10:	.long	_asm_exception_handler	/* Reserved				*/
vector11:	.long	_asm_exception_handler	/* Reserved				*/
vector12:	.long	_asm_exception_handler	/* Reserved				*/
vector13:	.long	_asm_exception_handler	/* Reserved				*/
vector14:	.long	_asm_exception_handler	/* Reserved				*/
vector15:	.long	_asm_exception_handler	/* Reserved				*/
vector16:	.long	_asm_exception_handler	/* Reserved				*/
vector17:	.long	_asm_exception_handler	/* Reserved				*/
vector18:	.long	_asm_exception_handler	/* Spurious Interrupt	*/
vector19:	.long	_irq_handler			/* Autovector Level 1	*/
vector1A:	.long	_irq_handler			/* Autovector Level 2	*/
vector1B:	.long	_irq_handler			/* Autovector Level 3	*/
vector1C:	.long	_irq_handler			/* Autovector Level 4	*/
vector1D:	.long	_irq_handler			/* Autovector Level 5	*/
vector1E:	.long	_irq_handler			/* Autovector Level 6	*/
vector1F:	.long	_irq_handler			/* Autovector Level 7	*/
vector20:	.long	_asm_exception_handler	/* TRAP #0				*/
vector21:	.long	_asm_exception_handler	/* TRAP #1				*/
vector22:	.long	_asm_exception_handler	/* TRAP #2				*/
vector23:	.long	_asm_exception_handler	/* TRAP #3				*/
vector24:	.long	_asm_exception_handler	/* TRAP #4				*/
vector25:	.long	_asm_exception_handler	/* TRAP #5				*/
vector26:	.long	_asm_exception_handler	/* TRAP #6				*/
vector27:	.long	_asm_exception_handler	/* TRAP #7				*/
vector28:	.long	_asm_exception_handler	/* TRAP #8				*/
vector29:	.long	_asm_exception_handler	/* TRAP #9				*/
vector2A:	.long	_asm_exception_handler	/* TRAP #10				*/
vector2B:	.long	_asm_exception_handler	/* TRAP #11				*/
vector2C:	.long	_asm_exception_handler	/* TRAP #12				*/
vector2D:	.long	_asm_exception_handler	/* TRAP #13				*/
vector2E:	.long	_asm_exception_handler	/* TRAP #14				*/
vector2F:	.long	_asm_exception_handler	/* TRAP #15				*/
vector30:	.long	_asm_exception_handler	/* Reserved				*/
vector31:	.long	_asm_exception_handler	/* Reserved				*/
vector32:	.long	_asm_exception_handler	/* Reserved				*/
vector33:	.long	_asm_exception_handler	/* Reserved				*/
vector34:	.long	_asm_exception_handler	/* Reserved				*/
vector35:	.long	_asm_exception_handler	/* Reserved				*/
vector36:	.long	_asm_exception_handler	/* Reserved				*/
vector37:	.long	_asm_exception_handler	/* Reserved				*/
vector38:	.long	_asm_exception_handler	/* Reserved				*/
vector39:	.long	_asm_exception_handler	/* Reserved				*/
vector3A:	.long	_asm_exception_handler	/* Reserved				*/
vector3B:	.long	_asm_exception_handler	/* Reserved				*/
vector3C:	.long	_asm_exception_handler	/* Reserved				*/
vector3D:	.long	_asm_exception_handler	/* Reserved				*/
vector3E:	.long	_asm_exception_handler	/* Reserved				*/
vector3F:	.long	_asm_exception_handler	/* Reserved				*/
vector40:	.long	_irq_handler
vector41:	.long	_irq_handler
vector42:	.long	_irq_handler
vector43:	.long	_irq_handler
vector44:	.long	_irq_handler
vector45:	.long	_irq_handler
vector46:	.long	_irq_handler
vector47:	.long	_irq_handler
vector48:	.long	_irq_handler
vector49:	.long	_irq_handler
vector4A:	.long	_irq_handler
vector4B:	.long	_irq_handler
vector4C:	.long	_irq_handler
vector4D:	.long	_prvvMBPortSerialISR
vector4E:	.long	_irq_handler
vector4F:	.long	_irq_handler
vector50:	.long	_irq_handler
vector51:	.long	_irq_handler
vector52:	.long	_irq_handler
vector53:	.long	_irq_handler
vector54:	.long	_irq_handler
vector55:	.long	_irq_handler
vector56:	.long	_irq_handler
vector57:	.long	_irq_handler
vector58:	.long	_irq_handler
vector59:	.long	_irq_handler
vector5A:	.long	_irq_handler
vector5B:	.long	_irq_handler
vector5C:	.long	_irq_handler
vector5D:	.long	_irq_handler
vector5E:	.long	_irq_handler
vector5F:	.long	_irq_handler
vector60:	.long	_irq_handler
vector61:	.long	_irq_handler
vector62:	.long	_irq_handler
vector63:	.long	_irq_handler
vector64:	.long	_irq_handler
vector65:	.long	_prvvMBPortTimerISR
vector66:	.long	_irq_handler
vector67:	.long	_irq_handler
vector68:	.long	_irq_handler
vector69:	.long	_irq_handler
vector6A:	.long	_irq_handler
vector6B:	.long	_irq_handler
vector6C:	.long	_irq_handler
vector6D:	.long	_irq_handler
vector6E:	.long	_irq_handler
vector6F:	.long	_irq_handler
vector70:	.long	_irq_handler
vector71:	.long	_irq_handler
vector72:	.long	_irq_handler
vector73:	.long	_irq_handler
vector74:	.long	_irq_handler
vector75:	.long	_irq_handler
vector76:	.long	_irq_handler
vector77:	.long	_irq_handler
vector78:	.long	_irq_handler
vector79:	.long	_irq_handler
vector7A:	.long	_irq_handler
vector7B:	.long	_irq_handler
vector7C:	.long	_irq_handler
vector7D:	.long	_irq_handler
vector7E:	.long	_irq_handler
vector7F:	.long	_irq_handler
vector80:	.long	_irq_handler
vector81:	.long	_irq_handler
vector82:	.long	_irq_handler
vector83:	.long	_irq_handler
vector84:	.long	_irq_handler
vector85:	.long	_irq_handler
vector86:	.long	_irq_handler
vector87:	.long	_irq_handler
vector88:	.long	_irq_handler
vector89:	.long	_irq_handler
vector8A:	.long	_irq_handler
vector8B:	.long	_irq_handler
vector8C:	.long	_irq_handler
vector8D:	.long	_irq_handler
vector8E:	.long	_irq_handler
vector8F:	.long	_irq_handler
vector90:	.long	_irq_handler
vector91:	.long	_irq_handler
vector92:	.long	_irq_handler
vector93:	.long	_irq_handler
vector94:	.long	_irq_handler
vector95:	.long	_irq_handler
vector96:	.long	_irq_handler
vector97:	.long	_irq_handler
vector98:	.long	_irq_handler
vector99:	.long	_irq_handler
vector9A:	.long	_irq_handler
vector9B:	.long	_irq_handler
vector9C:	.long	_irq_handler
vector9D:	.long	_irq_handler
vector9E:	.long	_irq_handler
vector9F:	.long	_irq_handler
vectorA0:	.long	_irq_handler
vectorA1:	.long	_irq_handler
vectorA2:	.long	_irq_handler
vectorA3:	.long	_irq_handler
vectorA4:	.long	_irq_handler
vectorA5:	.long	_irq_handler
vectorA6:	.long	_irq_handler
vectorA7:	.long	_irq_handler
vectorA8:	.long	_irq_handler
vectorA9:	.long	_irq_handler
vectorAA:	.long	_irq_handler
vectorAB:	.long	_irq_handler
vectorAC:	.long	_irq_handler
vectorAD:	.long	_irq_handler
vectorAE:	.long	_irq_handler
vectorAF:	.long	_irq_handler
vectorB0:	.long	_irq_handler
vectorB1:	.long	_irq_handler
vectorB2:	.long	_irq_handler
vectorB3:	.long	_irq_handler
vectorB4:	.long	_irq_handler
vectorB5:	.long	_irq_handler
vectorB6:	.long	_irq_handler
vectorB7:	.long	_irq_handler
vectorB8:	.long	_irq_handler
vectorB9:	.long	_irq_handler
vectorBA:	.long	_irq_handler
vectorBB:	.long	_irq_handler
vectorBC:	.long	_irq_handler
vectorBD:	.long	_irq_handler
vectorBE:	.long	_irq_handler
vectorBF:	.long	_irq_handler
vectorC0:	.long	_irq_handler
vectorC1:	.long	_irq_handler
vectorC2:	.long	_irq_handler
vectorC3:	.long	_irq_handler
vectorC4:	.long	_irq_handler
vectorC5:	.long	_irq_handler
vectorC6:	.long	_irq_handler
vectorC7:	.long	_irq_handler
vectorC8:	.long	_irq_handler
vectorC9:	.long	_irq_handler
vectorCA:	.long	_irq_handler
vectorCB:	.long	_irq_handler
vectorCC:	.long	_irq_handler
vectorCD:	.long	_irq_handler
vectorCE:	.long	_irq_handler
vectorCF:	.long	_irq_handler
vectorD0:	.long	_irq_handler
vectorD1:	.long	_irq_handler
vectorD2:	.long	_irq_handler
vectorD3:	.long	_irq_handler
vectorD4:	.long	_irq_handler
vectorD5:	.long	_irq_handler
vectorD6:	.long	_irq_handler
vectorD7:	.long	_irq_handler
vectorD8:	.long	_irq_handler
vectorD9:	.long	_irq_handler
vectorDA:	.long	_irq_handler
vectorDB:	.long	_irq_handler
vectorDC:	.long	_irq_handler
vectorDD:	.long	_irq_handler
vectorDE:	.long	_irq_handler
vectorDF:	.long	_irq_handler
vectorE0:	.long	_irq_handler
vectorE1:	.long	_irq_handler
vectorE2:	.long	_irq_handler
vectorE3:	.long	_irq_handler
vectorE4:	.long	_irq_handler
vectorE5:	.long	_irq_handler
vectorE6:	.long	_irq_handler
vectorE7:	.long	_irq_handler
vectorE8:	.long	_irq_handler
vectorE9:	.long	_irq_handler
vectorEA:	.long	_irq_handler
vectorEB:	.long	_irq_handler
vectorEC:	.long	_irq_handler
vectorED:	.long	_irq_handler
vectorEE:	.long	_irq_handler
vectorEF:	.long	_irq_handler
vectorF0:	.long	_irq_handler
vectorF1:	.long	_irq_handler
vectorF2:	.long	_irq_handler
vectorF3:	.long	_irq_handler
vectorF4:	.long	_irq_handler
vectorF5:	.long	_irq_handler
vectorF6:	.long	_irq_handler
vectorF7:	.long	_irq_handler
vectorF8:	.long	_irq_handler
vectorF9:	.long	_irq_handler
vectorFA:	.long	_irq_handler
vectorFB:	.long	_irq_handler
vectorFC:	.long	_irq_handler
vectorFD:	.long	_irq_handler
vectorFE:	.long	_irq_handler
vectorFF:	.long	_irq_handler

start:
	move.w	#0x2700,sr
	jmp 	  _asm_startmeup
	.end
