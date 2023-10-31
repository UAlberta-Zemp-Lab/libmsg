project('libmsg',
	['c', 'cpp'],
	license: 'ISC',
	version: files('./VERSION'),
	default_options: [
		'c_std=c99',
		'cpp_std=c++20',
		'debug=false',
		'default_library=both',
		'optimization=s',
		'warning_level=3',
		'werror=true',
	]
)

incdir = include_directories('include')

# determine the platform being built for
win32 = host_machine.system() == 'cygwin' or host_machine.system() == 'windows'
posix = not win32

c_sources = files(
	'src/msg/msg.c',
	# Backends
)

if posix
	c_sources += files(
		# Backends
		'src/unistd/unistd.c',
	)
endif

cpp_sources = files(
	'src/msg/msg.cpp',
	'src/msg/MsgHandler.cpp',
	'src/msg/communicator/SerialCommunicator.cpp',
	'src/msg/communicator/WorkerThread.cpp'
)

libmsg = library('msg', c_sources, install: true, include_directories: incdir)
libmsg_cpp = library('msg_cpp', cpp_sources, install: true, include_directories: incdir)

libmsg_dep = declare_dependency(include_directories : incdir, link_with : libmsg)
libmsg_cpp_dep = declare_dependency(include_directories : incdir, link_with : libmsg_cpp)

subdir('test')
