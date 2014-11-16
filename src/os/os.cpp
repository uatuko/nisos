/*
*  Copyright (c) 2014 Uditha Atukorala
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

