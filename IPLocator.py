#! /usr/bin/env python3

""" IPLocator: locate IP in the QQWry.dat.
    Usage:
        python IPLocator.py <ip>
    Create and test under Python 2.2.3  2008-2-19
    update to Python 3.4.1 2018.1.27
    spadger@bmy <echo.xjtu@gmail.com>
"""

import socket,string,struct,sys

class IPLocator :
    def __init__( self, ipdbFile ):
        self.ipdb = open( ipdbFile, "rb" )
        str = self.ipdb.read( 8 )
        (self.firstIndex,self.lastIndex) = struct.unpack('II',str)
        self.indexCount = (self.lastIndex - self.firstIndex)//7+1
       #print("%s 纪录总数: %d 条 " % (self.getVersion(), self.indexCount) )

    def getVersion(self):
        s = self.getIpAddr(0xffffff00)
        return s

    def getAreaAddr(self,offset=0):
        if offset :
            self.ipdb.seek( offset )
        str = self.ipdb.read( 1 )
        (byte,) = struct.unpack('B',str)
        if byte == 0x01 or byte == 0x02:
            p = self.getLong3()
            if p:
                return self.getString( p )
            else:
                return ""
        else:
            self.ipdb.seek(-1,1)
            return self.getString( offset )

    def getAddr(self,offset,ip=0):
        self.ipdb.seek( offset + 4)
        countryAddr = ""
        areaAddr = ""
        str = self.ipdb.read( 1 )
        (byte,) = struct.unpack('B',str)
        if byte == 0x01:
            countryOffset = self.getLong3()
            self.ipdb.seek( countryOffset )
            str = self.ipdb.read( 1 )
            (b,) = struct.unpack('B',str)
            if b == 0x02:
                countryAddr = self.getString( self.getLong3() )
                self.ipdb.seek( countryOffset + 4 )
            else:
                countryAddr = self.getString( countryOffset )
            areaAddr = self.getAreaAddr()
        elif byte == 0x02:
            countryAddr = self.getString( self.getLong3() )
            areaAddr = self.getAreaAddr( offset + 8 )
        else:
            countryAddr = self.getString( offset + 4 )
            areaAddr = self.getAreaAddr()
        return countryAddr + " " + areaAddr

    def dump(self, first ,last ):
        if last > self.indexCount :
            last = self.indexCount
        for index in range(first,last):
            offset = self.firstIndex + index * 7
            self.ipdb.seek( offset )
            buf = self.ipdb.read( 7 )
            (ip,of1,of2) = struct.unpack("IHB",buf)
            print("%d\t%s\t%s" %(index, self.ip2str(ip), \
                self.getAddr( of1 + (of2 << 16) ) ) )

    def setIpRange(self,index):
        offset = self.firstIndex + index * 7
        self.ipdb.seek( offset )
        buf = self.ipdb.read( 7 )
        (self.curStartIp,of1,of2) = struct.unpack("IHB",buf)
        self.curEndIpOffset = of1 + (of2 << 16)
        self.ipdb.seek( self.curEndIpOffset )
        buf = self.ipdb.read( 4 )
        (self.curEndIp,) = struct.unpack("I", buf)

    def getIpAddr(self,ip):
        L = 0
        R = self.indexCount - 1
        while L < R-1:
            M = (L + R) // 2
            self.setIpRange(M)
            if ip == self.curStartIp:
                L = M
                break
            if ip > self.curStartIp:
                L = M
            else:
                R = M
        self.setIpRange( L )
        #version information,255.255.255.X,urgy but useful
        if ip&0xffffff00 == 0xffffff00:
            self.setIpRange( R )
        if self.curStartIp <= ip <= self.curEndIp:
            address = self.getAddr( self.curEndIpOffset )
        else:
            address = "未找到该IP的地址"
        return address

    def getIpRange(self,ip):
        self.getIpAddr(ip)
        range = self.ip2str(self.curStartIp) + ' - ' \
            + self.ip2str(self.curEndIp)
        return range

    def getString(self,offset = 0):
        if offset :
            self.ipdb.seek( offset )
        str = b""
        ch = self.ipdb.read( 1 )
        (byte,) = struct.unpack('B', ch)
        while byte != 0:
            str = str + ch
            ch = self.ipdb.read( 1 )
            (byte,) = struct.unpack('B', ch)
        return str.decode(encoding='gbk')  # QQWry.dat is GBK encoding.

    def ip2str(self,ip):
        return str(ip>>24)+'.'+str((ip>>16)&0xff)+'.' \
            +str((ip>>8)&0xff)+'.'+str(ip&0xff)

    def str2ip(self,s):
        (ip,) = struct.unpack('I', socket.inet_aton(s))
        return ((ip>>24)&0xff)|((ip&0xff)<<24) \
            |((ip>>8)&0xff00)|((ip&0xff00)<<8)

    def getLong3(self,offset = 0):
        if offset :
            self.ipdb.seek( offset )
        str = self.ipdb.read(3)
        (a,b) = struct.unpack('HB',str)
        return (b << 16) + a

#Demo
def main():
    IPL = IPLocator( "QQWry.dat" )
    ip = ""
    if len(sys.argv) != 2:
        print('Usage: python IPLocator.py <IP>')
        return
    else:
        ip = sys.argv[1]
    address = IPL.getIpAddr( IPL.str2ip(ip) )
    range = IPL.getIpRange( IPL.str2ip(ip) )
    print("此IP %s 属于 %s\n所在网段: %s" % (ip,address, range))


if __name__ == "__main__" :
    main()
