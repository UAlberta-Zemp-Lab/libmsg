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

cpp_test_lib_Sources = files(
	'include/msg/test/DualPort.hpp',
	'include/msg/test/BaseMsgTests.hpp',
)

msg_cpp_test = library('msg++_test', cpp_test_lib_Sources, install: true, include_directories: [incdirTest], link_with: [msg_cpp])

libmsg_cpp_test_dep = declare_dependency(include_directories : incdirTest, link_with : msg_cpp_test)

cpp_test_Sources = files(
	'msglib_cpp_tests.cpp',
	'MsgTest.cpp',
	'MsgHandlerTest.cpp',
)

MsgTest = executable('MsgTest', cpp_test_Sources, include_directories: [incdir, incdirTest], link_with: [msg_cpp, msg_cpp_test])

test('MsgTest', MsgTest)