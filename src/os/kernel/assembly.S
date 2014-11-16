/*
*  Nisos - A basic multitasking Operating System for ARM Cortex-M3
*          processors
*
*  Copyright (c) 2014 Uditha Atukorala
*
*  This file is part of Nisos.
*
*  Nisos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Nisos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Nisos.  If not, see <http://www.gnu.org/licenses/>.
*
*/

				.syntax unified
				.cpu    cortex-m3
				.thumb
				.text

				.thumb_func
				.globl  context_switch
context_switch:
				// r0 = next stack ptr, r1 = current stack ptr
				str     r12, [r1, #0]
				ldr     r12, [r0, #0]
				msr     msp, r11
				ldmia   r12!, {r4 - r11, lr}
				msr     psp, r12
				bx      lr

