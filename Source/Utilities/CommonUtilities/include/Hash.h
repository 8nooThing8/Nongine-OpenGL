#pragma once

#include "lookup3.h"
#include <string>

namespace CommonUtilities
{
	template <class Key>
	uint32_t Hash(const Key& aKey)
	{
		return hashlittle(&aKey, sizeof(Key), 0);
	}

	template <class Key>
	uint32_t Hash(const std::string& aKey)
	{
		return hashlittle(aKey.data(), aKey.length(), 0);
	}
}