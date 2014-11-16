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

#ifndef OS_SEM_H
#define OS_SEM_H

#include <os/kernel/types.h>


namespace os {

	class sem {
	public:
		sem( key_t sem_key );
		virtual ~sem() { };

		void wait();
		void signal();

	private:
		kernel::sem_t * _sem;

	};

} /* end of namespace os */

#endif /* !OS_SEM_H */

