#!/usr/bin/env python3
import os
import sys
import re
import time
import PyPDF2

#from collections import OrderedDict  

g_key_map = dict()
g_key_set = set()

def get_words(filename):
    try:
        fp = open(filename, 'r')
        words = fp.readlines()
    except e:
        print(e)
    finally:
        fp.close()
    for word in words:
        g_key_set.add(word.strip())

def getPageCount(pdf_file):
    pdfFileObj = open(pdf_file, 'rb')
    pdfReader = PyPDF2.PdfFileReader(pdfFileObj)
    pages = pdfReader.numPages
    return pages

def extractData(pdf_file, page):
    pdfFileObj = open(pdf_file, 'rb')
    pdfReader = PyPDF2.PdfFileReader(pdfFileObj)
    pageObj = pdfReader.getPage(page)
    data = pageObj.extractText()
    return data

def getWordCount(data):
    words = data.split()
    for w in words:
        w = w.lower()
        if w.isalpha() and w in g_key_set:
            if g_key_map.get(w) == None:
                g_key_map[w] = 1
            else:
                g_key_map[w] += 1
    return len(words)

def print_words():
    new_dict = sorted(g_key_map.items(), key=lambda x:x[1])
    with open("output.txt", "w") as f:
        for k  in new_dict:
            f.write("%-24s %-8d\n"%(k[0], k[1]))

def main():
    if len(sys.argv)!=2:
        print('command usage: python word_count.py FileName')
        exit(1)
    else:
        pdfFile = sys.argv[1]

        # check if the specified file exists or not
        try:
            if os.path.exists(pdfFile):
                print("file found!")
        except OSError as err:
            print(err.reason)
            exit(1)

        # get the word count in the pdf file
        totalWords = 0
        get_words("./enable1.txt")

        numPages = getPageCount(pdfFile)
        for i in range(numPages):
            text = extractData(pdfFile, i)
            totalWords+=getWordCount(text)
        time.sleep(1)

        print (totalWords)
        print_words()

if __name__ == '__main__':
    main()
