#pragma once

#include <iostream>
#include <windows.h>
#include <cstring>
#include <string>
#include <cstdio>
using namespace std;


class ParentView
{
public:
	ParentView()   // 构造函数初始化句柄
	{
		read1 = NULL;
		read2 = NULL;
		write1 = NULL;
		write2 = NULL;
		std_write = NULL;
	}
	void CreateATTRIBUTES(); // 创建安全属性
	void PipeCreate();   // 创建管道
	void Pipe_Read();    // 写管道
	void Pipe_Write();	 // 读管道
						 // 定义两个数组
	char Writes[4096];  // 写出去
	char Reads[4096];   // 读回来
private:
	HANDLE read1;    // 管道1 读句柄
	HANDLE read2;    // 管道2 读句柄
	HANDLE write1;   // 管道1 写句柄
	HANDLE write2;   // 管道2 写句柄
	HANDLE std_write;
	SECURITY_ATTRIBUTES sa; // 管道的安全属性
};

