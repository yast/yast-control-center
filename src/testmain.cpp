#include <yastmodules.h>
#include <iostream.h>

#define WAS 0

int main(void)
{
	YModules y;
	const QString* s;
	y.dumpmods();
#if WAS==1
	cout << "Number of Groups: " << y.numGroups() << endl;
	for(s=y.firstGroup();s;s=y.nextGroup())
	{
		cout << "  " << *s << endl;
		const YModule* m;
		y.begin();
		do
		{
			m=y.get();
			cout << "    " << m->getName()  << endl;
		}while (y.next());
	}
#else
	int number;
	number = y.numGroups();
	cout << "Number of Groups: " << number << endl;
	for (int i=0;i< number;i++)
	{
		s=y.setGroup(i);
		cout << *s  << endl;
		const YModule* m;
		y.begin();
		do
		{
			m=y.get();
			cout << "    " << m->getName()  << endl;
		}while (y.next());
	}
#endif
	return 0;
}
