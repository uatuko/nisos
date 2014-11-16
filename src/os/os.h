/*
*  Copyright (c) 2014 Uditha Atukorala
*
*/

#ifndef OS_H
#define OS_H

#include <os/kernel/kernel.h>


namespace os {

	// global kernel instance for the os
	extern kernel::kernel * kernel;

	void boot();
	void run();

} /* end of namespace os */

#endif /* !OS_H */

