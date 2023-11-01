incdir = include_directories('../include')

catch2 = dependency('catch2', fallback : ['catch2', 'catch2_with_main_dep'])

utils = static_library('utils', 'utils.c', include_directories: incdir)

# tests that only run on POSIX compatible systems
if posix
	unistd = executable('unistd-test', 'unistd-test.c',
	                     include_directories: incdir,
	                     link_with: [utils, libmsg])
	test('unistd', unistd)
endif

cppSourcesLibObj = libmsg_cpp.extract_objects(cpp_sources)

incdirTest = include_directories('include')

libmsg_cpp_test = library('msg_cpp_test', install: true, include_directories: [incdirTest], dependencies: [catch2, libmsg_cpp_dep])

libmsg_cpp_test_dep = declare_dependency(include_directories : incdirTest, link_with : libmsg_cpp_test)

testHeaders = files(
	'include/msg/test/BaseMsgTests.hpp',
	'include/msg/test/DualPort.hpp'
)
install_headers(testHeaders, subdir: 'msg/test')

cpp_testSources = files(
	'MsgTest.cpp',
	'MsgHandlerTest.cpp',
	'SerialCommunicatorTest.cpp'
)

MsgTest = executable('MsgTest', [cpp_testSources], objects: cppSourcesLibObj, include_directories: [incdir, incdirTest], dependencies: [catch2], install : true)

test('MsgTest', MsgTest)