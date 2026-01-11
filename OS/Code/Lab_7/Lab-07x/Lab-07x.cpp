#include <windows.h>
#include <iostream>

bool IsPrime(unsigned long long n)
{
	if (n < 2) return false;

	for (unsigned long long i = 2; i * i <= n; ++i) if (n % i == 0) return false;
	return true;
}
int main(int argc, char* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	if (argc < 2)
	{
		std::cout << "Usage: Lab-07x.exe <seconds>\n";
		return 1;
	}
	unsigned long long workSeconds = _strtoui64(argv[1], nullptr, 10);

	ULONGLONG startTime = 0;
	QueryUnbiasedInterruptTime(&startTime);

	unsigned long long primeCount = 0;
	unsigned long long number = 2;

	while (true)
	{
		if (IsPrime(number))
		{
			std::cout << ++primeCount << ") " << number << std::endl;
		}

		number++;

		ULONGLONG currentTime = 0;
		QueryUnbiasedInterruptTime(&currentTime);

		if ((currentTime - startTime) / 10000000ULL >= workSeconds)break;
	}
	ULONGLONG endTime = 0;
	QueryUnbiasedInterruptTime(&endTime);
	double elapsed = (double)(endTime - startTime) / 10000000.0;
	std::cout << "worked " << elapsed << " seconds\nAmount of numbers: " << primeCount << std::endl;
	return 0;
}
