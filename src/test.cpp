#	include <stdio.h>

struct get_attr
{
public:
	get_attr( void * p )
		: p(p)
	{ }

public:
	template<class T>
	operator T ()
	{
		return static_cast<T>(p);
	}

protected:
	void *p;
};

template<class F>
void foo( F f, void * p )
{
	(*f)(get_attr(p));
}

void goo( int * a )
{
	printf( "%d", *a );
}

int main()
{ 
	int a = 0;
	foo( &goo, &a );

	return 0;
}