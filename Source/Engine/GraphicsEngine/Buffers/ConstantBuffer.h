#pragma once

#include <wrl.h>

struct ID3D11Buffer;

class ConstantBuffer
{
	friend class RenderHardwareInterface;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myBuffer;
	std::string myName;
	size_t mySize = 0;

	unsigned myPipeLineStages = 0;
	unsigned mySlotIndex = 0;
};

