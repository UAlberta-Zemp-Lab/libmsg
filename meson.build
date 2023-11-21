project('libmsg',
	['c', 'cpp'],
	license: 'ISC',
	version: files('./VERSION'),
	default_options: [
		'c_std=c99',
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
	'src/msg/msghandler.c',
	# Backends
)

if posix
	c_sources += files(
		# Backends
		'src/unistd/unistd.c',
	)
endif

msg = library('msg', c_sources, install: true, include_directories: incdir)
libmsg = declare_dependency(link_with: msg, include_directories: incdir)

subdir('test')
