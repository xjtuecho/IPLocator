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

#include "IPLocator.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define REDIRECT_MODE1 0x01
#define REDIRECT_MODE2 0x02

typedef struct {
	FILE *fp;
	unsigned int first_index;
	unsigned int last_index;
	unsigned int cur_start_ip;
	unsigned int cur_end_ip;
	unsigned int cur_end_ip_offset;
}	ip_locator;

unsigned int str2ip(const char *cs)
{
    char *result = NULL;
	unsigned int ret=0;
	char *s=strdup(cs);
    result = strtok( s, "." );
    while( result ) {
		ret <<= 8;
		ret |= (unsigned int)atoi(result);
		result = strtok( NULL, "." );
    }
	free(s);
	return ret;
}

/**
 * Make sure 'buf' is big enough....
 */
char *ip2str(unsigned int ip, char *buf)
{
	sprintf(buf,"%d.%d.%d.%d",ip>>24,(ip>>16)&0xff,(ip>>8)&0xff,ip&0xff );
	return buf;
}

/**
 *	convert 1-4 bytes to an integer, little-endian.
 */
static unsigned int bytes2integer(const unsigned char *ip, int count)
{
	int i;
	unsigned int ret;

	if(count < 1 || count > 4)
		return 0;
	ret = ip[0];
	for (i = 0; i < count; i++)
		ret |= ((unsigned int)ip[i])<<(8*i);
	return ret;
}

/**
 *	read 'len' of bytes to 'buf', from 'fp' at 'offset'
 */
static void read_from_file(FILE *fp, unsigned int offset, unsigned char *buf, int len)
{
	if(fseek(fp, offset, SEEK_SET)) {
		fprintf(stderr, "fseek() error in read_from_file()...\n");
		memset(buf, 0, len);
		return;
	}
	if(!fread(buf, 1, len, fp)) {
		fprintf(stderr, "Fail read %d bytes in read_from_file()...", len);
		memset(buf, 0, len);
		return;
	}
}


/**
 *	read the string from fp at offset, allocate memory on the heap
 */
static char *get_string(FILE *fp, unsigned int offset)
{
	/*Maybe this is not big enough.....*/
	unsigned char buf[256];
	read_from_file(fp,offset,buf,256);
	return strdup(buf);
}

static char *get_area_addr(FILE *fp,unsigned int offset)
{
	unsigned char byte;
	unsigned char buf[4];
	unsigned int p=0;
	unsigned char *ret=NULL;

	read_from_file(fp,offset,buf,4);
	byte = buf[0];
	if(REDIRECT_MODE1 == byte || REDIRECT_MODE2 == byte) {
		p = bytes2integer(buf+1,3);
		if(p)
			ret = get_string(fp,p);
		else
			ret = "";
	} else
		ret = get_string(fp,offset);
	return ret;
}

static void get_addr(FILE *fp, unsigned int offset, unsigned char *addr, unsigned int len)
{
	unsigned char byte;
	unsigned char buf[4];
	unsigned int country_offset;
	char *country_addr=NULL,*area_addr=NULL;

	read_from_file(fp, offset+4,buf,4);
	byte=buf[0];
	if(REDIRECT_MODE1 == byte) {
		country_offset = bytes2integer(buf+1,3);
		read_from_file(fp,country_offset,buf,4);
		byte = buf[0];
		if(REDIRECT_MODE2 == byte){
			country_addr = get_string(fp,bytes2integer(buf+1,3));
			area_addr = get_area_addr(fp,country_offset+4);
		} else {
			country_addr = get_string(fp,country_offset);
			area_addr = get_area_addr(fp,country_offset+strlen(country_addr)+1);
		}
	} else if(REDIRECT_MODE2 == byte) {
		read_from_file(fp,offset+4+1,buf,3);
		country_offset = bytes2integer(buf,3);
		country_addr = get_string(fp,country_offset);
		area_addr = get_area_addr(fp,offset+4+4);
	} else {
		country_addr = get_string(fp,offset+4);
		area_addr = get_area_addr(fp,offset+4+strlen(country_addr)+1);
	}
	if(len > strlen(country_addr)+strlen(area_addr)+1){
		strcpy(addr,country_addr);
		strcat(addr," ");
		strcat(addr,area_addr);
	}
	/*free the memory allocated by get_string()*/
	free(country_addr);
	free(area_addr);
}

static void set_ip_range(int rec_no, ip_locator *f)
{
	unsigned char buf[7];
	unsigned int offset=0;

	if(f == NULL)
		return;
	offset = f->first_index + rec_no * 7;
	read_from_file(f->fp, offset, buf, 7);
	f->cur_start_ip = bytes2integer(buf, 4);
	f->cur_end_ip_offset = bytes2integer(buf + 4, 3);
	read_from_file(f->fp, f->cur_end_ip_offset, buf, 4);
	f->cur_end_ip = bytes2integer(buf, 4);
}

/**
 *	get IP location, return -1 if error
 */
int get_ip_addr(unsigned int ip, unsigned char *addr, int len,
	unsigned int *range_start_ip,unsigned int *range_end_ip)
{
	ip_locator f;
	unsigned int M, L, R, record_count;
	unsigned char buf[4];

	if ((f.fp = fopen(DEFAULT_IPDB_FILE,"rb")) == NULL) {
		fprintf(stderr, "Unable to open file: %s\n",
			DEFAULT_IPDB_FILE);
		return -1;
	}
	/* read the file header, just 8 bytes */
	read_from_file(f.fp, 0, buf, 4);
	f.first_index = bytes2integer(buf, 4);
	read_from_file(f.fp, 4, buf, 4);
	f.last_index = bytes2integer(buf, 4);
	record_count = (f.last_index-f.first_index)/7+1;
	/* search for right range */
	L = 0;
	R = record_count - 1;
	while (L < R-1) {
		M = (L + R) / 2;
		set_ip_range(M, &f);
		if (ip == f.cur_start_ip) {
			L = M;
			break;
		}
		if (ip > f.cur_start_ip)
			L = M;
		else
			R = M;
	}
	set_ip_range(L, &f);
	/* version infomation, the last item */
	if((ip & 0xffffff00) == 0xffffff00)
		set_ip_range(R, &f);
	if (f.cur_start_ip <= ip && ip <= f.cur_end_ip)
		get_addr(f.fp, f.cur_end_ip_offset, addr, len);
	else
		*addr = '\0';
	/**/
	if(range_start_ip !=NULL)
		*range_start_ip = f.cur_start_ip;
	if(range_end_ip != NULL)
		*range_end_ip = f.cur_end_ip;
	fclose(f.fp);
	return 0;
}

void get_version(unsigned char *version, int len)
{
	/* the last item is the version information. */
	get_ip_addr(0xffffff00,version,len,NULL,NULL);
}

/**
 * Demo: a command tool to query ip address
 */

#if 1
int main(int argc, char **argv)
{
	unsigned int start_ip,end_ip;
	unsigned char addr[256];

	if(argc != 2){
		printf("\nUsage: ShowIP <IP Address>\n");
		return -1;
	}
	get_version(addr, 256);
	printf("\n%s\n",addr);
	get_ip_addr(str2ip(argv[1]), addr, 256, &start_ip, &end_ip);
	printf("此IP %s 属于 %s\n", argv[1], addr);
	ip2str(start_ip,addr);
	printf("所在网段: %s - ", addr);
	ip2str(end_ip,addr);
	printf("%s\n", addr);
	return 0;
}
#endif
