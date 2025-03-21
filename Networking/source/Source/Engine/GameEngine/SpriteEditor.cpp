#include "../Engine.pch.h"

#include "SpriteEditor.h"

#include "Components/SpriteRenderer.h"
#include "Components/GameObject.h"

#include <strstream>

#include <Matrix/Matrix4x4.hpp>
#include <InputHandler.h>

#include <rapidjson-master/include/rapidjson/filewritestream.h>
#include <rapidjson-master/include/rapidjson/filereadstream.h>
#include <rapidjson-master/include/rapidjson/document.h>
#include "rapidjson-master/include/rapidjson/prettywriter.h"

#include "ImGui/imgui.h"

#include "ModelSprite/Sprite.h"

#undef GetObject

SpriteEditor::SpriteEditor()
{
	//myEdgeSprite = std::make_shared<GameObject>("Dot");;
	//myEdgeSpriteRenderer = myEdgeSprite->AddComponent<SpriteRenderer>(false);

	//myEdgeSpriteRenderer->mySprite = new Sprite();

	//ID3D11Resource** texture = nullptr;

	//DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Sprites/BlueDot.dds", texture, myEdgeSpriteRenderer->mySprite->aTexture.GetAddressOf());
}

void  SpriteEditor::SaveSpriteConfig()
{
	CommonUtilities::Vector4<float> returnColor;

	rapidjson::Document document;
	document.SetObject();

	rapidjson::Value allRects(rapidjson::kArrayType);

	rapidjson::Value nameExt(myImagePath.string().c_str(), document.GetAllocator());

	document.AddMember("name", nameExt, document.GetAllocator());

	for (int i = 0; i < spriteBuffer.numberOfSprites; i++)
	{
		rapidjson::Value object(rapidjson::kObjectType);

		//rapidjson::Value jsonName("NoName", document.GetAllocator());
		//object.AddMember("name", jsonName, document.GetAllocator());

		rapidjson::Value rect(rapidjson::kObjectType);

		CommonUtilities::Vector4<float> currentSpriteRects = spriteBuffer.spritesMinMax[i];

		rapidjson::Value someMin(rapidjson::kArrayType);
		someMin.PushBack(currentSpriteRects.x, document.GetAllocator());
		someMin.PushBack(currentSpriteRects.y, document.GetAllocator());

		object.AddMember("Min", someMin, document.GetAllocator());

		rapidjson::Value someMax(rapidjson::kArrayType);
		someMax.PushBack(currentSpriteRects.z, document.GetAllocator());
		someMax.PushBack(currentSpriteRects.w, document.GetAllocator());

		object.AddMember("Max", someMax, document.GetAllocator());

		allRects.PushBack(object, document.GetAllocator());
	}

	document.AddMember("Sprite rects", allRects, document.GetAllocator());

	char writeBuffer[65536];
	FILE* fp;

	errno_t err;

	std::filesystem::path path = myImagePath;

	std::string pathWithExt = path.replace_extension(std::filesystem::path(".meta")).string();

	err = fopen_s(&fp, pathWithExt.c_str(), "w");

	if (err != 0)
	{
		throw("could not save file to: " + pathWithExt);
	}

	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

	document.Accept(writer);

	fclose(fp);
}

bool SpriteEditor::FindMetaFile()
{
	FILE* fp;
	char readBuffer[65536];

	std::filesystem::path path = myImagePath;
	std::string pathWithExt = path.replace_extension(std::filesystem::path(".meta")).string();

	errno_t err = fopen_s(&fp, pathWithExt.c_str(), "r");
	if (err != 0)
		return false;

	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document document;
	document.ParseStream(is);

	fclose(fp);

	if (!document.IsObject()) 
	{
		return false;
	}

	//if (document.HasMember("name"))
	//{
	//	newScene.name = document["name"].GetString();
	//}

	const rapidjson::Value& objects = document["Sprite rects"];
	if (!objects.IsArray())
	{
		return false;
	}

	for (unsigned i = 0; i < objects.GetArray().Size(); i++)
	{
		auto& spriteRect = objects.GetArray()[i];

		const rapidjson::Value& min = spriteRect["Min"];
		Vector2<float> rectMin
		(
			min[0].GetFloat(),
			min[1].GetFloat()
		);

		const rapidjson::Value& max = spriteRect["Max"];
		Vector2<float> rectMax
		(
			max[0].GetFloat(),
			max[1].GetFloat()
		);

		spriteBuffer.spritesMinMax[i].x = rectMin.x;
		spriteBuffer.spritesMinMax[i].y = rectMin.y;
		spriteBuffer.spritesMinMax[i].z = rectMax.x;
		spriteBuffer.spritesMinMax[i].w = rectMax.y;

		numberOfSprites++;
	}

	return true;
}

void SpriteEditor::Initilize(ID3D11ShaderResourceView* aImage, std::filesystem::path aPath)
{
	myImagePath = aPath;

	spriteBuffer = SpriteEditorBuffer();
	numberOfSprites = 0;

	FindMetaFile();

	InputHandler::GetInstance()->SetMouseDeltaZero();

	ID3D11Resource* SourceResource = nullptr;

	aImage->GetResource(&SourceResource);

	ID3D11Texture2D* texture2d = nullptr;

	SourceResource->QueryInterface(&texture2d);

	D3D11_TEXTURE2D_DESC desc;
	texture2d->GetDesc(&desc);

	int width = desc.Width;
	int height = desc.Height;

	myImageSize = ImVec2(static_cast<float>(width), static_cast<float>(height));
	myImageSRV = aImage;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = static_cast<unsigned>(myImageSize.x);
	textureDesc.Height = static_cast<unsigned>(myImageSize.y);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3D11Device>& device = GraphicsEngine::GetRHI()->GetDevice();

	// Create the shadow sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // Point filtering
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Texture addressing mode
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f; // Border color
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;

	device->CreateSamplerState(&samplerDesc, myPointSampler.GetAddressOf());

	mySprite = std::make_shared<GameObject>("aSprite");

	ID3D11Texture2D* texture = nullptr;
	hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);

	hr = device->CreateRenderTargetView(texture, nullptr, &myImageWithBackgroundRTV);

	hr = device->CreateShaderResourceView(texture, nullptr, &myImageWithBackgroundSRV);

	mySpriteRenderer = mySprite->AddComponent<SpriteRenderer>(false);

	mySpriteRenderer->GetMaterial().SetPixelShader(L"Assets/Shaders/Sprite_EditorShader_PS.hlsl");
	mySpriteRenderer->GetMaterial().SetVertexShader(L"Assets/Shaders/Sprite_EditorShader_VS.hlsl");

	mySpriteRenderer->GetSprite() = new Sprite();

	spriteBuffer.textureResolution = CommonUtilities::Vector2<float>(myImageSize.x, myImageSize.y);

	realPos = CommonUtilities::Vector4<float>();
}

bool IsInsideRect(const CommonUtilities::Vector2<float> aPos, const CommonUtilities::Vector4<float> aMinMax)
{
	if (aPos.x > aMinMax.z || aPos.x < aMinMax.x)
		return false;
	if (aPos.y > aMinMax.w || aPos.y < aMinMax.y)
		return false;

	return true;
}

void SpriteEditor::CloseSpriteEditor()
{

}

int SpriteEditor::isOnSelectedBorder(const CommonUtilities::Vector2<float> aPos)
{
	float marginal = 4.f / (myScrollDelta * 0.0001f);

	int touchingedge = 0;

	if (aPos.y < spriteBuffer.spritesMinMax[selectedSprite].w + marginal && aPos.y > spriteBuffer.spritesMinMax[selectedSprite].y - marginal)
	{
		if (aPos.x > spriteBuffer.spritesMinMax[selectedSprite].z - marginal && aPos.x < spriteBuffer.spritesMinMax[selectedSprite].z + marginal)
			touchingedge |= Left;
		if (aPos.x > spriteBuffer.spritesMinMax[selectedSprite].x - marginal && aPos.x < spriteBuffer.spritesMinMax[selectedSprite].x + marginal)
			touchingedge |= Right;
	}
	if (aPos.x < spriteBuffer.spritesMinMax[selectedSprite].z + marginal && aPos.x > spriteBuffer.spritesMinMax[selectedSprite].x - marginal)
	{
		if (aPos.y > spriteBuffer.spritesMinMax[selectedSprite].w - marginal && aPos.y < spriteBuffer.spritesMinMax[selectedSprite].w + marginal)
			touchingedge |= Up;
		if (aPos.y > spriteBuffer.spritesMinMax[selectedSprite].y - marginal && aPos.y < spriteBuffer.spritesMinMax[selectedSprite].y + marginal)
			touchingedge |= Down;
	}

	return touchingedge;
}

void SpriteEditor::SetCursorInternal(const int input)
{
	HCURSOR hCursor;

	switch (input)
	{
	case Left | Up:
	case Right | Down:
		hCursor = LoadCursor(nullptr, IDC_SIZENWSE);
		break;
	case Right | Up:
	case Left | Down:
		hCursor = LoadCursor(nullptr, IDC_SIZENESW);
		break;
	case Left:
	case Right:
		hCursor = LoadCursor(nullptr, IDC_SIZEWE);
		break;
	case Up:
	case Down:
		hCursor = LoadCursor(nullptr, IDC_SIZENS);
		break;

	default:
		hCursor = LoadCursor(nullptr, IDC_ARROW);
		break;
	}

	SetCursor(hCursor);
}

void SpriteEditor::PropertiesWindow(const ImVec2& windowPosition, const ImVec2& windowSize)
{
	ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	if (ImGui::IsMouseClicked(0) && !ImGui::IsWindowHovered() && !cursor)
	{
		selectedSprite = -1;
	}

	ImGui::SetWindowPos(ImVec2(windowPosition.x + windowSize.x - ImGui::GetWindowSize().x - 10, windowPosition.y + windowSize.y - ImGui::GetWindowSize().y - 50));

	ImGui::Text("Min: ");

	ImGui::Text("X:");
	ImGui::SameLine();

	ImGui::DragFloat(" ", &spriteBuffer.spritesMinMax[selectedSprite][0], 1.f, 0.f, spriteBuffer.spritesMinMax[selectedSprite].z);

	ImGui::Text("Y:");
	ImGui::SameLine();

	ImGui::DragFloat("  ", &spriteBuffer.spritesMinMax[selectedSprite][1], 1.f, 0.f, spriteBuffer.spritesMinMax[selectedSprite].w);

	ImGui::Text("Max: ");

	ImGui::Text("X:");
	ImGui::SameLine();

	ImGui::DragFloat("   ", &spriteBuffer.spritesMinMax[selectedSprite][2], 1.f, spriteBuffer.spritesMinMax[selectedSprite].x, myImageSize.x);

	ImGui::Text("Y:");
	ImGui::SameLine();

	ImGui::DragFloat("    ", &spriteBuffer.spritesMinMax[selectedSprite][3], 1.f, spriteBuffer.spritesMinMax[selectedSprite].y, myImageSize.y);

	ImGui::End();

	spriteBuffer.spritesMinMax[selectedSprite][0] = floor(spriteBuffer.spritesMinMax[selectedSprite][0]);
	spriteBuffer.spritesMinMax[selectedSprite][1] = floor(spriteBuffer.spritesMinMax[selectedSprite][1]);

	spriteBuffer.spritesMinMax[selectedSprite][2] = ceil(spriteBuffer.spritesMinMax[selectedSprite][2]);
	spriteBuffer.spritesMinMax[selectedSprite][3] = ceil(spriteBuffer.spritesMinMax[selectedSprite][3]);
}

void SpriteEditor::Update()
{
	SetCursorInternal(cursor);

	GraphicsEngine::GetRHI()->ClearRenderTargetView(myImageWithBackgroundRTV);

	if (InputHandler::GetInstance()->GetScrollWheelDelta() < 0)
	{
		InputHandler::GetInstance()->SetMouseDeltaZero();
	}

	myScrollDelta = InputHandler::GetInstance()->GetScrollWheelDelta() + 1024;
	mySizeMultiplier = 1.f + (pow(myScrollDelta, 2.f) * scrollSpeed);

	ImGui::Begin("SpriteSheet", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

	if (ImGui::Button("Save"))
	{
		SaveSpriteConfig();
	}

	float aspectMultiplier = size.y / size.x;

	realPos += CommonUtilities::Vector4<float>((ImGui::GetMouseDragDelta(1, 0.1f).x), (-ImGui::GetMouseDragDelta(1, 0.1f).y), 0, 0);
	mySprite->transform->position = realPos / CommonUtilities::Vector4<float>(size.x * 0.00052f, size.y * 0.00052f, 1, 1);

	CommonUtilities::Vector4<float> realSize = CommonUtilities::Vector4<float>(myScrollDelta, myScrollDelta, 0, 1);
	CommonUtilities::Vector4<float> tests = CommonUtilities::Vector4<float>(mySizeMultiplier, mySizeMultiplier, 0, 1);

	mySprite->transform->scale = tests * CommonUtilities::Vector4<float>(aspectMultiplier, 1, 0, 1);

	mySprite->transform->Update(0);

	ImVec2 mousePos = ImGui::GetMousePos();

	ImVec2 mouseScreenPos;
	mouseScreenPos.x = mousePos.x - position.x;
	mouseScreenPos.y = mousePos.y - position.y;

	float windowCenterX = size.x / 2.0f;
	float windowCenterY = size.y / 2.0f;

	ImVec2 spritePos = ImVec2(realPos.x, realPos.y);

	float baseTextureSize = 2048.0f;
	float baseScaleOffset = 11500.0f;

	float derivedScaleOffsetX = (myImageSize.x / baseTextureSize) * baseScaleOffset;
	float derivedScaleOffsetY = (myImageSize.y / baseTextureSize) * baseScaleOffset;

	CommonUtilities::Vector2<float> mouseToSprite((((mouseScreenPos.x - windowCenterX - spritePos.x) / (tests.x / derivedScaleOffsetX) + myImageSize.x) * 0.5f), (((mouseScreenPos.y - windowCenterY + spritePos.y) / (tests.y / derivedScaleOffsetY) + myImageSize.y) * 0.5f));

	mouseToSprite = CommonUtilities::Vector2<float>(std::clamp(mouseToSprite.x, 0.f, myImageSize.x), std::clamp(mouseToSprite.y, 0.f, myImageSize.y));

	//std::stringstream stream;
	//stream << mouseToSprite.x << "   |   " << mouseToSprite.y;
	//PrintLog(stream.str());

	//CommonUtilities::Vector4<float> newPosition = CommonUtilities::Vector4<float>(0, 0, 0, 0);

	//if (myPrevSizeMultiplier < mySizeMultiplier)
	//{
	//	newPosition = CommonUtilities::Vector4<float>(-mouseScreenPos.x, mouseScreenPos.y, 0, 0);
	//}

	//if (myPrevSizeMultiplier > mySizeMultiplier)
	//{
	//	newPosition = CommonUtilities::Vector4<float>(mouseScreenPos.x, -mouseScreenPos.y, 0, 0);
	//}


	ImGui::ResetMouseDragDelta(1);

	D3D11_VIEWPORT viewPort = {};

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = myImageSize.x;
	viewPort.Height = myImageSize.y;

	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	if (selectedSprite >= 0)
	{
		ImVec2 positionition = position;

		PropertiesWindow(positionition, size);

		int onEdge = isOnSelectedBorder(mouseToSprite);

		if (!ImGui::IsMouseDragging(0))
		{
			cursor = onEdge;
		}

		if (InputHandler::GetInstance()->GetKeyDown(keycode::DELETE_BUTTON))
		{
			for (int j = selectedSprite; j < numberOfSprites; j++)
			{
				spriteBuffer.spritesMinMax[j] = spriteBuffer.spritesMinMax[j + 1];
			}

			--numberOfSprites;

			selectedSprite = -1;
		}
	}
	else
	{
		cursor = 0;
	}

	hoveredSprite = -1;

	for (int i = 0; i < numberOfSprites; i++)
	{
		if (IsInsideRect(mouseToSprite, spriteBuffer.spritesMinMax[i]) && !cursor)
		{
			hoveredSprite = i;
			break;
		}
	}

	if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !cursor)
	{
		bool hasInside = false;

		for (int i = 0; i < numberOfSprites; i++)
		{
			if (IsInsideRect(mouseToSprite, spriteBuffer.spritesMinMax[i]))
			{
				selectedSprite = i;
				hasInside = true;
				break;
			}
		}

		if (!hasInside)
		{
			numberOfSprites++;

			tempMinMax = CommonUtilities::Vector4<float>(floor(mouseToSprite.x), floor(mouseToSprite.y), floor(mouseToSprite.x), floor(mouseToSprite.y));

			creatingNewRect = true;
		}
	}

	if (ImGui::IsMouseDragging(0))
	{
		if (cursor)
		{
			tempMinMax = CommonUtilities::Vector4<float>(floor(mouseToSprite.x), floor(mouseToSprite.y), floor(mouseToSprite.x), floor(mouseToSprite.y));

			if (cursor & Right)
			{
				spriteBuffer.spritesMinMax[selectedSprite].x = floor(mouseToSprite.x);
				spriteBuffer.spritesMinMax[selectedSprite].x = Clamp(spriteBuffer.spritesMinMax[selectedSprite].x, 0.f, spriteBuffer.spritesMinMax[selectedSprite].z);
			}
			else if (cursor & Left)
			{
				spriteBuffer.spritesMinMax[selectedSprite].z = ceil(mouseToSprite.x);
				spriteBuffer.spritesMinMax[selectedSprite].z = Clamp(spriteBuffer.spritesMinMax[selectedSprite].z, spriteBuffer.spritesMinMax[selectedSprite].x, myImageSize.x);
			}
			if (cursor & Down)
			{
				spriteBuffer.spritesMinMax[selectedSprite].y = floor(mouseToSprite.y);
				spriteBuffer.spritesMinMax[selectedSprite].y = Clamp(spriteBuffer.spritesMinMax[selectedSprite].y, 0.f, spriteBuffer.spritesMinMax[selectedSprite].w);
			}
			else if (cursor & Up)
			{
				spriteBuffer.spritesMinMax[selectedSprite].w = ceil(mouseToSprite.y);
				spriteBuffer.spritesMinMax[selectedSprite].w = Clamp(spriteBuffer.spritesMinMax[selectedSprite].w, spriteBuffer.spritesMinMax[selectedSprite].y, myImageSize.y);
			}


		}
		else if (creatingNewRect)
		{
			tempMinMax.z = floor(mouseToSprite.x);
			tempMinMax.w = floor(mouseToSprite.y);
		}
	}

	if (creatingNewRect)
	{
		spriteBuffer.spritesMinMax[numberOfSprites - 1].x = min(tempMinMax.x, tempMinMax.z);
		spriteBuffer.spritesMinMax[numberOfSprites - 1].y = min(tempMinMax.y, tempMinMax.w);

		spriteBuffer.spritesMinMax[numberOfSprites - 1].z = max(tempMinMax.x, tempMinMax.z);
		spriteBuffer.spritesMinMax[numberOfSprites - 1].w = max(tempMinMax.y, tempMinMax.w);
	}

	if (ImGui::IsMouseReleased(0) && creatingNewRect)
	{
		creatingNewRect = false;
	}

	spriteBuffer.numberOfSprites = numberOfSprites;
	spriteBuffer.selectedSprite = selectedSprite;
	spriteBuffer.hoveredSprite = hoveredSprite;

	spriteBuffer.zoomAmount = mySizeMultiplier;

	GraphicsEngine::GetRHI()->SetRenderTargetView(myImageWithBackgroundRTV, viewPort);

	GraphicsEngine::GetRHI()->GetContext()->PSSetSamplers(3, 1, myPointSampler.GetAddressOf());
	GraphicsEngine::GetRHI()->GetContext()->PSSetShaderResources(30, 1, &myImageSRV);


	mySpriteRenderer->LateUpdateEditor(0);

	/*myEdgeSprite->transform->scale = CommonUtilities::Vector4<float>(64, 64 * aspectMultiplier, 0, 0);

	if (selectedSprite >= 0)
	{
		CommonUtilities::Vector2<float> position;
		position.x = spriteBuffer.spritesMinMax[selectedSprite].x;
		position.y = spriteBuffer.spritesMinMax[selectedSprite].w - spriteBuffer.spritesMinMax[selectedSprite].y;

		ImVec2 mappedPos(position.x - position.x, position.y - position.y);

		CommonUtilities::Vector2<float> coordToSprite(((position.x - windowCenterX - spritePos.x - 8.5) / (tests.x / derivedScaleOffsetX) + myImageSize.x) * 0.5f, ((position.y - windowCenterY + spritePos.y - 26) / (tests.y / derivedScaleOffsetY) + myImageSize.y) * 0.5f);

		myEdgeSprite->transform->position.x = coordToSprite.x;
		myEdgeSprite->transform->position.y = coordToSprite.y;

		myEdgeSprite->transform->Update(0);

		myEdgeSpriteRenderer->LateUpdateEditor(0);
	}*/

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::SpriteEditor, spriteBuffer);

	ImGui::Image((ImTextureID)myImageWithBackgroundSRV, ImGui::GetWindowSize());

	position = ImGui::GetItemRectMin();
	size = ImGui::GetItemRectSize();

	ImGui::End();

	myPrevSizeMultiplier = mySizeMultiplier;
}
