project('libmsg',
	['c', 'cpp'],
	license: 'ISC',
	version: files('./VERSION'),
	default_options: [
		'c_std=c99',
		'cpp_std=c++20',
		'debug=false',
		'default_library=both',
		'optimization=s',
		'warning_level=3',
		'werror=true',
	]
)

incdir = include_directories('include')
install_subdir('include', install_dir: 'include', strip_directory: true) #should probably use install_headers instead

# determine the platform being built for
win32 = host_machine.system() == 'cygwin' or host_machine.system() == 'windows'
posix = not win32

subdir('src')

subdir('test')
