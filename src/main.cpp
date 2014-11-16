/*
*  Copyright (c) 2014 Uditha Atukorala
*
*/

#include <os/os.h>
#include <os/sem.h>


#define SEM_KEY         1
#define BLINK_PORT      GPIOC
#define BLINK_PIN       14
#define BLINK_RCC_BIT   RCC_APB2Periph_GPIOC


void * task( void * arg __attribute__((unused)) ) {

	os::sem sem( SEM_KEY );
	uint32_t counter = 0;

	RCC_APB2PeriphClockCmd( BLINK_RCC_BIT, ENABLE );
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin   = ( 1 << BLINK_PIN );
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(BLINK_PORT, &GPIO_InitStructure);

	// on
	GPIO_ResetBits( BLINK_PORT, ( 1 << BLINK_PIN ) );


	while ( true ) {

		// wait for the semaphore
		sem.wait();

		if ( ++counter % 2 ) {
			GPIO_ResetBits( BLINK_PORT, ( 1 << BLINK_PIN ) );
		} else {
			GPIO_SetBits( BLINK_PORT, ( 1 << BLINK_PIN ) );
		}

	}

	return 0;

}


void * another_task( void * arg __attribute__((unused)) ) {

	os::sem sem( SEM_KEY );

	while ( true ) {

		// signal the semaphore
		sem.signal();

		// some delay (~25ms)
		for ( size_t i = 0; i < 100000; i++ );

	}

	return 0;

}



int main() {

	// boot os
	os::boot();

	// launch tasks
	os::kernel->launch( task, 64 );
	os::kernel->launch( another_task, 64 );

	// hand over to the os
	os::run();

	return 0;

}

