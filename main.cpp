#include "LZW.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc,char *argv[])
{
	FILE *fp1,*fp2;
	int flag=0;
	char *op;
	if(argc!=4)flag=1;
	else
	{
		op=argv[1];
		if((strcmp(op,"-z")!=0) && strcmp(op,"-e")!=0)flag=1;
	}
	if(flag)
	{
		printf("使用方法：command -z/-e source dest\n");
		exit(1);
	}
	if((fp1=fopen(argv[2],"rb"))==NULL)
	{
		printf("不能打开源文件！！！\n");
		exit(1);
	}
	if((fp2=fopen(argv[3],"wb"))==NULL)
	{
		printf("不能创建目标文件！！！\n");
		exit(1);
	}
	if(strcmp(op,"-z")==0)
		compress(fp1,fp2);     // 调用压缩函数 
	else
		expand(fp1,fp2);       // 解压缩函数
		
    fclose(fp1);
    fclose(fp2);
    return 0;
}