#include <windows.h>
#include <iostream>

int main()
{
	ULONGLONG startTime = GetTickCount64();
	ULONGLONG currentTime;

	unsigned long long counter = 0;

	bool printed5 = false;
	bool printed10 = false;

	while (true)
	{
		counter++;

		currentTime = GetTickCount64();
		ULONGLONG elapsed = currentTime - startTime;

		if (!printed5 && elapsed >= 5000)
		{
			std::cout << "Iterations after 5 seconds: " << counter << std::endl;
			printed5 = true;
		}

		if (!printed10 && elapsed >= 10000)
		{
			std::cout << "Iterations after 10 seconds: " << counter << std::endl;
			printed10 = true;
		}

		if (elapsed >= 15000)
		{
			std::cout << "Final iterations after 15 seconds: " << counter << std::endl;
			break;
		}
	}

	return 0;
}
