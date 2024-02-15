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
endif

incdirTest = include_directories('include')

cpp_testSources = files(
	'MsgTest.cpp',
)

MsgTest = executable('MsgTest', cpp_testSources, include_directories: [incdir, incdirTest], link_with: [msg_cpp])

test('MsgTest', MsgTest)