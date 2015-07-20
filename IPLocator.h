/*
http://spadger.blog.com.cn
Original code Copyright (C) 2008 spadger <echo.xjtu@gmail.com>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#ifndef _IP_LOCATOR_
#define _IP_LOCATOR_

/**
 * Define you database file name here.
 */
#define DEFAULT_IPDB_FILE  "D:\\network\\fterm\\QQWry.dat"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Input:
 *		ip: little endaian
 *		len: the size of 'addr'
 *	Output:
 *		addr: ip address
 *		range_start_ip & range_end_ip: the ip range
 *			set to NULL to omit it.
 *	return 0 for success. -1 if error.
 */
int get_ip_addr(unsigned int ip,
				unsigned char *addr,
				int len,
				unsigned int *range_start_ip,
				unsigned int *range_end_ip);
unsigned int str2ip(const char *cs);
char *ip2str(unsigned int ip, char *buf);
void get_version(unsigned char *version, int len);

#ifdef __cplusplus
extern "C" {
#endif

#endif

