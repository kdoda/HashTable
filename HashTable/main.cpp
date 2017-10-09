// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HashTable.h"
#include "HashTable.cpp"

int main()
{
	HashTable<int> h;

	cout << "Array:" << " || ArrayTemp:" << endl;
	for (int i = 0;i<100;i++)
		h.insert(i);

	if (h.remove(50))
	{
		cout << "After remove" << endl
			 << "Array:" << " || ArrayTemp:" << endl
			 << h.getCurrentSize() << "\t  " << h.getCurrentSizeTemp() << endl;
	}
	
	return 0;
}

