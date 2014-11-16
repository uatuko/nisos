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

