	IPLocator 文档
		-- http://spadger.blog.com.cn
    在水木上面看到有人用Python写IP数据库的查询程序，下载其源
代码却一直调试不通，于是找来纯真IP数据库格式文档，按照文档说
明重写了其大部分代码，即IPLocator.py，实现了IP地址的查询，写
完后意犹未尽，于是分别给出了C语言和C++实现.

主要文件清单:
1.IPLocator.py
  IP数据库查询Python语言实现
2.IPLocator.h & IPLocator.c
  IP数据库查询C语言实现
3.IPLocator.hpp & IPLocator.cpp
  IP数据库查询C++语言实现
4.QQWry.dat
  珊瑚虫版IP数据库 20061105 记录总数：50424条
5.纯真IP数据库格式详解.htm
  来自LumaQQ开发者文档
6.readme.txt
  本文档

用法描述:
1.Python
    IPL = IPLocator( "QQWry.dat" )
    address = IPL.getIpAddr( IPL.str2ip("202.200.225.52") )
  具体参考IPLocator.py中的Demo
2.C语言
  int get_ip_addr(unsigned int ip, unsigned char *addr, int len, 
	unsigned int *range_start_ip, unsigned int *range_end_ip);
  ip: 待查询的IP
  addr: 存放结果的缓冲区
  len: 存放结果的缓冲区长度
  range_start_ip: IP所在IP段的起始,如果不需要设为NULL
  range_start_ip: IP所在IP段的结束,如果不需要设为NULL
3.C++
  IPLocator ipl("QQWry.dat");
  string ip("202.200.225.52");
  cout << ipl.getVersion() << endl 
       << ipl.getIpAddr(ip) << endl
       << ipl.getIpRange(ip) << endl;

    按照你的需要，将不同的文件加入你的工程即可，更多细节请直接
阅读源程序。
    本程序基于 ZLib 许可证发布，任何人可以以任何目的使用本程序，
由此造成的一切后果由使用者承担，本程序作者不承担任何形式的担保
和责任，许可证细节详见源文件顶部。如果你使用本程序，则默认接受
ZLib 许可证。
    任何意见，建议，Bug报告，Job Offer，多余的Money…… 请直接
在我的Blog留言或者 mailto:echo.xjtu@gmail.com
    Good Luck!
                                spadger <echo.xjtu@gmail.com>
                                2008-2-20 22:49 于家中