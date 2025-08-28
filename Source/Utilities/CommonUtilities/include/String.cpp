#include "String.hpp"

#include <utility>

using namespace non;

#include <cstring>

string::string()
{
	
}

string::string(const string& aString)
{
	operator=(aString);
}
string::string(const char* aString)
{
	operator=(aString);
}

void string::operator=(const string& aString)
{
	if (!myData || aString.mySize > myCapacity)
	{
		allocate(mySize + 16);
	}

	mySize = aString.mySize;

	std::memcpy(myData, aString.myData, aString.mySize + 1);
}
void string::operator=(const char* aString)
{
	size_t strLength = strlen(aString);

	if (!myData || strLength > myCapacity)
	{
		allocate(strLength + 16);
	}

	mySize = strLength;

	

	std::memcpy(myData, aString, strLength + 1);
}

bool string::operator==(const string& aString) const
{
	return std::memcmp(myData, aString.myData, mySize) > 0;
}
bool string::operator==(const char* aString) const
{
	return std::memcmp(myData, aString, mySize) > 0;
}
bool string::operator<(const string& other) const
{
	/*int cmp = std::memcmp(myData, other.myData, std::min(mySize, other.mySize));
	if (cmp < 0) return true;
	if (cmp > 0) return false;

	return mySize < other.mySize;*/

	return true;
}

void string::substr(size_t aStartOffset, size_t aCount)
{
	mySize = (aCount > mySize) ? mySize : aCount;
	std::memcpy(myData, myData + aStartOffset, mySize);
	std::memcpy(myData + mySize, "\0", 1);
}

void string::allocate(size_t aNewSize)
{
	void* where;

	if (mySize)
	{
		where = realloc(myData, aNewSize);
		myData = static_cast<char*>(where);
	}
	else
	{
		where = malloc(aNewSize);
		myData = static_cast<char*>(where);
		std::memcpy(myData, "\0", 1);
	}

	myCapacity = aNewSize;
}

const char* string::c_str()
{
	return myData;
}
const char* string::c_str() const
{
	return myData;
}
char* string::data()
{
	return myData;
}
char* string::data() const
{
	return myData;
}

const size_t string::size() const
{
	return mySize;
}
const size_t string::size()
{
	return mySize;
}

const size_t string::length() const
{
	return mySize;
}
const size_t string::length()
{
	return mySize;
}
