#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
 
from xml.dom.minidom import parse
import xml.dom.minidom
 
# 使用minidom解析器打开 XML 文档
DOMTree = xml.dom.minidom.parse("tz.xml")
tzdb = DOMTree.documentElement
if tzdb.hasAttribute("num"):
   num = tzdb.getAttribute("num")

sigs = tzdb.getElementsByTagName("sig")
with open("md5.txt", 'w') as md5file:
   for sig in sigs:
      md5 = sig.getAttribute("md5")
      print(md5)
      md5file.write("%s\n"%md5)

