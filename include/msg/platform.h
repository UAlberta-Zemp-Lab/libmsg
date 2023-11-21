/* See LICENSE file for copyright and license details. */

/* Platform specific defines
 *
 * No need to include this file manually
 */
#ifndef _MSG_PLATFORM_H
#define _MSG_PLATFORM_H

#ifdef _WIN32
	#define LIBMSG_API       __declspec(dllexport)
	#define LIBMSG_SELECTANY __declspec(selectany)
#elif defined(__GNUC__) || defined(__clang__)
	#define LIBMSG_API __attribute__((visibility("default")))
	#define LIBMSG_SELECTANY
#else
	#define LIBMSG_API
	#define LIBMSG_SELECTANY
#endif

#endif /* _MSG_PLATFORM_H */
