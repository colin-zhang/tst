#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <string>
#include <string.h>
#include <stdint.h>
#include <sys/queue.h>

#define HASH_TABLE_DEBUG 1

struct KeyValue
{
  void* key;
  void* value; 
};

struct Handle 
{
  std::string key;
  uint32_t hash;
  KeyValue* vk;
  Handle* next_hash;
  TAILQ_ENTRY(Handle) entry;
  Handle(std::string k, uint32_t h)
  : key(k)
  , hash(h)
  , next_hash(NULL)
  {
    memset(&entry, 0, sizeof(entry));
  }
};

struct HandleBase
{
  Handle* list_;
};

class HashTable 
{
  public:
    explicit HashTable(uint32_t cap);
    ~HashTable();
    Handle* Lookup(std::string key, uint32_t hash);
    Handle* Insert(Handle* h);
    Handle* Remove(std::string key, uint32_t hash);
    void InitList(uint32_t cap);
    void Resize();
    void Internal();
  private:
    Handle** FindPointer(std::string key, uint32_t hash);
    void (*Cmp)(void* value1, void* value2, uint32_t len);
    uint32_t length_;
    uint32_t elems_;
    Handle** list_;
    //HandleBase* base_list_;
    bool resize_;
};

#endif
