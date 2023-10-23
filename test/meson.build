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
	'include/msg/test/DualPort.hpp',
)

libmsg_cpp_test = library('msg_cpp_test', cpp_testSources, install: true, include_directories: [incdirTest], dependencies: [libmsg_cpp_dep])

libmsg_cpp_test_dep = declare_dependency(include_directories : incdirTest, link_with : libmsg_cpp_test)

cpp_testSources += files(
	'msglib_cpp_tests.cpp',
	'MsgTest.cpp',
	'MsgHandlerTest.cpp',
)

MsgTest = executable('MsgTest', cpp_testSources, include_directories: [incdir, incdirTest], link_with: [msg_cpp])

test('MsgTest', MsgTest)