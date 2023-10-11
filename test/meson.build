incdir = include_directories('../include')

utils = static_library('utils', 'utils.c', include_directories: incdir)

# tests that only run on POSIX compatible systems
if posix
	unistd = executable('unistd-test', 'unistd-test.c',
	                     include_directories: incdir,
	                     link_with: [utils, libmsg])
	test('unistd', unistd)
	arduino = executable('arduino-test', 'arduino/host.c',
	                     include_directories: incdir,
	                     link_with: [utils, libmsg])
	test('arduino', arduino)
endif
