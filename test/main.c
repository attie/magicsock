/*
	magicsock - a magical socket library

	Copyright (C) 2014 onwards  Attie Grande (attie@attie.co.uk)

	magicsock is free software: you can redistribute it and/or modify it
	under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	magicsock is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with magicsock. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <magicsock.h>

void my_cb(void *ctx, const char *addr, int fd) {
	printf("connection from '%s' on fd %d\n", addr, fd);
	sleep(2);
	printf("fd %d finished!\n", fd);
}

int main(int argc, char *argv[]) {
	struct ms_handle *s;
	int ret;

	if ((s = ms_start_tcp(27015, "context info", my_cb, 1)) == NULL) {
		printf("ms_start_tcp() failed...\n");
		return -1;
	}

	sleep(10);

	if ((ret = ms_stop(s)) != 0) {
		printf("ms_stop() failed... returned %d\n", ret);
	}

	return 0;
}
