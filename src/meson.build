c_sources = files(
	'msg/msg.c',
	'msg/msghandler.c',
	# Backends
)

if posix
	c_sources += files(
		# Backends
		'unistd/unistd.c',
	)
endif


msg = library('msg', c_sources, install: true, include_directories: incdir)

libmsg_dep = declare_dependency(link_with : msg, include_directories : incdir)
