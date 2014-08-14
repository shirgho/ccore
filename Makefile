all: debug

debug:
	scons target="debug"

release:
	scons -Q target="release"

clean:
	scons -c
