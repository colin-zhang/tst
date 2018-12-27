#include <iostream>
#include "hash_table1.h"
#include <stdlib.h>
#include <stdio.h>

void print(Handle*);

int main(int argc, char* argv[]) {
  
  Handle ha("sdau", 2);
  Handle hb("whu", 2);
  Handle hc("pku", 2);
  Handle hd("pku1", 2);
  Handle he("pku2", 2);
  Handle hf("pku3", 2);

  HashTable table(0);
  table.Insert(&ha);
  table.Insert(&ha);
  table.Insert(&hb);
  table.Insert(&hc);
  table.Insert(&hd);
  table.Insert(&he);
  table.Insert(&hf);

  table.Remove(hb.key, hb.hash);


  Handle* h = table.Lookup(hb.key, hb.hash);
  h = table.Lookup(hc.key, hc.hash);
  print(h);

  return 0;
  
  std::string vv("weibo");
  char buf[10];
  for (int i = 0; i < 9 - 4; ++i) {
    sprintf(buf, "%d", i);
    Handle* hi = new Handle(vv + buf, ((i + 1) % 5 == 0 ? 2 : i));
    table.Insert(hi);
  }

  return 0;
}

void print(Handle* h) {
  std::cout << "for Lookup: " << std::endl;
  if (h) {
    std::cout << "found: " << h->key << " => " << h->hash << std::endl;
    return;
  }
  std::cout << "not found ~" << std::endl;
}


