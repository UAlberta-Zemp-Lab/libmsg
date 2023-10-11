project('libmsg',
	['c', 'cpp'],
	license: 'ISC',
	version: files('./VERSION'),
	default_options: [
		'c_std=c99',
		'debug=false',
		'default_library=both',
		'optimization=s',
		'warning_level=1',
	]
)

incdir = include_directories('include')

c_sources = files(
	'src/msg/msg.c',
	# Backends
	'src/unistd/unistd.c',
)

libmsg = library('msg', c_sources, install: true, include_directories: incdir)

subdir('test')
