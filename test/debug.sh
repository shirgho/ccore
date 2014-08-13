#!/bin/bash

cd "$( dirname "${BASH_SOURCE[0]}" )"

if scons -Q
then 
	xterm -e "sleep 2 && gdb -tui -x=~/.config/gdbinit ./featuretest && scons -Q -c" &
	valgrind --vgdb=yes --vgdb-error=0 ./featuretest
fi
