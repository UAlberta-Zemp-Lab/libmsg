incdir = include_directories('../include')

utils = static_library('utils', 'utils.c', include_directories: incdir)

unistd = executable('unistd-test', 'unistd-test.c',
                     include_directories: incdir,
                     link_with: [utils, libmsg])
test('unistd', unistd)
