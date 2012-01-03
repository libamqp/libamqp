/*
   Copyright 2011-2012 StormMQ Limited

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#include <errno.h>
#include <stdlib.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "libamqp_common.h"
#include "Misc/IO.h"

int write_all(int fd, const char *buffer, size_t n)
{
    int count = n;
    int written;
    while ((written = write(fd, buffer, count)) >= 0 || errno == EINTR)
    {
        buffer += written;
        count -= written;
	if (count == 0)
	{
	    return n;
	}
    }
    return written;
}