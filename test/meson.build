incdir = include_directories('../include')

utils = static_library('utils', 'utils.c', include_directories: incdir)

# tests that only run on POSIX compatible systems
if posix
	unistd = executable('unistd-test', 'unistd-test.c',
	                     include_directories: incdir,
	                     link_with: [utils, msg])
	test('unistd', unistd)
endif
