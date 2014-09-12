all: debug

debug:
	scons target="debug"

release:
	scons -Q target="release"

clean:
	scons -c

install:
	scons target="install" install

uninstall:
	scons -c install
