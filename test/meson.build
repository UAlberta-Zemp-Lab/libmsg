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

cpp_testSources = files(
	'MsgTest.cpp',
	'MsgHandlerTest.cpp',
	'util/DualPort.cpp'
)

MsgTest = executable('MsgTest', cpp_testSources, objects: cppSourcesLibObj, include_directories: incdir, dependencies: [catch2], install : true)

test('MsgTest', MsgTest)