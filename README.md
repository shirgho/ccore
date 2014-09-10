ccore
=====

Ccore is a light weight low level library to facilitate cross platform software development in C. CCore is focused on but not exclusively aimed at game development. It provides the user with basic functionality while leaving high level implementations up to the user.

Ccore is described in detail in [the ccore wiki.](../../wiki)

###Features###
The features of ccore include support for the following:
- Creating a window with an openGL context (with glew built in to load extensions)
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
