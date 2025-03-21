#pragma once

#include "Vector/Vector.hpp"

constexpr unsigned kernelSize = 64;

struct SSAO
{
	CommonUtilities::Vector4<float> Kernel[kernelSize];
	int KernelSize = kernelSize;
	CommonUtilities::Vector3<float> padding;
};