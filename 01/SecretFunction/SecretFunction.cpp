#include <iostream>
#include <string>
#include <stdexcept>
#include "IsPrime.h"

namespace
{
    using namespace std;

    enum EndCode
    {
        Success = 0,
        Error = 1,
    };
   

	bool ParseNumber(const string& str, Number& number)
	{
		try
		{
			number = stoi(str);
		}
		catch (exception& e)
		{
			return false;
		}
		
		return true;
	}

	Number ReadNumber()
	{
		string input;
		Number number;

		getline(cin, input);

		while (!ParseNumber(input, number))
		{
			cout << "Bad entry. Enter a correct number." << endl;
			getline(cin, input);
		}

		return number;
	}
}

int main()
{
	Number number = ReadNumber();
	if (IsPrime(number))
	{
		cout << "Number is prime" << endl;
	}
	else
	{
		cout << "Number is not prime" << endl;
	}

    return EndCode::Success;
}