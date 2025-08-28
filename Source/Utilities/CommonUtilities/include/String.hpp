#pragma once
#include <ostream>
#include "Hash.h"

namespace non
{
	class string
	{
	public:
		static const size_t npos = static_cast<size_t>(-1);

		string();
		~string() = default;

		// String constructor
		string(const string& aString);
		string(const char* aString);

		// Assign operator
		void operator=(const string& aString);
		void operator=(const char* aString);

		// operators
		bool operator==(const string& aString) const;
		bool operator==(const char* aString) const;

		bool operator<(const string& aString) const;

		// Substringing
		void substr(size_t aStartOffset = 0, size_t aCount = npos);

		// Changing size of string
		void allocate(size_t aNewSize);
		//void shrink_to_fit();

		// Getting data from class
		const char* c_str() const;
		const char* c_str();
		char* data() const;
		char* data();

		const size_t size() const;
		const size_t size();

		const size_t length() const;
		const size_t length();

	private:
		char* myData = static_cast<char*>(malloc(0));
		size_t mySize = 0;
		size_t myCapacity = 0;
	};

	// Streaming
	inline std::ostream& operator<<(std::ostream& os, const std::string& aString)
	{
		os << aString.c_str();
		return os;
	}

	/*struct NonStringHash
	{
		size_t operator()(const std::string& cs) const
		{
			return CommonUtilities::Hash(cs);
		}
	};*/
};