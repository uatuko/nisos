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

#include "kernel.h"
#include <os/cmsis/core_cm3.h>

#include <cstring>


#ifndef SYS_IDLE_STACK_SIZE
#define SYS_IDLE_STACK_SIZE 16
#endif


namespace kernel {

	kernel::kernel() : _systicks( 0 ), _status( 0 ), _current_task( 0 ) {

		// reserve memory for task stack space
		_stk_space = new memaddr_t [KERNEL_TASK_STACK_SIZE];
		memset( _stk_space, 0, ( sizeof( memaddr_t ) * KERNEL_TASK_STACK_SIZE ) );

		// set free stack pointer
		_free_stkp = _stk_space;


		// set interrupt priority grouping to be preemptive (no sub-priority)
		NVIC_SetPriorityGrouping( 0b011 );

		// configure system tick frequency
		SysTick_Config( SystemCoreClock / SYSTICK_FREQUENCY_HZ );

		// clear base priority for exception processing
		__set_BASEPRI( 0 );

		// set PendSV to the lowest priority
		NVIC_SetPriority( PendSV_IRQn, 0xFF );

	}


	memaddr_t * kernel::reserve_stack( size_t stk_size ) {

		memaddr_t * stack_space = 0;

		// add required stack size for context switching
		stk_size += ( sizeof( stkctx_t ) + sizeof( tskctx_t ) ) / sizeof( memaddr_t );

		// do we have enough stack space left?
		if ( ( _free_stkp + stk_size ) < ( _stk_space + KERNEL_TASK_STACK_SIZE ) )  {

			_free_stkp += stk_size;
			stack_space = ( _free_stkp - 1 );

		}

		return stack_space;

	}


	pid_t kernel::reschedule() {

		pid_t next_task = 0;

		for ( pid_t task = 1; task <= MAX_TASKS; task++ ) {

			if ( ( _tasks[task].status & TASKS_SEM_WAIT_FLAG )
				|| ( task == _current_task ) ) {
				continue;
			}

			if ( _tasks[task].status & TASK_ACTIVE_FLAG ) {

				next_task = task;
				break;

			}

		}

		return next_task;

	}


	pid_t kernel::launch( void * (* task)(void *), size_t stk_size ) {

		// get the next available pid from the task table
		pid_t pid = -1;

		for ( size_t i = 1; i <= MAX_TASKS; i++ ) {
			if (! ( _tasks[i].status & TASK_ACTIVE_FLAG ) ) {
				pid = i;
				break;
			}
		}


		if ( pid > 0 ) {

			// reserve stack memory
			memaddr_t * stack = reserve_stack( stk_size );

			if ( stack ) {

				// setup context
				stkctx_t * s_ctx = (stkctx_t *)( ( (uint32_t) stack ) - sizeof( stkctx_t ) );
				s_ctx->pc  = (uint32_t) task;
				s_ctx->psr = 0x01000000;              // default PSR value

				tskctx_t * t_ctx = (tskctx_t *)( ( (uint32_t) s_ctx ) - sizeof( tskctx_t ) );
				t_ctx->lr = 0xFFFFFFFD;


				// update task table
				_tasks[pid].stk_end = (memaddr_t *)( ( (uint32_t) stack ) - ( ( sizeof( stkctx_t ) + sizeof( tskctx_t ) ) + ( sizeof( memaddr_t ) * stk_size ) ) );
				_tasks[pid].sp      = (memaddr_t *)( ( (uint32_t) s_ctx ) - sizeof( tskctx_t ) );
				_tasks[pid].status |= TASK_ACTIVE_FLAG;


				// set the scheduler status flag
				_status |= KERNEL_SCHEDULER_FLAG;

			} else {

				// failure
				pid = -1;

			}

		}

		return pid;

	}


	void kernel::system_task() {

		// reset the MSP
		__set_MSP( (uint32_t) &__stack );

		// update task table
		_tasks[0].sp      = (memaddr_t *)( &__stack );
		_tasks[0].status |= TASK_ACTIVE_FLAG;

		// enable interrupts
		__enable_irq();

		// drop privileges
		__set_CONTROL( 0x01 );

		// system idle loop
		while ( true );

	}


	void kernel::systick_handler() {

		_systicks++;

		if ( _status & KERNEL_SCHEDULER_FLAG ) {
			_status   ^= KERNEL_SCHEDULER_FLAG;
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
		}

	}


	void kernel::scheduler() {

		pid_t next_task    = reschedule();
		pid_t current_task = _current_task;


		if ( next_task != 0 ) {

			// get a copy of MSP
			asm volatile ( "mrs     r12, msp" );

			if ( current_task != 0 ) {

				// copy the saved context onto task stack space
				asm volatile (
					"push    {r4 - r11}            \n"
					"ldmia   r12!, {r4 - r11, lr}  \n"
					"mrs     r12, psp              \n"
					"stmdb   r12!, {r4 - r11, lr}  \n"
					"pop     {r4 - r11}            \n"
				);

			} else {

				// context switch will try to reset the MSP
				asm volatile ( "mrs     r11, msp" );

			}

			_current_task = next_task;
			context_switch( _tasks[next_task].sp, _tasks[current_task].sp );

		}


		// we are switching back to system task

		// are we already running the system task?
		if ( current_task != 0 ) {

			// save user task context
			asm volatile (
				"mrs     r12, psp              \n"
				"str     r12, [%0, #0]         \n"
				: : "r" ( &_tasks[current_task].sp )
			);

		}

		// task switch to system task
		asm volatile (
			"msr      msp, %0           \n"
			"pop      {r4 - r11, lr}    \n"
			"bx       lr                \n"
			: : "r" ( &_tasks[0].sp )
		);

	}


	sem_t * kernel::sem_open( key_t key ) {

		sem_t * sem = 0;

		for ( std::forward_list< sem_t *>::iterator it = _sems.begin(); it != _sems.end(); it++ ) {

			if ( (* it)->key == key ) {
				sem = (* it);
				break;
			}

		}

		if (! sem ) {
			sem = new sem_t( key );
			_sems.push_front( sem );
		}

		return sem;

	}


	int kernel::sem_wait( sem_t * sem ) {

		pid_t pid = -1;

		if ( sem->value == 0 ) {

			for ( std::forward_list<pid_t>::iterator it = sem->tasks.begin(); it != sem->tasks.end(); it++ ) {
				if ( (* it) == _current_task ) {
					pid = _current_task;
				}
			}

			if ( pid == -1 ) {
				pid = _current_task;
				sem->tasks.push_front( pid );
			}

			// set the task status
			_tasks[pid].status |= TASKS_SEM_WAIT_FLAG;

			// set the scheduler status flag
			_status |= KERNEL_SCHEDULER_FLAG;

			// semaphore wait loop
			while ( sem->value == 0 );

		}

		// consume the semaphore
		sem->value--;

		return 0;

	}


	int kernel::sem_post( sem_t * sem ) {

		pid_t pid;

		if (! sem->tasks.empty() ) {

			pid = sem->tasks.front();
			sem->tasks.pop_front();

			// clear the task semaphore wait status
			_tasks[pid].status ^= TASKS_SEM_WAIT_FLAG;

		}

		// release the semaphore
		sem->value++;

		// set the scheduler status flag
		_status |= KERNEL_SCHEDULER_FLAG;

		return 0;

	}

} /* end of namespace os */

