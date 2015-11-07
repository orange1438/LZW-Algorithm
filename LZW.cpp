#include <stdio.h>
#include <stdlib.h>
#include "LZW.h"

LZW_DATA lzw1,*lzw;
unsigned char decode_stack[TABLE_SIZE];           // 用于保存解压缩后的数据 
/////////////      压缩函数         ///////////////// 
void compress(FILE *input,FILE *output)
{
	unsigned int curr_code;
	unsigned int suffix;                 // 后缀字符
	unsigned int prefix;                 // 前缀字符
	unsigned int index;
	int i;
	
	if(!(lzw1.code=(int*)malloc(TABLE_SIZE*sizeof(unsigned int))))
	{
		printf("内存分配失败！\n");
		exit(0);
	}
	if(!(lzw1.prefix=(unsigned int*)malloc(TABLE_SIZE*sizeof(unsigned int))))
	{
		printf("内存分配失败！\n");
		exit(0);
	}
	if(!(lzw1.suffix=(unsigned char*)malloc(TABLE_SIZE*sizeof(unsigned char))))
	{
		printf("内存分配失败！\n");
		exit(0);
	}
	
	lzw=&lzw1;
	curr_code=258;                      // 编译表中的字符串从258开始的
	for(i=0;i<TABLE_SIZE;i++)           // 初始化标号数组 
	    lzw->code[i]=-1; 
    i=0;
    printf("\n开始压缩.");
    prefix=getc(input);                 // 从文件中读取一个字节
	while((suffix=getc(input)) != (unsigned)EOF)
	{
		if(++i==1000)
		{
			i=0;
			printf(".");
		}
		index = hashsearch(prefix,suffix);   // 在散列表中查找，并返回索引号
		if(lzw->code[index]!=-1)             // 若该标号存在 
		    prefix = lzw->code[index];       // 使用该标号作为前缀 
		else                                 // 若不存在 
		{
			if(curr_code<=(MAX_CODE))        // 标号未超过最大标号 
			{
				lzw->code[index]=curr_code++; // 增加一个标号 
				lzw->prefix[index]=prefix;    // 保存前缀 
				lzw->suffix[index]=suffix;    // 保存后缀 
			}
			outcode(output,prefix);           // 输出前缀字符内容 
			prefix=suffix;                    // 将后缀作为前缀，准备下次循环 
		}
	}
	outcode(output,prefix);                   // 输出前缀 
	outcode(output,(MAX_VALUE));              // 输出结束标志 
	outcode(output,0);
	
	free(lzw->code);
	free(lzw->prefix);
	free(lzw->suffix);
}


/////////////      散列表搜索函数         ///////////////// 
unsigned int hashsearch(int prefix,unsigned int suffix)
{
	int index;
	int offset;
	index=(suffix << (HASGING_SHIFT))^prefix;        // 构造HASH地址
	if(index==0)
	    offset=-1;
	else
	    offset=TABLE_SIZE-index;
	while(1)
	{
		if(lzw->code[index]==-1)return (index);    // 找到一个空表项 
		if(lzw->prefix[index]==prefix && lzw->suffix[index]==suffix)  // 找到目标地址 
		    return (index);
        index-=offset;                            //  处理冲突,调整hash地址 
        if(index<0)
            index+=TABLE_SIZE;                     // 调整HASH地址 
	}
}

/////////////      输出压缩后字符内容         ///////////////// 
void outcode(FILE *output,unsigned int code)
{
	static int ob=0;                          // 静态变量，保留已输出数据的二进制位数 
	static unsigned long obb=0L;              // 静态变量，保留需输出数据的二进制
	obb |=(unsigned long)code <<(32-BITS-ob);    // 进行移位合并
	ob+=BITS;
	while(ob>=8)                              // 达到一个字节(8位)，则输出 
	{
		putc(obb>>24,output);                 // 右移24位，使低8位为输出数据 
		obb<<=8;                              // 左移8位，去掉已输入的一个字节数据 
		ob-=8;                                // 减输出的8位，保留剩余的未输出的位数 
	}
	return;
}


/////////////      解压缩函数         ///////////////// 
void expand(FILE*input,FILE*output)
{
	unsigned int curr_code;
	unsigned int suffix;
	unsigned int prefix;
	int ch;
	int i;
	unsigned char *ps;
	unsigned char *decode(unsigned char *buffer ,unsigned int code);
	
	if(!(lzw1.code=(int*)malloc(TABLE_SIZE*sizeof(unsigned int))))
	{
		printf("内存分配失败！\n");
		exit(0);
	}
	if(!(lzw1.prefix=(unsigned int*)malloc(TABLE_SIZE*sizeof(unsigned int))))
	{
		printf("内存分配失败！\n");
		exit(0);
	}
	if(!(lzw1.suffix=(unsigned char*)malloc(TABLE_SIZE*sizeof(unsigned char))))
	{
		printf("内存分配失败！\n");
		exit(0);
	}
	lzw=&lzw1;
	curr_code=258;
	i=0;
	printf("\n解压缩.");
	prefix = incode(input);
	ch=prefix;
	putc(prefix,output);
	while((suffix=incode(input))!=(MAX_VALUE))
	{
		if(++i==1000)
		{
		    i=0;
			printf(".");
		}
		if(suffix>=curr_code)
		{
			*decode_stack=ch;
			ps=decode(decode_stack+1,prefix);
		}
		else
		    ps=decode(decode_stack,suffix);
        ch=*ps;
        while(ps>=decode_stack)
            putc(*ps--,output);
        if(curr_code<=MAX_CODE)
        {
        	lzw->prefix[curr_code]=prefix;
        	lzw->suffix[curr_code]=ch;
        	curr_code++;
        }
        prefix=suffix;
	}
	free(lzw->code);
	free(lzw->prefix);
	free(lzw->suffix);
}


/////////////      从压缩文件中读取数据         ///////////////// 
unsigned int incode(FILE*input)
{
	unsigned int ret;
	static int ib=0;
	static unsigned long ibb=0L;
	while(ib<=24)
	{
		ibb|=(unsigned long)getc(input)<<(24-ib);
		ib+=8;
	}
	ret=ibb>>(32-BITS);
	ibb<<=BITS;
	ib-=BITS;
	return (ret);
}

/////////////       解码函数         ///////////////// 
unsigned char *decode(unsigned char*buffer,unsigned int code)
{
	int i=0;
	while(code>257)
	{
		*buffer++ =lzw->suffix[code];
		code=lzw->prefix[code];
		if(i++>=TABLE_SIZE)
		{
			printf("内存溢出\n");
			exit(1);
		}
	}
	*buffer=code;
	return (buffer);
}