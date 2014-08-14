import os

env=Environment(CC='gcc', CCFLAGS='-Iinclude/')

sources=[Glob('lib/*/*.c'), Glob('src/ccore/linux/*/*.c'), Glob('src/ccore/common/*/*.c'), Glob('src/ccore/x11/*/*.c')]
libs=['X11', 'Xrandr', 'Xinerama', 'GL', 'GLU', 'pthread']
libpaths=['/usr/lib', '/usr/local/lib', '.']

opts=Variables('custom.py', ARGUMENTS)
opts.Add('target', 'Compile Target (debug/release)', 'debug', allowed_values=('debug', 'release'))
opts.Add('test', 'Create test files in the bin folder', 'no', allowed_values=('yes', 'no'))
opts.Update(env)

if(env['target']=='debug'):
    env.Append(CCFLAGS=['-D_DEBUG']);
    env.Append(CCFLAGS=['-g']);
    env.Append(CCFLAGS=['-Wall'])

staticLibrary=env.Library(target='lib/ccore', source=sources, LIBS=libs, LIBPATH=libpaths)

if(env['test']=='yes'):
    env.Program(target='bin/featuretest', source=Glob('test/*.c'), LIBS=[staticLibrary, libs], LIBPATH=libpaths);
    Command("bin/commands.tga", "test/commands.tga", Copy("$TARGET", "$SOURCE"));
    Command("bin/logo.tga", "test/logo.tga", Copy("$TARGET", "$SOURCE"))

Help(opts.GenerateHelpText(env))
