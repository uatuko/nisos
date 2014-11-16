/*
*  Copyright (c) 2014 Uditha Atukorala
*
*/

#include "sem.h"
#include "os.h"


namespace os {

	sem::sem( key_t sem_key ) {

		// initialise the system semaphore
		_sem = os::kernel->sem_open( sem_key );

	}


	void sem::wait() {
		os::kernel->sem_wait( _sem );
	}


	void sem::signal() {
		os::kernel->sem_post( _sem );
	}

} /* end of namespace os */

