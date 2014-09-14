import os

env=Environment(CC='gcc', CCFLAGS='-Iinclude/')

sources=[Glob('lib/*/*.c'), Glob('src/ccore/linux/*/*.c'), Glob('src/ccore/common/*/*.c'), Glob('src/ccore/x11/*/*.c')]
libs=['X11', 'Xrandr', 'Xinerama', 'Xi', 'GL', 'GLU', 'pthread']
libpaths=['/usr/lib', '/usr/local/lib', '.']

opts=Variables('custom.py', ARGUMENTS)
opts.Add('target', 'Compile Target (debug/release/install)', 'debug', allowed_values=('debug', 'release', 'install'))
opts.Add('test', 'Create test files in the bin folder', 'no', allowed_values=('yes', 'all', 'net', 'feature', 'no'))
opts.Update(env)

if(env['target']=='debug'):
    env.Append(CCFLAGS=['-D_DEBUG'])
    env.Append(CCFLAGS=['-g'])
    env.Append(CCFLAGS=['-Wall'])

if(env['target']=='install'):
    libInstall = env.Install(dir='/usr/lib', source='lib/libccore.a')
    headerInstall = env.Install(dir='/usr/include', source='include/ccore')
    Clean(libInstall, '/usr/include/ccore')
    env.Alias('install', '/usr')
else:
    staticLibrary=env.Library(target='lib/ccore', source=sources, LIBS=libs, LIBPATH=libpaths)
    if(env['test']=='yes' or env['test']=='all' or env['test']=='feature'):
        env.Program(target='bin/featuretest', source=['test/featuretest.c', 'test/tga.c', 'test/cursor.c'], LIBS=[staticLibrary, libs], LIBPATH=libpaths)
        Command("bin/commands.tga", "test/commands.tga", Copy("$TARGET", "$SOURCE"))
        Command("bin/logo.tga", "test/logo.tga", Copy("$TARGET", "$SOURCE"))

    if(env['test']=='yes' or env['test']=='all' or env['test']=='net'):
        env.Program(target='bin/nettest', source='test/nettest.c', LIBS=[staticLibrary], LIBPATH=libpaths)

Help(opts.GenerateHelpText(env))
