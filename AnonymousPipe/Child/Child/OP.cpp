#include "child_head.h"

void ChildView::InitPipe()
{
	read = GetStdHandle(STD_INPUT_HANDLE); // 继承句柄
	write = GetStdHandle(STD_OUTPUT_HANDLE);
	if ((read == INVALID_HANDLE_VALUE) || (write == INVALID_HANDLE_VALUE))
		cout << "继承句柄无效" << endl;
}

void ChildView::Read_pipe()
{
	DWORD dwRead;
	bool rsuccess = false;
	rsuccess = ReadFile(read, Readss, sizeof(Readss), &dwRead, NULL);	//使用标准输入句柄(即匿名管道读句柄)读取数据  
	int sum = 0;
	int k = 0;
	k = Bitadd(Readss, Writess, dwRead - 2, k);  // 减2原因  去除末尾'/','0'
	Writess[k++] = '\n';
	Writess[k++] = '\r';
	Writess[k] = '\0';
}

void ChildView::Write_pipe()
{
	DWORD dwWrite;
	bool wsuccess = false;
	wsuccess = WriteFile(write, Writess, strlen(Writess), &dwWrite, NULL);
	if (!wsuccess)cout << "输出失败" << endl;//使用标准输出句柄(即匿名管道写句柄)写入数据  
}
