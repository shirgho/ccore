#!/bin/bash

cd "$( dirname "${BASH_SOURCE[0]}" )"

if scons -Q
then 
	xterm -e "sleep 2 && gdb -tui -x=~/.config/gdbinit ./test-x11 && scons -Q -c" &
	valgrind --vgdb=yes --vgdb-error=0 ./test-x11
fi
