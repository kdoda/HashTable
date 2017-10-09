#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <string>

using namespace std;

template <typename HashedObj>
class HashTable
{
public:
	explicit HashTable(int = 101);
	//no need for destructor in this case
	bool insert(const HashedObj &);  //returns true if it was inserted,false if it was already in the HashTable(2 vectors)
	bool insert(HashedObj &&); 
	bool remove(const HashedObj &);
	bool contains(const HashedObj &) const;
	bool isEmpty();
	void print();
	int isPartOf(const HashTable<HashedObj>&);
	int getCurrentSize();
	int getCurrentSizeTemp();
	enum EntryType { ACTIVE, EMPTY, DELETED }; //for lazy deletion because findPos fails 

private:
	struct HashEntry
	{
		HashedObj element;
		EntryType info; //creating enum info
		HashEntry(const HashedObj & e = HashedObj{}, EntryType i = EMPTY) //EntryType by default empty
			: element{ e }, info{ i } //struct member inicialization
		{ }

		HashEntry(HashedObj && e, EntryType i = EMPTY) //move constructor
			: element{ std::move(e) }, info{ i }
		{ }
	};

	vector<HashEntry> array;       
	int currentSize;
	vector<HashEntry> arrayTemp;  //use this array to insert when the array is half full
	int currentSizeTemp;
	int copied;

	void makeEmpty(bool);         //inicializes all positions in array as empty;true to make empty the array,false the arrayTemp
	bool isActive(int) const;
	bool isActiveTemp(int) const;
	int findPos(const HashedObj &) const;  //returns the first position where it is empty,or the position where the passed element is stored,in the array
	int findPosTemp(const HashedObj &) const; //returns the first position where it is empty,or the position where the passed element is stored,in the arrayTemp
	size_t myhash(const HashedObj &,bool) const; //gets the variable and return the positions where to store it;true for the array,false the arrayTemp
	void rehash();
	int nextPrime(int);
	bool isPrime(int);
};






//active kur ka objekt ne hash table ne ate index
//deleted kur ka pasur objekt ne hash table ne ate index,por eshte fshire,na duhet per search
//empty kur nuk ka pasur objekt ne hash table ne ate index