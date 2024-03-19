project('libmsg',
	['c', 'cpp'],
	license: 'ISC',
	version: files('./VERSION'),
	default_options: [
		'debug=true',
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
	'src/msg/msghandler.c',
	# Backends
)

if posix
	c_sources += files(
		# Backends
		'src/unistd/unistd.c',
	)
elif win32
	c_sources += files(
		# Backends
		'src/w32serial/device.c',
		'src/w32serial/w32serial.c',
	)
endif

msg = library('msg', c_sources, install: true, include_directories: incdir)
libmsg = declare_dependency(link_with: msg, include_directories: incdir)

subdir('test')
