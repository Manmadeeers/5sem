#include <windows.h>
#include <iostream>

int main()
{
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (!hTimer)
		return 1;
	LARGE_INTEGER dueTime;
	dueTime.QuadPart = -3LL * 10000000LL;
	SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, FALSE);
	unsigned long long counter = 0;
	int ticks = 0;
	while (true)
	{
		counter++;

		if (WaitForSingleObject(hTimer, 0) == WAIT_OBJECT_0)
		{
			ticks++;

			if (ticks < 5)
			{
				std::cout << "Iterations after " << ticks * 3 << " seconds: " << counter << std::endl;
			}
			else
			{
				std::cout << "Final iterations after 15 seconds: " << counter << std::endl;
				break;
			}

			SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, FALSE);
		}
	}
	CloseHandle(hTimer);
	return 0;
}
