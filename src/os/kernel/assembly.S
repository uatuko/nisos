/*
*  Copyright (c) 2014 Uditha Atukorala
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

