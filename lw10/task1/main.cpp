#include <iostream>
#include <vector>
#include "quadricsolver.h"

// разобратья чем отличается компоновка STATIC от динмаической библиотеки. Как компьютер понимает что ф-ию quadricsolver нужно загрузить из dll
// static: -> .exe на этапе линковки
// dynamic -> .dll -> .exe на этапе запуска9

int main()
{
	std::cout << "Enter coefficients: " << std::endl;
	double a, b, c;
	std::cin >> a >> b >> c;

	double root1, root2;
	int rootsCount = SolveQuadratic(a, b, c, root1, root2);
	if (rootsCount == 1)
	{
		std::cout << "Only root = " << root1 << std::endl;
	}
	else if (rootsCount == 2)
	{
		std::cout << "Roots: " << root1 << ", " << root2 << std::endl;
	}
	else
	{
		std::cout << "No solutions" << std::endl;
	}

	return EXIT_SUCCESS;
}
