env = Environment()

build_toolchain = ARGUMENTS.get('toolchain', 'devtoolset3')
if build_toolchain == 'devtoolset3':
  env.Replace(CXX='/opt/rh/devtoolset-3/root/usr/bin/g++')
  env.Replace(CC='/opt/rh/devtoolset-3/root/usr/bin/gcc')
  
build_type = ARGUMENTS.get('build', 'debug')
if build_type.lower() == 'debug':
  env['CXXFLAGS'] += ['-g']
else:
  env['CXXFLAGS'] += ['-O3']

env.Append(CPPPATH=['/usr/local/include'])
env.Append(LIBPATH=['/usr/local/lib'])
env.Append(CXXFLAGS=["--std=c++11"])
env.Append(LINKFLAGS=[
        '-undefined',
        '-pipe',
        '-W',
        '-Wall',
        '-Werror',
        '-fPIC',
        '-g',
])
env.Append(LIBS=[
        'z',
        'pthread',
])

SRC=Glob("*.c*")
env.Program(target="mac", source=[SRC], LIBS=[])

