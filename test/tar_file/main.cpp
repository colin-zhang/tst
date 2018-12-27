#include "tar_file.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <sys/stat.h>  
#include <unistd.h>  
#include <glob.h>

int main()
{
    TarFile tar_file;
    //tar_file.SetGzipHelp(20 << 20, 0);
    tar_file.SetGzip(0);
    tar_file.SetRootDir("colin");
    //tar_file.AddFile("1.txt", "1 hello world \n  ", strlen("1 hello world \n  "));
    // tar_file.AddFile("2.txt", "1 hello world \n  ", strlen("1 hello world \n  "));
    // tar_file.AddFile("3.txt", "1 hello world \n  ", strlen("1 hello world \n  "));
    FILE* f = fopen("gziphelper.cpp", "r");
    assert(f != NULL);
    char line[1024];
    char* end;
    int len = 0;

    tar_file.AddFileHeader("hehe/hehe.c", 7881);
    while ((end = fgets(line, sizeof(line), f)) != NULL)
    {
        len += strlen(line);
       
        tar_file.AppendFileData(line, strlen(line));
    }
     printf("len = %d \n", len);
    tar_file.FinishFileData("", 0);

    //printf("%o \n", 10);
    // printf("sizeof(UStarHeader) = %d \n", sizeof(UStarHeader));

    glob_t globbuf;
    glob("*.png", GLOB_NOSORT, NULL, &globbuf);

    for (int j = 0;  j < 2; j++)
    {
        char dst_path[128];
        for (size_t i = 0; i < globbuf.gl_pathc; i++)
        {
            //printf("globbuf.gl_pathv[%lu]= %s \n", i, (globbuf.gl_pathv[i]));
            char path[128];
            snprintf(path, sizeof(path), "%s", globbuf.gl_pathv[i]);
            tar_file.AddFile(path);
        }
        snprintf(dst_path, sizeof(dst_path), "test_%04d", j);
        tar_file.DumpFile(dst_path);
    }

    globfree(&globbuf);

    // struct stat buf;
    // stat("22", &buf);
    // printf("buf.st_mode =  %d\n", buf.st_mode);
    return 0;
}
