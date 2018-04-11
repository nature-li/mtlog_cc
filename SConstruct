env = Environment(LINKCOM = "$LINK -o $TARGET $SOURCES $_LIBDIRFLAGS $_LIBFLAGS $dynamic_libs $LINKFLAGS", LINKFLAGS = "-static -pthread -Wl,-rpath,./:../lib -Wl,--dynamic-linker=/lib64/ld-linux-x86-64.so.2"	)

build_toolchain = ARGUMENTS.get('toolchain', 'devtoolset3')
if build_toolchain == 'devtoolset3':
  env.Replace(CXX='/opt/rh/devtoolset-3/root/usr/bin/g++')
  env.Replace(CC='/opt/rh/devtoolset-3/root/usr/bin/gcc')
  
build_type = ARGUMENTS.get('build', 'debug')
if build_type.lower() == 'debug':
  env['CXXFLAGS'] += ['-g']
else:
  env['CXXFLAGS'] += ['-O3']

env['CXXFLAGS'] += ['--std=c++11', '-Wall', '-pthread']
env.Append(dynamic_libs='-Wl,-Bdynamic -lm -lc -lrt -ldl')

env.Append(CPPPATH=['/usr/local/include'])
env.Append(LIBPATH=['/usr/local/lib'])

SRC=Glob("*.c*")
env.Program(target="mac", source=[SRC], LIBS=[])
