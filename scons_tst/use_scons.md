#install
```sh
$ tar -xzvf scons-2.4.1.tar.gz
$ cd scons-2.4.1 
$ sudo python setup.py install
```
#use scons
##basic
```sh
$ ls 
main.c log.c SConstruct
$ scons

$ cat SConstruct
Program('main',['main.c','log.c'])
##other parameter
clean 
scons -c 
indicate file
scons -f
```
#SConstruct file
##1
Program('helloscons2', ['helloscons2.c', 'file1.c', 'file2.c']
LIBS = 'm'
LIBPATH = ['/usr/lib', '/usr/local/lib']
CCFLAGS = ('-DHELLOSCONS')
CPPPATH = ['include', '/home/project/inc']
##2
StaticLibrary('foo', ['f1.c', 'f2.c', 'f3.c'])

SharedLibrary('foo', ['f1.c', 'f2.c', 'f3.c']) 
