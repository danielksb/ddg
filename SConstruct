import os

HOME = os.environ['HOME']

CCFLAGS = ' `pkg-config --cflags libcurl jansson` '
LINKFLAGS = ' `pkg-config --libs libcurl jansson` '

INSTALLDIR = HOME + '/.bin'

env = Environment(CCFLAGS=CCFLAGS, LINKFLAGS=LINKFLAGS)
ddg = env.Program('ddg.c')
env.Install(INSTALLDIR, ddg);
env.Alias('install', INSTALLDIR)

