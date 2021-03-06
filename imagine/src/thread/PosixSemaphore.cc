/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <imagine/thread/Semaphore.hh>
#include <imagine/logger/logger.h>
#include <utility>

namespace IG
{

Semaphore::Semaphore(unsigned int startValue)
{
	sem_init(&sem, 0, startValue);
}

PosixSemaphore::PosixSemaphore(PosixSemaphore &&o)
{
	*this = std::move(o);
}

PosixSemaphore &PosixSemaphore::operator=(PosixSemaphore &&o)
{
	deinit();
	sem = std::exchange(o.sem, {});
	return *this;
}

PosixSemaphore::~PosixSemaphore()
{
	deinit();
}

void Semaphore::wait()
{
	if(sem_wait(&sem))
	{
		logErr("error in sem_wait()");
	}
}

void Semaphore::notify()
{
	if(sem_post(&sem))
	{
		logErr("error in sem_post()");
	}
}

void PosixSemaphore::deinit()
{
	sem_destroy(&sem);
}

}
