ccore
=====

A development library for games which makes it easier to use low level system functionality while maintaining cross-platform useabillity.
It is currently developed for Win32 (Windows 2000 and higher) and Linux with X11. We are planning on adding a Mac version later.

How to compile
==============
###Linux###
The following libraries are needed:
- scons
- libglu1-mesa-dev
- libx11-dev
- libxrandr-dev
- libxinerama-dev

**Release:**
Run 'scons' and include the files from the include folder like 'featuretest.c' in the test folder.

**Test:**
Change the "custom.py" file to:

<pre><code>test = 'yes'<br\>
target = 'debug'
</code></pre>

Run <code>scons && bin/featuretest</code>, and check if all the features work.

### TODO ###
**Major:**
- Gamepad support
- Mac & BSD support

**Extensions:**
- Networking
- Sound
- Game Icon
