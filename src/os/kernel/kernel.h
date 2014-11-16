/*
*  Copyright (c) 2014 Uditha Atukorala
*
*/

#ifndef KERNEL_KERNEL_H
#define KERNEL_KERNEL_H

#include <stm32f10x.h>
#include <os_conf.h>
#include <os/kernel/types.h>


namespace os {
	extern void boot();
	extern void run();
}

extern uint32_t __stack;
extern "C" {
	void context_switch( kernel::memaddr_t * &next_spp, kernel::memaddr_t * &current_spp );
}



namespace kernel {

	class kernel {
	public:

		pid_t launch( void * (* task)(void *), size_t stk_size );
		void system_task();

		void systick_handler();
		void scheduler()       __attribute__ ( ( isr, naked ) );

		// semaphore interface
		sem_t * sem_open( key_t key );
		int sem_init( sem_t * sem, unsigned int value );
		int sem_wait( sem_t * sem );
		int sem_post( sem_t * sem );

	protected:

		task_t        _tasks[MAX_TASKS + 1];
		systicks_t    _systicks;
		status_t      _status;
		pid_t         _current_task;

		memaddr_t *   _stk_space;
		memaddr_t *   _free_stkp;

		std::forward_list<sem_t *> _sems;

		pid_t reschedule();


	private:
		kernel();
		virtual ~kernel() { };

		kernel( kernel const &copy );
		kernel &operator=( const kernel & copy );

		memaddr_t * reserve_stack( size_t stk_size );


		// give the boot sequence access to instantiate the kernel
		friend void os::boot();

	};

} /* end of namespace kernel */

#endif /* !KERNEL_KERNEL_H */

