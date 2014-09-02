ccore
=====

is a simple abstraction layer between different Operating Systems. It's focused on Game Development, and only tries to abstract the components of a Operating System which are required to develop a game, and to stay as close as possible to those components. It is possible to statically build an application using ccore with a filesize of ~1MB.

###Features###
The features of ccore include support for the following:
- Creating a window with an openGL contex (with glew built in to load extensions)
- Managing multiple monitors, multi monitor fullscreen
- Raw input with gamepad support
- Threading
- Networking
- Cross platform implementations for several OS specific functions

How to compile
==============
###Linux###
The following libraries are needed:
- scons
- libglu1-mesa-dev
- libx11-dev
- libxrandr-dev
- libxinerama-dev
- libxi-dev

**Release:**
Run `make && sudo make install` to install ccore.

**Test:**
Change the "custom.py" file to:

<pre><code>test = 'yes'<br\>
target = 'debug'
</code></pre>

Run <code>scons && bin/featuretest</code>, and check if all the features work.

###Windows###
A Microsoft Visual Studio 2013 project file is provided for building the test file.
When building in Visual Studio, all libraries are automatically linked. Otherwise, the user should link against the following libraries:
- opengl32
- hid.lib
- XInput9_1_0.lib

### TODO ###
**Major:**
- Mac & BSD support

**Extensions:**
- Sound
- Game Icon
