#!/usr/bin/env python3
import csv
'''
https://www.anva.org.cn/virusAddress/listBlack
病毒引擎内置规制
#iconv -f gb2312 -t utf-8 virus_sample.csv > virus_sample_utf8.csv
#编号,MD5,恶意行为属性,操作系统
'''

'''
每一个 sig 节点对应一条恶意软件特征
a) name 属性为必填字段。表示该特征对应恶意软件的名称,用字符串表示;
b) type 属性为选填字段。恶意软件的类型,
    1 为病毒,2 为木马, 3 为蠕虫, 4 为僵尸, 5 为后门,根据需要可增加;
c) cate 属性为选填字段。描述该恶意软件的危害类型。
    1 为恶意扣费、2 位隐私窃取、3 位远程控制、4 为恶意传播、5 为资费消耗、6 为系统破坏,7 为诱骗欺诈,8 为流氓行为;
d)  md5 属性为必填字段。描述该恶意软件原生样本的128bitMD5 值;
e) upload 属性为选填字段。描述是否上传样本文件 1 需要上传, 0 不需上传 ;缺少该属性时默认为不需要上传对应样本文件;
f) 恶意软件特征码。
'''
cate_dict = {
    "恶意扣费": 1, 
    "窃取隐私": 2,
    "远程控制": 3,
    "恶意传播": 4,
    "资费消耗": 5,
    "系统破坏": 6,
    "诱骗欺诈": 7,
    "流氓行为": 8,
}

'''
<?xml version="1.0" encoding="utf-8"?>
<tzdb version="201612201817" num="8433" incremental="0">
   <sig name="A.Privacy.ssearch.eb" type="1" cate="2" md5="69004B18FDCD6031C499B33C6DBBBE77" upload="1">A.Privacy.ssearch.eb</sig>
'''
def  csvTz2Xml(csv_file, xml_file):
    cnt = 0
    line_list = []
    with open(csv_file, 'r') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            if cate_dict.get(row[2]) != None:
                row[2] = cate_dict.get(row[2])
                line_list.append(row)
    with open(xml_file, 'w') as xmlfile:
        xmlfile.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
        xmlfile.write("<tzdb version=\"001\" num=\"%d\" incremental=\"0\">\n" % len(line_list))
        for line in line_list:
            xmlfile.write("\t<sig name=\"%s\" type=\"1\" cate=\"%d\" md5=\"%s\" upload=\"1\">%s</sig>\n"%
                                        (line[0], line[2], line[1], line[0]))
        xmlfile.write("</tzdb>\n")

if __name__=='__main__':
    csvTz2Xml('inner_virus_sample_utf8.csv', 'inner_virus_sample.xml')
