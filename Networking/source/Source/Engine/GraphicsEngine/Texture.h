#pragma once
#include <wrl.h>
#include <array>

#include <Vector/Vector.hpp>

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
class RenderHardwareInterface;

class Texture
{
public:
	friend RenderHardwareInterface;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> myRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDSV;

private:


	std::array<float, 4> myClearColor = { 0.1137f, 0.3647f, 0.4784f, 1.0f };

	CommonUtilities::Vector4<float> myViewPort = {0, 0, 0, 0};
};

