#include "stdafx.h"
#include "HashTable.h"

#include <iostream>
#include <iomanip>

using namespace std;

template <class HashedObj>
HashTable<HashedObj>::HashTable(int size)
	: array(nextPrime(size)), 
	  arrayTemp(0)
{
	copied = 0;
	currentSizeTemp = 0;
	makeEmpty(true);
}

template <class HashedObj> 
bool HashTable<HashedObj>::insert(const HashedObj & x)
{
	//when the first vector is filled ,continue to fill the second one and every time we 
	//fill the second one copy a fixed amount of elements form the first one

	if (currentSize < array.size() / 2)
	{
		int currentPos = findPos(x);

		if (isActive(currentPos))
			return false;

		++currentSize;
		array[currentPos].element = x;
		array[currentPos].info = ACTIVE;
	}
    else       
	{
		if (currentSizeTemp == 0) //(currentSize >= array.size() / 2 && currentSizeNew == 0)
		{
			arrayTemp.resize(nextPrime(2 * array.size()));
			makeEmpty(false);
		}

		int currentPos = findPos(x);
		int currentPosTemp = findPosTemp(x);

		if (isActive(currentPos) || isActiveTemp(currentPosTemp))
			return false;

		currentSizeTemp++;
		arrayTemp[currentPosTemp].element = x;
		arrayTemp[currentPosTemp].info = ACTIVE;

		int g = copied;
		for (copied; copied<g + 10; copied++) //copied should be 0 at the begining 
		{
			if (copied == array.size())
			{
				copied = 0;
				currentSize = currentSizeTemp;
				currentSizeTemp = 0;
				arrayTemp.swap(array); //O(1)
				arrayTemp.resize(0);
				break;
			}

			if (array[copied].info == ACTIVE) 
			{
				currentSizeTemp++;
				currentPosTemp = findPosTemp(array[copied].element);
				arrayTemp[currentPosTemp]= array[copied].element;
				arrayTemp[currentPosTemp].info = ACTIVE;
			}
		}

	}
	cout << setw(4) << getCurrentSize() << setw(12) << getCurrentSizeTemp() << endl; //to try if it works
	return true;
}

template <class HashedObj>
bool HashTable<HashedObj>::insert(HashedObj && x)
{
	// Insert x as active
	int currentPos = findPos(x);

	if (isActive(currentPos)) 
		return false;

	array[currentPos] = std::move(x);
	array[currentPos].info = ACTIVE;
	// Rehash;
	if (++currentSize > array.size() / 2)
		rehash();
	return true;
}

template <class HashedObj>
bool HashTable<HashedObj>::remove(const HashedObj & x)
{
	//if it is the case that we are already coping to the next table it might be in both the tables so we should delete in in both of them.
	int currentPos = findPos(x);
	
	if (isActive(currentPos)) //if it is in the first 
	{
		currentSize--;
		array[currentPos].info = DELETED;
		if (currentSizeTemp!=0)
		{
			int currentPosTemp = findPosTemp(x);
			if (isActiveTemp(currentPosTemp)) //if it is in the second also 
			{
				currentSizeTemp--;
				arrayTemp[currentPosTemp].info = DELETED;
			}
		}
		return true;
	}
	
	return false;
}

template <class HashedObj>
bool HashTable<HashedObj>::contains(const HashedObj & x) const
{
	return  isActive(findPos(x)) || (currentSizeTemp != 0 && isActiveTemp(findPosTemp(x)));
}

template <class HashedObj>
bool HashTable<HashedObj>::isEmpty()
{
	return currentSize == 0;
}

template <class HashedObj>
void HashTable<HashedObj>::print()
{

	for (int i = 0;i<array.size();i++)
	{
		if (array[i].info == ACTIVE && (arrayTemp.size() == 0 || !isActiveTemp(findPosTemp(array[i].element))))
			cout << array[i].element << endl;
	}

	for (int i = 0;i<arrayTemp.size();i++)
	{
		if (arrayTemp[i].info == ACTIVE)
			cout << array[i].element << endl;
	}
}

template <class HashedObj>
int HashTable<HashedObj>::isPartOf(const HashTable<HashedObj>& other)
{
	if (other == *this)
		return 0; //otherHashT==thisHashT

	bool flag = true;

	for (auto & entry : this->array)
	{
		if (entry.info == ACTIVE && !other.contains(entry.element)) 
		{
			flag = false;
			break;
		}
	}
	if (flag && arrayTemp.size() != 0) //because we have the second array
	{
		for (auto & entry : this->arrayTemp)
		{
			if (entry.info == ACTIVE && !other.contains(entry.element)) 
			{
				flag = false;
				break;
			}
		}
	}
	if (flag == false)
		return -1; //otherHashT!=thisHashT

	if (flag == true && this->currentSize == other.currentSize)
		return 0; //otherHashT == thisHashT
	if (flag == true && this->currentSize != other.currentSize) //you can omit it because is the only case left 
		return 1; //otherHashT is part of this HashT
}

template <class HashedObj>
int HashTable<HashedObj>::getCurrentSize()
{
	return currentSize;
}

template <class HashedObj>
int HashTable<HashedObj>::getCurrentSizeTemp()
{
	return currentSizeTemp;
}

template <class HashedObj>
void HashTable<HashedObj>::makeEmpty(bool whichArray)
{
	if (whichArray)
	{
		currentSize = 0;
		for (auto & entry : array)
			entry.info = EMPTY;
	}
	else
	{
		currentSizeTemp = 0;
		for (auto & entry : arrayTemp)
			entry.info = EMPTY;
	}
}

template <class HashedObj>
bool HashTable<HashedObj>::isActive(int currentPos) const
{
	return (array[currentPos].info == ACTIVE); 
}

template <class HashedObj>
bool HashTable<HashedObj>::isActiveTemp(int currentPos) const
{
	return (arrayTemp[currentPos].info == ACTIVE); 
}

template <class HashedObj>
int HashTable<HashedObj>::findPos(const HashedObj & x) const 
{
	int offset = 1;
	int currentPos = myhash(x,true); //the position where it should be stored 

	while (array[currentPos].info != EMPTY && array[currentPos].element != x) 
	{
		currentPos += offset; // Compute ith probe
		offset += 2;  //increases by 2 on successive probes

		if (currentPos >= array.size())
			currentPos -= array.size();
	}
	return currentPos;
}

template <class HashedObj>
int HashTable<HashedObj>::findPosTemp(const HashedObj & x) const 
{
	int offset = 1;
	int currentPosTemp = myhash(x,false); 

	while (arrayTemp[currentPosTemp].info != EMPTY && arrayTemp[currentPosTemp].element != x)
	{
		currentPosTemp += offset; // Compute ith probe
		offset += 2;

		if (currentPosTemp >= arrayTemp.size())
			currentPosTemp -= arrayTemp.size();
	}
	return currentPosTemp;
} 

template <class HashedObj>
size_t HashTable<HashedObj>::myhash(const HashedObj & x,bool whichArray) const 
{ 
	static hash<HashedObj> hf;
	int hashIndex = hf(x);
	return ( whichArray ? hashIndex % array.size() : hashIndex % arrayTemp.size()) ;
}

template <class HashedObj>
void HashTable<HashedObj>::rehash()
{
	vector<HashEntry> oldArray = array;
	// Create new double-sized, empty table
	array.resize(nextPrime(2 * oldArray.size()));

	for (auto & entry : array)
		entry.info = EMPTY;

	// Copy table over
	currentSize = 0;
	for (auto & entry : oldArray)
		if (entry.info == ACTIVE)
			insert(std::move(entry.element));
}

template <class HashedObj>
bool HashTable<HashedObj>::isPrime(int n)
{
	if (n == 2 || n == 3)
		return true;

	if (n == 1 || n % 2 == 0)
		return false;

	for (int i = 3; i * i <= n; i += 2)
		if (n % i == 0)
			return false;

	return true;
}

template <class HashedObj>
int HashTable<HashedObj>::nextPrime(int n)
{
	if (n % 2 == 0)
		++n;

	for (; !isPrime(n); n += 2)
		;

	return n;
}