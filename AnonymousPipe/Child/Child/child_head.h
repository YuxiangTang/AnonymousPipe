#pragma once

#include <windows.h>
#include <cstring>
#include <iostream>
#include <stack>
#include <cstdio>
#include "Bitadd_h.h"
using namespace std;
#define BUFSIZE 4096 

class ChildView
{
public:
	void InitPipe();
	void Read_pipe();
	void Write_pipe();

	char Writess[BUFSIZE];
	char Readss[BUFSIZE];
private:
	HANDLE read;  // 继承的读管道
	HANDLE write; // 继承的写管道
};
