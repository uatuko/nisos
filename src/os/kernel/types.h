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

#ifndef OS_TYPES_H
#define OS_TYPES_H

#include <cstddef>
#include <forward_list>
#include <sys/types.h>


#define KERNEL_SCHEDULER_FLAG 0x0001

#define TASK_ACTIVE_FLAG      0x0001
#define TASKS_SEM_WAIT_FLAG   0x0002


namespace kernel {

	typedef uint32_t systicks_t;

	typedef uint32_t status_t;

	typedef uint32_t memaddr_t;

	// semaphore data structure
	struct sem_t {
		key_t                       key;      ///< semaphore key
		volatile unsigned int       value;    ///< semaphore value
		std::forward_list<pid_t>    tasks;    ///< tasks linked with this semaphore

		sem_t( key_t key ) :
			key( key ), value( 0 ) { }

	private:
		virtual ~sem_t() { };

		friend class kernel;
	};

	// task table
	struct task_t {
		memaddr_t * sp;
		memaddr_t * stk_end;
		status_t status;

		task_t() :
			sp( 0 ), stk_end( 0 ), status( 0 ) { }
	};

	// context saved by the hardware
	struct stkctx_t {
		memaddr_t r0;
		memaddr_t r1;
		memaddr_t r2;
		memaddr_t r3;
		memaddr_t r12;
		memaddr_t lr;
		memaddr_t pc;
		memaddr_t psr;
	};

	// context saved by the software
	struct tskctx_t {
		memaddr_t r4;
		memaddr_t r5;
		memaddr_t r6;
		memaddr_t r7;
		memaddr_t r8;
		memaddr_t r9;
		memaddr_t r10;
		memaddr_t r11;
		memaddr_t lr;
	};

} /* end of namespace kernel */

#endif /* !OS_TYPES_H */

