import os

env = Environment(CC='gcc', CCFLAGS = '-g -Wall -O2')

env.Program(target = 'test', source = Glob('test/*.c'), LIBPATH=['/usr/lib', '/usr/local/lib', '.'])
