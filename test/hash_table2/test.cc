#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "fix_hash_table.h"

int main(int argc, char* argv[]) {


    //printf("%d\n", memcmp(NULL, NULL, 1));

    HtHandle h1;
    h1.key = (void*)"h1";
    h1.key_len = 2;

    HtHandle h2;
    h2.key = (void*)"h2";
    h2.key_len = 2;

    HtHandle h3;
    h3.key = (void*)"h3";
    h3.key_len = 2;

    HtHandle h4;
    h4.key = (void*)"h4";
    h4.key_len = 2;

    HashTable table(4, NULL);

    table.Insert(&h2);
    table.Insert(&h1);
    table.Insert(&h4);

    table.Remove((void*)"h1", 2, 0);
    table.Insert(&h3);
    //table.Insert(&h4);

    return 0;
}
