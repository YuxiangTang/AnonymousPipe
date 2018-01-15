# [C++] 匿名管道的理解与实现 

### 什么是匿名管道？

​	匿名管道用于进程之间通信，且仅限于本地父子进程之间通信，结构简单，类似于一根水管，一端进水另一端出水(单工)。相对于命名管道，其占用小实现简单，在特定情况下，比如实现两围棋引擎本地对战可以使用匿名管道。

### 怎样实现匿名管道双向通信？

​	由于匿名管道是单工的，所以为实现父子进程双向通信需要创建两根管道，并由子进程继承一根管道的读句柄和另一根管道的写句柄。

### 如何理解匿名管道的双向通信？

​	管道相当于一段内存，一个进程输入，一个进程读出。

​	在进程通信时一般会产生进程同步问题(进程同步讲解请见操作系统类书籍)：父子进程各自均具有读写功能，在管道为空时，相应读进程应该被阻塞起来，直到管道被写入为止才被唤醒。

​	这种空管道不允许读的特性应当加一个锁，但匿名管道自带了这种功能，所以不需要对读写进行限制，其能自动阻塞。

## 在VS2017下实现匿名管道

### 对几个基本点进行介绍

​	`#include <windows.h>`

​	匿名管道需要包含此头文件

​	首先我们需要了解一下最后程序实现中我想要的效果：父进程输入任意长数字(当然局限于匿名管道的最大大小4MB)通过匿名管道传给子进程，由子进程对该字符串(由于在管道中以字符流形式存在)的各位数进行加和，把这个加和的结果返回父进程。

​	在实际制作时，我将子进程这个计算函数做成动态链接库的形式进行链入。所以在实际代码中将以一行代码的形式呈现：

​	 `	int Bitadd(char *ary1, char *ary2, unsigned long len, int Lcount);`

​	其中ary1为子进程接收到的字符串、ary2为计算结果、len是接收到的字符串长度、Lcount为计算结果长度。

#### 创建管道

函数原型：

`BOOL WINAPI CreatePipe( _Out_PHANDLE hReadPipe, _Out_PHANDLE hWritePipe, _In_opt_LPSECURITY_ATTRIBUTES lpPipeAttributes, _In_DWORD nSize);`

实际调用形式：

​	`CreatePipe(&read, &write, &sa, 0)；`

其中read是读句柄，write是写句柄，sa是管道安全属性，0代表管道缓冲设置为系统默认值。

由上函数可知在创建管道之前，需要先设置管道安全属性。

#### 设置管道安全属性

对象原型：

```c++
typedef struct _SECURITY_ATTRIBUTES {

DWORD nLength; //结构体的大小，可用SIZEOF取得

LPVOID lpSecurityDescriptor; //安全描述符

BOOL bInheritHandle ;//安全描述的对象能否被新创建的进程继承

} SECURITY_ATTRIBUTES，* PSECURITY_ATTRIBUTES;

```

在程序中仅需如下设置即可：(ParentView为我创建的父进程管道类)

```c++
void ParentView::CreateATTRIBUTES()  // 设置管道安全属性
{
sa.bInheritHandle = TRUE; // TRUE为管道可以被子进程所继承  
sa.lpSecurityDescriptor = NULL; // 默认为NULL
sa.nLength = sizeof(SECURITY_ATTRIBUTES);
}
```
各参数在原型中已有很好的注释。

创建好管道后，可以考虑创建子进程，使其继承父进程的管道句柄。

#### 创建子进程

先贴代码：

```C++
	
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
si.hStdError = write1;      // 错误输出句柄(在写句柄中写回父进程)
si.hStdOutput = write1;     // 子进程继承管道1写句柄
si.hStdInput = read2;	    // 子进程继承管道2读句柄
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
		0,   		 // creation flags:指定附加的、用来控制优先类和进程的创建的标志。
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
```

​	首先设置子进程所在路径，子进程为一个exe可执行程序。然后会用到两个类型STARTUPINFO和PROCESS_INFORMATION，有兴趣的朋友可自行百度，查看两种类中的参数。

​	这里也不贴CreateProcess的函数原型了，代码块中有较好的注释。

​	其实对于管道创建和子进程创建都是一个模版框架。



> 读写函数请见github源代码



## 实现双向通信

​	在父进程中创建两个匿名管道。此时父进程共有六个句柄Read1,Write1,Read2,Write2,标准输入输出句柄。

![MARAZG~H8~1$B3@`9466V8F](C:\Users\71094\Desktop\MARAZG~H8~1$B3@`9466V8F.png)

​	由图所示，标准输入输出句柄用于在Dos窗口的输入和输出。

​	然后我们需要让创建的子进程继承Write1句柄和Read2句柄。

![`$H{BNHP6D2I`_0~BM}$5K](C:\Users\71094\Desktop\`$H{BNHP6D2I`_0~BM}]$5K.png)

#### 子进程初始化句柄代码

```C++
read = GetStdHandle(STD_INPUT_HANDLE); // 继承句柄
write = GetStdHandle(STD_OUTPUT_HANDLE);
if ((read == INVALID_HANDLE_VALUE) || (write == INVALID_HANDLE_VALUE))
	cout << "继承句柄无效" << endl;
```

​	可以看到，子进程的标准输入输出句柄已经被继承的Write1句柄和Read2句柄所覆盖。

​	因此无法实现在子进程的Dos窗口进行显示，子进程窗口将是永远黑窗，可以在父程序中注释掉子进程所继承的写句柄进行对比，并将CreateProcess函数中的一个参数设置为显示子进程窗口(注释中有)。



需要注意的坑点：

- `si.dwFlags |= STARTF_USESTDHANDLES;`  
- 若要实现双向通信，子进程Dos是黑窗。但是可以将子进程收到的结果写到文件。