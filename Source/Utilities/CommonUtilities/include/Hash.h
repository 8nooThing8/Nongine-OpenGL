#pragma once

#include "lookup3.h"
#include "String.hpp"

namespace CommonUtilities
{
	template <class Key>
	uint32_t Hash(const Key& aKey)
	{
		return hashlittle(&aKey, sizeof(Key), 0);
	}
}