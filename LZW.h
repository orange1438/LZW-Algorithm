#include <stdio.h>
#ifndef LZW_STRUCT_H
#define LZW_STRUCT_H
#define BITS 12
#define HASGING_SHIFT BITS-8                      // HASH 的移位数 
#define MAX_VALUE (1<<BITS)-1 
#define MAX_CODE MAX_VALUE-1                      // 最大标号 
#define TABLE_SIZE 4099                           // 散列表的长度 
typedef struct
{
	int *code;                                    // 标识 
	unsigned int *prefix;                         // 前缀 
	unsigned char *suffix;                        // 后缀 
}LZW_DATA;
//unsigned char decode_stack[TABLE_SIZE];           // 用于保存解压缩后的数据 
//LZW_DATA lzw1,*lzw;

void compress(FILE *input, FILE *output);         // 压缩函数
unsigned int hashsearch(int prefix,unsigned int suffix);    // 散列表搜索函数
void outcode(FILE *output,unsigned int code);     // 输出压缩后的字节类容


void expand(FILE *input,FILE *output);             // 解压缩函数 
unsigned int incode(FILE *input);                  // 从压缩文件中读取数据
unsigned char *decode(unsigned char *buffer,unsigned int code);     // 解码函数 
#endif