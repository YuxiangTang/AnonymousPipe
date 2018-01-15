#include "parent_head.h"

int main()
{
	string input;
	ParentView pv;
	pv.CreateATTRIBUTES();
	pv.PipeCreate();
	int n = 10;
	int j = 0;
	while (n--)
	{
		cin >> input;
		const char* p = input.data();
		strcpy(pv.Writes, p);
		const char *q = "/0";
		strcat(pv.Writes, q);
		pv.Pipe_Write();
		pv.Pipe_Read();
	}

	//Sleep(3000);

	system("pause");
	return 0;
}