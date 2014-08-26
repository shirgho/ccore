ccore
=====

is a simple abstraction layer between different Operating Systems. It's focused on Game Development, and only tries to abstract the components of a Operating System which are required to develop a game, and to stay as close as possible to those components. It is possible to statically build an application using ccore with a filesize of ~1MB.

###Features###
The core features of ccore include support for the following:
- OpenGL
- Multithreading
- Fullscreen on multiple displays
- Raw input (mouse, keyboard & gamepads) for high-performance games
- Multiple gamepads

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

### TODO ###
**Major:**
- Mac & BSD support

**Extensions:**
- Networking
- Sound
- Game Icon
