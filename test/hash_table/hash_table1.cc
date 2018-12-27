#include "hash_table1.h"
#include <stdio.h>

void HashTable::Internal() {
#if HASH_TABLE_DEBUG
  std::cout << "length_ = " << length_
            << " elems_ = " << elems_ << std::endl;
  for (int i = 0; i < length_; ++i) {
    Handle* h = list_[i];
    if (h != NULL) {
      std::cout << h->key << " => " << h ->hash << "\t";
      while ((h = h->next_hash) != NULL) {
        std::cout << h->key << " => " << h ->hash << "\t";
      }
      std::cout << std::endl;
    } else {
      std::cout << "0x0" << std::endl;
    }
  }
#endif
}

HashTable::HashTable(uint32_t cap)
  : length_(0)
  , elems_(0)
  , list_(NULL)
  , resize_(false)
{
  if (cap) {
    InitList(cap);
  }
  else {
    resize_ = true;
    Resize();
  }
}

HashTable::~HashTable()
{ 
  delete[] list_; 
}

void HashTable::InitList(uint32_t cap)
{
  uint32_t new_length = 1;
  while (new_length < cap) {
    new_length <<= 1;
  }
  Handle** new_list = new Handle* [new_length];
  memset(new_list, 0, sizeof(new_list[0]) * new_length);
  list_ = new_list;
  length_ = new_length;
}

void HashTable::Resize() {
  uint32_t new_length = 4;
  while (new_length < elems_) {
    new_length <<= 1;
  }
  Handle** new_list = new Handle* [new_length];
  memset(new_list, 0, sizeof(new_list[0]) * new_length);
  for (uint32_t i = 0; i < length_; ++i) {
    Handle* h = list_[i];
    while (h != NULL) {
      Handle* next = h->next_hash;
      Handle** ptr = &new_list[h->hash & (new_length - 1)];
      h->next_hash = *ptr;
      *ptr = h;
      h = next;
    }
  }
  delete[] list_;
  list_ = new_list;
  length_ = new_length;
#if HASH_TABLE_DEBUG
  std::cout << "for resize: ";
  Internal();
#endif
}

Handle** HashTable::FindPointer(std::string key, uint32_t hash) {
  Handle** ptr = &list_[hash & (length_ - 1)];
  while (*ptr != NULL && ((*ptr)->key != key || (*ptr)->hash != hash)) {
    ptr = &(*ptr)->next_hash;
  }
  return ptr;
}

Handle* HashTable::Remove(std::string key, uint32_t hash) {
  Handle** ptr = FindPointer(key, hash);
  Handle* result = *ptr;
  if (result != NULL) {
    *ptr = result->next_hash;
    elems_--;
  }
#if HASH_TABLE_DEBUG
  std::cout << "for Remove: ";
  Internal();
#endif
  return result;
}

Handle* HashTable::Insert(Handle* h) {
  Handle** ptr = FindPointer(h->key, h->hash);
  Handle* old = *ptr;
  h->next_hash = (old == NULL ? NULL : old->next_hash);
  *ptr = h;
  if (old == NULL) {
    elems_++;
    if (elems_ > length_) {
      if (resize_) {
        Resize();
      } else {
        printf("%s\n", "full !!!!");
      }
    }
  }

#if HASH_TABLE_DEBUG
    std::cout << "for Insert: ";
    Internal();
#endif
  return old;
}

Handle* HashTable::Lookup(std::string key, uint32_t hash) {
  return *FindPointer(key, hash);
}

