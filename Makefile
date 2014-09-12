all: debug

debug:
	scons target="debug"

release:
	scons -Q target="release"

clean:
	scons -c

install:
	scons -Q target="install" install

uninstall:
	scons -Q -c target="install" install
