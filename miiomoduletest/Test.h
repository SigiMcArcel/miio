#pragma once
#include <algorithm>
#include <utility>

class Test
{
private:
	int _I;
public:
	Test() = default;
	Test& operator=(const Test& other)
	{
		_I = 123;
		return *this;
	}

	Test(int i)
		:_I(i)
	{

	}
	Test(const Test& other)
		:_I(other._I)
	{

	}
	void set(int i)
	{
		_I = i;
	}
};

