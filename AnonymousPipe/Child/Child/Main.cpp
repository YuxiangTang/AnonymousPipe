#include "child_head.h"

int main(void)
{
	ChildView cv;
	cv.InitPipe();
	printf("\n*******\nabcd\n");    // 用于理解匿名管道
	int n = 10;
	while (n--)
	{
		cv.Read_pipe();

		cv.Write_pipe();
	}
	system("pause");
	return 0;
}