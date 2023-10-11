incdir = include_directories('../include')

utils = static_library('utils', 'utils.c', include_directories: incdir)

link = executable('linker', 'link.c',
                  include_directories: incdir,
                  link_with: [msg])
test('test-linking', link)

# tests that only run on POSIX compatible systems
if posix
	unistd = executable('unistd-test', 'unistd-test.c',
	                     include_directories: incdir,
	                     link_with: [utils, msg])
	test('unistd', unistd)
	arduino = executable('arduino-test', 'arduino/host.c',
	                     include_directories: incdir,
	                     link_with: [utils, msg])
	test('arduino', arduino)
endif
