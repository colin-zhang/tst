#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "container/url_cache.h"

int main(int argc, char* argv[]) 
{
    char line[2048];
    std::vector<std::string> url_list;
    std::string url;

    FILE* f = fopen("url_list.txt", "r");
    while (fgets(line, 2048, f))
    {
        url = std::string(line);

        if (UrlCacheTestInstance()->FindUrl(url.c_str(), url.size(), 0)) 
        {
            //printf("find : %s\n", url.c_str());
        } 
        else
        {
            if (url.find("r") != std::string::npos) {
                int ret = UrlCacheTestInstance()->InsertUrl(url.c_str(), url.size(), 0);
                if (ret == -1) {
                    UrlCacheTestInstance()->ClearAll();
                }
            }
        }
    }
     fclose(f);

    return 0;
}