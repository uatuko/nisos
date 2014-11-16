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

#include "os.h"

extern "C" {
	void SysTick_Handler() __attribute__ ( ( isr ) );
	void PendSV_Handler()  __attribute__ ( ( isr, naked ) );
}


namespace os {

	// kernel instance
	kernel::kernel * kernel;

	void boot() {

		// disable interrupts
		__disable_irq();

		// instantiate the kernel
		kernel = new kernel::kernel();

	}


	void run() {

		kernel->system_task();

	}

} /* end of namespace os */



void SysTick_Handler() {
	os::kernel->systick_handler();
}


void PendSV_Handler() {

	// save context
	asm volatile (
		"mrs     r0, msp           \n"
		"push    {r4 - r11, lr}    \n"
		"mov     r11, r0           \n"
	);

	// call the kernel scheduler
	os::kernel->scheduler();

}

