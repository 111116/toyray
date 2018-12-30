#include <cstdio>

class base
{
public:
	virtual void f();
};

class derived: public base
{
public:
	void f();
};

int main()
{
	base a = derived();
	a.f();
	base* b = new(derived);
	b->f();
}



void base::f()
{
	puts("bf");
}

void derived::f()
	{
		puts("df");
	}
