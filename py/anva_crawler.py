#!/usr/bin/env python3

'''
爬取URL黑名单
https://www.anva.org.cn/virusAddress/listBlack?max=100&offset=4661
https://www.anva.org.cn/virusAddress/show/15856057
'''
import requests
import re
import sqlite3

ListBlackUrlMap = { }

def getOneListBlackPage(page_max, offset):
    main_url = "https://www.anva.org.cn"
    show_url = '/virusAddress/show/'
    r = requests.get('https://www.anva.org.cn/virusAddress/listBlack?max=%d&offset=%d'%(page_max, offset))
    if (200 == r.status_code):
        urls = re.findall(r'/virusAddress/show/\d{8,10}', r.text)
        for u in urls:
            id = u[len(show_url):]
            url = main_url + u
            ListBlackUrlMap[int(id)] = url
    else :
        print('fail')

def getBlackListDespUrl(num):
    page_max = 100
    offset = 0
    conn = sqlite3.connect('anva.db')
    cursor = conn.cursor()
    cursor.execute('create table if not exists black_url_page_list_t (id varchar(20) primary key, url varchar(64))')

    for i in range(0, num):
        getOneListBlackPage(page_max, offset + 100 * i)
        for k, v in ListBlackUrlMap.items():
            #cursor.execute('insert or replace into black_url_page_list_t (id, url) values (\'%d\', \'%s\')' % (k, v))
            cursor.execute('insert or ignore into black_url_page_list_t (id, url) values (\'%d\', \'%s\')' % (k, v))
        conn.commit()

    cursor.close()
    conn.close()

'''
<li><span>URL</span>(.*)</li>
<li><span>域名</span>(.*)</li>
<li><span>网站名称</span>(.*)</li>
<li><span>危险等级</span>(.*)</li>
'''
ReList = [
r'<li><span>编号</span>(.*)</li>',
r'<li><span>URL</span>(.*)</li>',
r'<li><span>网站名称</span>(.*)</li>',
r'<li><span>危险等级</span>(.*)</li>'
]

def getBlackListUrl():
    cnt = 0
    conn = sqlite3.connect('anva.db')
    cursor = conn.cursor()
    cursor.execute('create table if not exists black_list_url_t   \
                                (id varchar(20) primary key, \
                                sn varchar(20), \
                                url varchar(512),  \
                                domain varchar(128), level varchar(4))')
    cursor.execute('select * from black_url_page_list_t')
    values = cursor.fetchall()
    for v in values:
        r = requests.get(v[1])
        if (200 == r.status_code):
            cnt = cnt + 1
            values_list = []
            for R in ReList:
                s = re.findall(R, r.text)
                values_list.append(s[0])
            print(values_list)
            cursor.execute('insert or ignore into black_list_url_t (id, sn, url, domain, level) \
                                            values (\'%s\', \'%s\',  \'%s\', \'%s\', \'%s\')'\
                                            %(values_list[0], v[0], values_list[1], values_list[2], values_list[3]))
            if (cnt % 100 == 0):
                conn.commit()
                cnt = 0
    cursor.close()
    conn.close()


def star_crawler():
    print('star')
    #getBlackListDespUrl(5000)
    getBlackListUrl()

if __name__ == '__main__':
    star_crawler()

