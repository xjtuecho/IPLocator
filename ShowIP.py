#! /usr/bin/env python3

import tkinter as tk
from IPLocator import *

class Application(tk.Frame):
    def __init__(self, master=None):
        tk.Frame.__init__(self, master)
        self.pack()
        self.createWidgets()

    def createWidgets(self):
        self.ipLabel = tk.Label(self, text='IP Addr:')
        self.ipLabel.grid(row=0,column=0)

        self.ipStr = tk.StringVar()
        self.ipStr.set('8.8.8.8')
        self.ipEntry = tk.Entry(self, textvariable=self.ipStr)
        self.ipEntry.grid(row=0,column=1)
        
        self.locationLabel = tk.Label(self, text='Location:')
        self.locationLabel.grid(row=1,column=0)
        
        self.showIpLabel = tk.Label(self)
        self.showIpLabel.grid(row=1,column=1)

        self.rangeLabel = tk.Label(self, text='IP Range:')
        self.rangeLabel.grid(row=2,column=0)
        
        self.showRangeLabel = tk.Label(self)
        self.showRangeLabel.grid(row=2,column=1)

        self.showIpButton = tk.Button(self, text='ShowIP', command=self.ShowIP)
        self.showIpButton.grid(row=3,column=1)
        
    def ShowIP(self):
        IPL = IPLocator("QQWry.dat")
        ip = self.ipStr.get()
        ipAddr = IPL.getIpAddr( IPL.str2ip(ip) )
        ipRange = IPL.getIpRange( IPL.str2ip(ip) )
        self.showIpLabel['text'] = ipAddr
        self.showRangeLabel['text'] = ipRange

if __name__ == '__main__':
    app = Application()
    app.master.title('IPLocator')
   #app.master.geometry('200x150')
    app.mainloop()