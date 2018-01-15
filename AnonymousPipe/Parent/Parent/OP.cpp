#include "parent_head.h"


void ParentView::CreateATTRIBUTES()  // 设置管道安全属性
{
	sa.bInheritHandle = TRUE; // TRUE为管道可以被子进程所继承  
	sa.lpSecurityDescriptor = NULL; // 默认为NULL
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
}

void ParentView::PipeCreate()  // 创建管道
{
	// 给出管道读写句柄、安全属性、及管道缓冲值
	if (!CreatePipe(&read1, &write1, &sa, 0)) //创建管道1,此处0代表管道缓冲设置为系统默认值
	{
		cout << "创建读管道失败！" << endl;
		return;
	}
	/*
	if (!SetHandleInformation(read1, HANDLE_FLAG_INHERIT, 0))
	{
	cout << "read1被继承" << endl;
	}*/
	if (!CreatePipe(&read2, &write2, &sa, 0)) //创建管道2
	{
		cout << "创建读管道失败！" << endl;
		return;
	}
	/*
	if (!SetHandleInformation(write2, HANDLE_FLAG_INHERIT, 0))
	{
	cout << "write2被继承" << endl;
	}
	*/
	TCHAR szCmdline[] = TEXT("../../child/Debug/child.exe"); // 设置子进程路径
	PROCESS_INFORMATION pi;  // 用来接收新进程的识别信息
	STARTUPINFO si;  // 用于决定新进程的主窗体如何显示
	BOOL bSuccess = FALSE;

	// 设置PROCESS_INFORMATION
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));  // 用0填充内存区域
												   // 设置STARTUPINFO
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);  // 结构大小
								  //*************** 句柄继承设置******************
								  // 创建了两个管道
								  // 管道1由父进程读，子进程写
								  // 管道2由父进程写，子进程读
	si.hStdError = write1;        // 错误输出句柄(在写句柄中写回父进程)
	si.hStdOutput = write1;       // 子进程继承管道1写句柄
	si.hStdInput = read2;		  // 子进程继承管道2读句柄
								  //*************** 句柄继承设置******************
	si.dwFlags |= STARTF_USESTDHANDLES;  // 使用hStdInput 、hStdOutput 和hStdError 成员   

										 // 创建子进程
										 // 摘自msdn:
										 // If lpApplicationName is NULL, 
										 // the first white space–delimited token of the command line specifies the module name. 
	bSuccess = CreateProcess(
		NULL,          // lpApplicationName
		szCmdline,     // command line 
					   // 以上两个字段都可以创建目标子进程
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // bInheritHandles:指示新进程是否从调用进程处继承了句柄
		CREATE_NEW_CONSOLE,             // creation flags:指定附加的、用来控制优先类和进程的创建的标志。
					   // 设置为 CREATE_NEW_CONSOLE 可显示子窗口
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&si,		   // STARTUPINFO :指向一个用于决定新进程的主窗体如何显示的STARTUPINFO结构体
		&pi			   // PROCESS_INFORMATION :指向一个用来接收新进程的识别信息的PROCESS_INFORMATION结构体
	);

	// If an error occurs, exit the application. 
	if (!bSuccess)
		cout << "创建子程序失败" << endl;
	else
	{
		// 关闭一些子进程用的句柄
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(write1);
		CloseHandle(read2);
	}

}

void ParentView::Pipe_Read()
{

	DWORD dwRead, dwWrite;
	bool rsuccess = false;
	// 获取标准输出句柄，因为想让结果在dos窗口显示
	std_write = GetStdHandle(STD_OUTPUT_HANDLE);

	// 读句柄、读取数组、缓冲区(即数组大小)、实际读取字节数、OVERLAPPED:最后一个参数此处设置为NULL
	rsuccess = ReadFile(read1, Reads, strlen(Writes), &dwRead, NULL);

	if (!rsuccess || dwRead == 0)cout << "读管道失败" << endl;
	rsuccess = WriteFile(std_write, Reads, dwRead, &dwWrite, NULL); // 写到标准输出，即dos窗口
	if (!rsuccess)cout << "输出失败" << endl;

}

void ParentView::Pipe_Write()
{
	DWORD dwWrite; // 记录DWORD数
	bool wsuccess = false;
	wsuccess = WriteFile(write2, Writes, strlen(Writes), &dwWrite, NULL);
	if (!wsuccess || dwWrite == 0)cout << "写管道失败" << endl; // 利用匿名管道的写句柄从管道中写入数据  
}