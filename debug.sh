cd "$( dirname "${BASH_SOURCE[0]}" )"

if scons target="debug" test="yes"
then
	if [ $1 = "net" ]; then
		xterm -e "sleep 2 && gdb -tui -x=~/.config/gdbinit bin/nettest && scons -Q -c" &
		valgrind --vgdb=yes --vgdb-error=0 bin/nettest
	else
		xterm -e "sleep 2 && gdb -tui -x=~/.config/gdbinit bin/featuretest && scons -Q -c" &
		valgrind --vgdb=yes --vgdb-error=0 bin/featuretest
	fi
fi
