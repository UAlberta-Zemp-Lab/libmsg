c_sources = files(
	'msg/msg.c',
	'msg/msghandler.c',
	# Backends
)

cpp_sources = files(
	'msg/Msg.cpp',
)

if posix
	c_sources += files(
		# Backends
		'unistd/unistd.c',
	)
endif


msg = library('msg', c_sources, install: true, include_directories: incdir)
msg_cpp = library('msg++', cpp_sources, install: true, include_directories: incdir)

libmsg_dep = declare_dependency(link_with : msg, include_directories : incdir)
libmsg_cpp_dep = declare_dependency(link_with : msg_cpp, include_directories : incdir)