#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
	unsigned char buf[20] ;
	int a=12345678;
	int i;
	memset(buf,0,sizeof(char)*20 );
	memcpy(buf, &a ,4);
	for(i=0;i<10;i++){
		printf("%02X \n",buf[i]);  
	}
	return 0;
}
