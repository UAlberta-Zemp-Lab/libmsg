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
install_subdir('include', install_dir: 'include', strip_directory: true) #should probably use install_headers instead

# determine the platform being built for
win32 = host_machine.system() == 'cygwin' or host_machine.system() == 'windows'
posix = not win32

c_sources = files(
	'src/msg/msg.c',
	'src/msg/msghandler.c',
	# Backends
)

cpp_sources = files(
	'src/msg/Msg.cpp',
	'src/msg/MsgHandler.cpp',
	'src/msg/communicator/SerialCommunicator.cpp',
	'src/msg/communicator/WorkerThread.cpp',
	'src/msg/MsgLog.cpp',
)

if posix
	c_sources += files(
		# Backends
		'src/unistd/unistd.c',
	)
endif


msg = library('msg', c_sources, install: true, include_directories: incdir)
msg_cpp = library('msg++', cpp_sources, install: true, include_directories: incdir)

libmsg_dep = declare_dependency(link_with : msg, include_directories : incdir)
libmsg_cpp_dep = declare_dependency(link_with : msg_cpp, include_directories : incdir)

subdir('test')
