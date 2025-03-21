#pragma once

#include <vector>

#include "Hash.h"

namespace CommonUtilities
{
	// State is used in case i wanna expand in the future
	enum State
	{
		Empty = 1 << 0,
		InUse = 1 << 1,
		Removed = 1 << 2
	};

	template <class Key, class Value>
	class HashMap
	{
		struct Entry
		{
			Entry()
			{
				state = Empty;
			}

			Key key;

			State state;
			Value* value;
		};

	public:
		HashMap(int aCapacity)
		{
			myEntries.reserve(sizeof(Value) * aCapacity);

			for (int i = 0; i < aCapacity; i++)
			{
				myEntries.push_back(Entry());
			}

			myTotalSize = 0;
		}

		~HashMap()
		{
			for (int i = 0; i < myEntries.size(); i++)
			{

			}
		}

		bool Insert(const Key& aKey, const Value& aValue)
		{
			if (myTotalSize >= myEntries.size())
			{
				return false;
			}

			uint32_t decimal = CommonUtilities::Hash<Key>(aKey);

			int index = decimal % myEntries.size();

			myEntries[index].key = aKey;
			myEntries[index].value = new Value(aValue);
			myEntries[index].state = InUse;

			++myTotalSize;

			return true;
		}

		bool Remove(const Key& aKey)
		{
			if (!myTotalSize)
			{
				return myTotalSize;
			}

			uint32_t decimal = CommonUtilities::Hash<Key>(aKey);

			int index = decimal % myEntries.size();

			if (!(aKey < myEntries[index].key && myEntries[index].key < aKey))
			{
				return false;
			}

			myEntries[index].key = Key();
			myEntries[index].value = nullptr;
			myEntries[index].state = Removed;

			--myTotalSize;

			return true;
		}

		const Value* Get(const Key& aKey) const
		{
			uint32_t decimal = CommonUtilities::Hash<Key>(aKey);

			int index = decimal % myEntries.size();

			return myEntries[index].value;
		}

		//Som ovan, men returnerar en icke-const-pekare.
		Value* Get(const Key& aKey)
		{
			uint32_t decimal = CommonUtilities::Hash<Key>(aKey);

			int index = decimal % myEntries.size();

			return myEntries[index].value;
		}

	private:
		std::vector<Entry> myEntries;

		int myTotalSize;
	};
}