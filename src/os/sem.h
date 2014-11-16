/*
*  Copyright (c) 2014 Uditha Atukorala
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

