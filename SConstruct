CCFLAGS = ' `pkg-config --cflags libcurl jansson` '
LINKFLAGS = ' `pkg-config --libs libcurl jansson` '

env = Environment(CCFLAGS=CCFLAGS, LINKFLAGS=LINKFLAGS)
env.Program('ddg.c')
