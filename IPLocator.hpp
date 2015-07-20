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

#ifndef _IP_LOCATOR_H_
#define _IP_LOCATOR_H_

#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::streamsize;

class IPLocator
{
public:
	IPLocator(const string& ipdb_name);
	~IPLocator();
	string getVersion();
	string getIpAddr(const string& ip);
	string getIpRange(const string& ip);
private:
	string getIpAddr(unsigned int ip);
	string getIpRange(unsigned int ip);
	static unsigned int getIpFromString(const string& ip);
	static string getIpString(unsigned int ip);
	static unsigned int bytes2integer(unsigned char *ip, int count);
	void readFromFile(streamsize offset, unsigned char *buf,int len);
	string readStringFromFile(streamsize offset);
	string getAddr(streamsize offset);
	string getAreaAddr(streamsize offset);
	void setIpRange(unsigned int rec_no);
private:
	std::ifstream ipdb;
	unsigned int first_index;
	unsigned int last_index;
	unsigned int index_count;
	unsigned int cur_start_ip;
	unsigned int cur_start_ip_offset;
	unsigned int cur_end_ip;
};

#endif
