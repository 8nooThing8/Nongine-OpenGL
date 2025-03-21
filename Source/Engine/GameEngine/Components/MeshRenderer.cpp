#include "../../Engine.pch.h"

#include "MeshRenderer.h"

#include "../../GraphicsEngine/GraphicsEngine.h"

#include "../../GraphicsEngine/Buffers/ObjectBuffer.h"

#include "Animator.h"

#include "../AssetsTab.h"

#include "Transform.h"

#include <string>
#include "GameObject.h"
#include "../../GraphicsEngine/Shader.h"

#include "../Console.h"

#include "../ShaderResources.h"
#include "../ModelSprite/ModelFactory.h"

#include "../Engine/GameEngine/DefferedRendering.h"

#include <utility>
#include <vector>

MeshRenderer::MeshRenderer(TGA::FBX::Mesh* aMesh, Material aMaterial): shaderItems(nullptr), myMaterial(aMaterial),
                                                                       myMesh(aMesh), myCastShadow(true),
                                                                       myDeffered(false)
{
	Component();

	myMaterial.SetColor(CommonUtilities::Vector4<float>(1, 1, 1, 1));
}

MeshRenderer::MeshRenderer(const MeshRenderer& aModel): shaderItems(nullptr), myMaterial(aModel.myMaterial),
                                                        myMesh(aModel.myMesh), myCastShadow(true),
                                                        myDeffered(false)
{
	Component();

	//std::cout << "test";
}

void MeshRenderer::SetLayer(int aLayer)
{
	myLayer = aLayer;

	DefferedRendering::layers.insert(aLayer);
}

void MeshRenderer::RemoveLayer(int aLayer)
{
	myLayerToRemove = aLayer;

	DefferedRendering::layers.insert(aLayer);
}

void MeshRenderer::SetDeffered(const bool aDeffered)
{
	myDeffered = aDeffered;

	if (myDeffered)
	{
		GetMaterial().SetPixelShader(L"Assets/Shaders/Deffered_PS.hlsl");
	}
	else
	{
		GetMaterial().SetPixelShader(L"Assets/Shaders/PBL_Lit_PS.hlsl");
	}
}

void MeshRenderer::SetCastShadow(const bool aValue)
{
	myCastShadow = aValue;
}

void MeshRenderer::LateUpdateEditor(float /*x*/)
{
	if (myMesh && !myLayer)
	{
		GraphicsEngine::GetRHI().get()->SetInputLayout(GraphicsEngine::Get().GetdefaultPSO().InputLayout);
		GraphicsEngine::Get().DrawMesh(this);
	}
}

void MeshRenderer::ShaderCleanUp() const
{
	shaderItems->CleanUp();
}

void MeshRenderer::Draw(Microsoft::WRL::ComPtr<ID3D11PixelShader> shaderOverride)
{
	shaderItems->SetShaderObjects();

	GraphicsEngine::GetRHI().get()->SetInputLayout(GraphicsEngine::Get().GetdefaultPSO().InputLayout);

	Animator* animator = nullptr; 

	if (gameObject)
	{
		animator = gameObject->GetComponent<Animator>();
	}

	if (animator)
		GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, animator->myAnimationBuffer);

	ObjectBuffer& objectBuffer = gameObject->transform->GetObjBuffer();

	objectBuffer.albedo = myMaterial.GetColor();
	objectBuffer.metallic = myMaterial.GetMetallic();
	objectBuffer.roughness = myMaterial.GetRoughness();
	objectBuffer.bloomStrength = myMaterial.GetBloomStrength() + 1;

	objectBuffer.aoStrenght = myMaterial.GetAOStrength();

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);

	GraphicsEngine::Get().RenderMesh(myMesh, myMaterial, shaderOverride);
}

void MeshRenderer::Destroy()
{
	std::cout << "Destroy" << std::endl;
}

void MeshRenderer::RenderImGUI()
{
	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	if (ImGui::InputText("Pixel Shader", PXshaderPath, IM_ARRAYSIZE(PXshaderPath), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		std::filesystem::path path = "Assets/Shaders/";

		path.append(PXshaderPath);

		myMaterial.SetPixelShader(path.wstring());
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	if (ImGui::InputText("Vertex Shader", VXshaderPath, IM_ARRAYSIZE(VXshaderPath), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		std::filesystem::path path = "Assets/Shaders/";

		path.append(VXshaderPath);

		myMaterial.SetVertexShader(path.wstring());
	}

	if (AssetsTab::payloadExt == ".fbx")
	{
		ImGui::PushStyleColor(0, ImVec4(1.f, 0.843f, 0.f, 1.f));
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	if (myMesh)
	{
		if (myMesh->Name.empty())
		{
			if (ImGui::Button("#Error#"))
				ImGui::OpenPopup("my_mesh_select_popup");
		}
		else if (ImGui::Button(myMesh->Name.c_str()))
			ImGui::OpenPopup("my_mesh_select_popup");
		
	}
	else
	{
		if (ImGui::Button("<None>"))
			ImGui::OpenPopup("my_mesh_select_popup");
	}

	if (AssetsTab::payloadExt == ".fbx")
	{
		ImGui::PopStyleColor();
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".fbx");

		if (payload)
		{
			myMesh = CreateModel(AssetsTab::payload);

			Start();

			{
				Animator* anim = gameObject->GetComponent<Animator>();

				if (anim)
					anim->SetMesh(myMesh);
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	ImGui::Text("Mesh");


	if (ImGui::BeginPopup("my_mesh_select_popup"))
	{
		std::vector<std::string> meshes;

		std::string directoryPath = "Assets/Models";

		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			PrintError("Directory for texture imgui checking does not exist or is not a directory");
		}

		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			const auto& path = entry.path();
			if (fs::is_regular_file(entry))
			{
				meshes.push_back(path.filename().string());
			}
		}

		ImGui::SeparatorText("Meshes");
		for (int i = 0; i < meshes.size(); i++)
		{
			if (ImGui::Selectable(meshes[i].c_str()))
			{
				myMesh = CreateModel("Assets/Models/" + meshes[i]);

				Start();

				{
					Animator* anim = gameObject->GetComponent<Animator>();

					if (anim)
						anim->SetMesh(myMesh);
				}
			}
		}

		ImGui::EndPopup();
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PushStyleColor(0, ImVec4(1.f, 0.843f, 0.f, 1.f));
	}

	if (!myMaterial.GetPSShader()->GetDiffusePath().empty())
	{
		if (ImGui::Button(myMaterial.GetPSShader()->GetDiffuseName().c_str()))
			ImGui::OpenPopup("Diffuse");
	}
	else
	{
		if (ImGui::Button("<None>"))
			ImGui::OpenPopup("Diffuse");
	}

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PopStyleColor();
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds");

		if (payload)
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, nullptr, 0);
			LPWSTR lpwstr = new wchar_t[size];
			MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, lpwstr, size);

			size_t lastSlashPos = AssetsTab::payload.find_last_of("/\\");

			std::string filenameWithExtension;
			if (lastSlashPos != std::string::npos)
				filenameWithExtension = AssetsTab::payload.substr(lastSlashPos + 1);
			else
				filenameWithExtension = AssetsTab::payload;

			size_t lastDotPos = filenameWithExtension.find_last_of(".");

			std::string filename;
			if (lastDotPos != std::string::npos)
				filename = filenameWithExtension.substr(0, lastDotPos);
			else
				filename = filenameWithExtension;

			myMaterial.GetPSShader()->SetDiffuseName(filename);

			myMaterial.SetDiffuseTexture(lpwstr);
		}

		payload = ImGui::AcceptDragDropPayload(".png");

		if (payload)
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, nullptr, 0);
			LPWSTR lpwstr = new wchar_t[size];
			MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, lpwstr, size);

			size_t lastSlashPos = AssetsTab::payload.find_last_of("/\\");

			std::string filenameWithExtension;
			if (lastSlashPos != std::string::npos)
				filenameWithExtension = AssetsTab::payload.substr(lastSlashPos + 1);
			else
				filenameWithExtension = AssetsTab::payload;

			size_t lastDotPos = filenameWithExtension.find_last_of(".");

			std::string filename;
			if (lastDotPos != std::string::npos)
				filename = filenameWithExtension.substr(0, lastDotPos);
			else
				filename = filenameWithExtension;

			myMaterial.GetPSShader()->SetDiffuseName(filename);

			myMaterial.SetDiffuseTexture(lpwstr);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	ImGui::Text("Diffuse");

	if (ImGui::BeginPopup("Diffuse"))
	{
		std::string directoryPath = "Assets/Textures";

		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			PrintError("Directory for texture imgui checking does not exist or is not a directory");
		}

		std::vector<std::string> textures{ "None" };

		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			const auto& path = entry.path();
			if (fs::is_regular_file(entry))
			{
				textures.push_back(path.filename().string());
			}
		}

		ImGui::SeparatorText("Diffuse");
		for (int i = 0; i < textures.size(); i++)
		{
			if (ImGui::Selectable(textures[i].c_str()))
			{
				if (textures[i] == "None")
				{
					myMaterial.SetDiffuseTextureToNull();
				}

				int size = MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, nullptr, 0);
				LPWSTR lpwstr = new wchar_t[size];
				MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, lpwstr, size);

				std::wstring name(lpwstr);

				std::wstring path = L"Assets/Textures/" + name;

				size_t lastDotPos = textures[i].find_last_of(".");

				std::string filename;
				if (lastDotPos != std::string::npos)
					filename = textures[i].substr(0, lastDotPos);
				else
					filename = textures[i];

				myMaterial.GetPSShader()->SetDiffuseName(filename);

				myMaterial.SetDiffuseTexture(path);
			}
		}
		ImGui::EndPopup();
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PushStyleColor(0, ImVec4(1.f, 0.843f, 0.f, 1.f));
	}
	if (!myMaterial.GetPSShader()->GetNormalPath().empty())
	{
		std::wstring wName = myMaterial.GetPSShader()->GetShaderName();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wName[0], (int)wName.size(), NULL, 0, NULL, NULL);
		std::string name(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wName[0], (int)wName.size(), &name[0], size_needed, NULL, NULL);

		if (ImGui::Button(myMaterial.GetPSShader()->GetNormalName().c_str()))
			ImGui::OpenPopup("Normal");
	}
	else
	{
		if (ImGui::Button("<None>"))
			ImGui::OpenPopup("Normal");
	}

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PopStyleColor();
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds");

		if (payload)
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, nullptr, 0);
			LPWSTR lpwstr = new wchar_t[size];
			MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, lpwstr, size);

			size_t lastSlashPos = AssetsTab::payload.find_last_of("/\\");

			std::string filenameWithExtension;
			if (lastSlashPos != std::string::npos)
				filenameWithExtension = AssetsTab::payload.substr(lastSlashPos + 1);
			else
				filenameWithExtension = AssetsTab::payload;

			size_t lastDotPos = filenameWithExtension.find_last_of(".");

			std::string filename;
			if (lastDotPos != std::string::npos)
				filename = filenameWithExtension.substr(0, lastDotPos);
			else
				filename = filenameWithExtension;

			myMaterial.GetPSShader()->SetNormalName(filename);

			myMaterial.SetNormalTexture(lpwstr);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	ImGui::Text("Normal");

	if (ImGui::BeginPopup("Normal"))
	{
		std::string directoryPath = "Assets/Textures";

		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			PrintError("Directory for texture imgui checking does not exist or is not a directory");
		}

		std::vector<std::string> textures{ "None" };

		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			const auto& path = entry.path();
			if (fs::is_regular_file(entry))
			{
				textures.push_back(path.filename().string());
			}
		}

		ImGui::SeparatorText("Normal");
		for (int i = 0; i < textures.size(); i++)
		{
			if (ImGui::Selectable(textures[i].c_str()))
			{
				if (textures[i] == "None")
				{
					myMaterial.SetNormalTextureToNull();
				}

				int size = MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, nullptr, 0);
				LPWSTR lpwstr = new wchar_t[size];
				MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, lpwstr, size);

				std::wstring name(lpwstr);

				std::wstring path = L"Assets/Textures/" + name;

				std::string pathN = "Assets/Textures/" + textures[i];

				size_t lastSlashPos = pathN.find_last_of("/\\");

				std::string filenameWithExtension;
				if (lastSlashPos != std::string::npos)
					filenameWithExtension = pathN.substr(lastSlashPos + 1);
				else
					filenameWithExtension = pathN;

				size_t lastDotPos = filenameWithExtension.find_last_of(".");

				std::string filename;
				if (lastDotPos != std::string::npos)
					filename = filenameWithExtension.substr(0, lastDotPos);
				else
					filename = filenameWithExtension;

				myMaterial.GetPSShader()->SetNormalName(filename);

				myMaterial.SetNormalTexture(path);
			}
		}

		ImGui::EndPopup();
	}

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PushStyleColor(0, ImVec4(1.f, 0.843f, 0.f, 1.f));
	}
	if (false)
	{
		std::wstring wName = myMaterial.GetPSShader()->GetShaderName();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wName[0], (int)wName.size(), NULL, 0, NULL, NULL);
		std::string name(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wName[0], (int)wName.size(), &name[0], size_needed, NULL, NULL);

		if (ImGui::Button(myMaterial.GetPSShader()->GetNormalName().c_str()))
			ImGui::OpenPopup("Roughness");
	}
	else
	{
		if (ImGui::Button("<None> Roughness"))
			ImGui::OpenPopup("Roughness");
	}

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PopStyleColor();
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds");

		if (payload)
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, nullptr, 0);
			LPWSTR lpwstr = new wchar_t[size];
			MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, lpwstr, size);

			size_t lastSlashPos = AssetsTab::payload.find_last_of("/\\");

			std::string filenameWithExtension;
			if (lastSlashPos != std::string::npos)
				filenameWithExtension = AssetsTab::payload.substr(lastSlashPos + 1);
			else
				filenameWithExtension = AssetsTab::payload;

			size_t lastDotPos = filenameWithExtension.find_last_of(".");

			std::string filename;
			if (lastDotPos != std::string::npos)
				filename = filenameWithExtension.substr(0, lastDotPos);
			else
				filename = filenameWithExtension;

			myMaterial.GetPSShader()->SetRoughnessName(filename);

			myMaterial.SetRoughnessTexture(lpwstr);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	ImGui::Text("Roughness");

	if (ImGui::BeginPopup("Roughness"))
	{
		std::string directoryPath = "Assets/Textures";

		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			PrintError("Directory for texture imgui checking does not exist or is not a directory");
		}

		std::vector<std::string> textures{ "None" };

		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			const auto& path = entry.path();
			if (fs::is_regular_file(entry))
			{
				textures.push_back(path.filename().string());
			}
		}

		ImGui::SeparatorText("Roughness");
		for (int i = 0; i < textures.size(); i++)
		{
			if (ImGui::Selectable(textures[i].c_str()))
			{
				if (textures[i] == "None")
				{
					myMaterial.SetRoughnessTextureToNull();
				}

				int size = MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, nullptr, 0);
				LPWSTR lpwstr = new wchar_t[size];
				MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, lpwstr, size);

				std::wstring name(lpwstr);

				std::wstring path = L"Assets/Textures/" + name;

				std::string pathN = "Assets/Textures/" + textures[i];

				size_t lastSlashPos = pathN.find_last_of("/\\");

				std::string filenameWithExtension;
				if (lastSlashPos != std::string::npos)
					filenameWithExtension = pathN.substr(lastSlashPos + 1);
				else
					filenameWithExtension = pathN;

				size_t lastDotPos = filenameWithExtension.find_last_of(".");

				std::string filename;
				if (lastDotPos != std::string::npos)
					filename = filenameWithExtension.substr(0, lastDotPos);
				else
					filename = filenameWithExtension;

				myMaterial.GetPSShader()->SetRoughnessName(filename);

				myMaterial.SetRoughnessTexture(path);
			}
		}

		ImGui::EndPopup();
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PushStyleColor(0, ImVec4(1.f, 0.843f, 0.f, 1.f));
	}
	if (false)
	{
		std::wstring wName = myMaterial.GetPSShader()->GetShaderName();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wName[0], (int)wName.size(), NULL, 0, NULL, NULL);
		std::string name(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wName[0], (int)wName.size(), &name[0], size_needed, NULL, NULL);

		if (ImGui::Button(myMaterial.GetPSShader()->GetNormalName().c_str()))
			ImGui::OpenPopup("Metallic");
	}
	else
	{
		if (ImGui::Button("<None>Meatllic"))
			ImGui::OpenPopup("Metallic");
	}

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PopStyleColor();
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds");

		if (payload)
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, nullptr, 0);
			LPWSTR lpwstr = new wchar_t[size];
			MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, lpwstr, size);

			size_t lastSlashPos = AssetsTab::payload.find_last_of("/\\");

			std::string filenameWithExtension;
			if (lastSlashPos != std::string::npos)
				filenameWithExtension = AssetsTab::payload.substr(lastSlashPos + 1);
			else
				filenameWithExtension = AssetsTab::payload;

			size_t lastDotPos = filenameWithExtension.find_last_of(".");

			std::string filename;
			if (lastDotPos != std::string::npos)
				filename = filenameWithExtension.substr(0, lastDotPos);
			else
				filename = filenameWithExtension;

			myMaterial.GetPSShader()->SetAOName(filename);

			myMaterial.SetMetallicTexture(lpwstr);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();


	ImGui::Text("Metallic");

	if (ImGui::BeginPopup("Metallic"))
	{
		std::string directoryPath = "Assets/Textures";

		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			PrintError("Directory for texture imgui checking does not exist or is not a directory");
		}

		std::vector<std::string> textures{ "None" };

		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			const auto& path = entry.path();
			if (fs::is_regular_file(entry))
			{
				textures.push_back(path.filename().string());
			}
		}

		ImGui::SeparatorText("Metallic");
		for (int i = 0; i < textures.size(); i++)
		{
			if (ImGui::Selectable(textures[i].c_str()))
			{
				if (textures[i] == "None")
				{
					myMaterial.SetMetallicTextureToNull();
				}

				int size = MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, nullptr, 0);
				LPWSTR lpwstr = new wchar_t[size];
				MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, lpwstr, size);

				std::wstring name(lpwstr);

				std::wstring path = L"Assets/Textures/" + name;

				std::string pathN = "Assets/Textures/" + textures[i];

				size_t lastSlashPos = pathN.find_last_of("/\\");

				std::string filenameWithExtension;
				if (lastSlashPos != std::string::npos)
					filenameWithExtension = pathN.substr(lastSlashPos + 1);
				else
					filenameWithExtension = pathN;

				size_t lastDotPos = filenameWithExtension.find_last_of(".");

				std::string filename;
				if (lastDotPos != std::string::npos)
					filename = filenameWithExtension.substr(0, lastDotPos);
				else
					filename = filenameWithExtension;

				myMaterial.GetPSShader()->SetMetalliclName(filename);

				myMaterial.SetMetallicTexture(path);
			}
		}

		ImGui::EndPopup();
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PushStyleColor(0, ImVec4(1.f, 0.843f, 0.f, 1.f));
	}
	if (false)
	{
		std::wstring wName = myMaterial.GetPSShader()->GetShaderName();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wName[0], (int)wName.size(), NULL, 0, NULL, NULL);
		std::string name(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wName[0], (int)wName.size(), &name[0], size_needed, NULL, NULL);

		if (ImGui::Button(myMaterial.GetPSShader()->GetNormalName().c_str()))
			ImGui::OpenPopup("AO");
	}
	else
	{
		if (ImGui::Button("<None>AO"))
			ImGui::OpenPopup("AO");
	}

	if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
	{
		ImGui::PopStyleColor();
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds");

		if (payload)
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, nullptr, 0);
			LPWSTR lpwstr = new wchar_t[size];
			MultiByteToWideChar(CP_UTF8, 0, AssetsTab::payload.c_str(), -1, lpwstr, size);

			size_t lastSlashPos = AssetsTab::payload.find_last_of("/\\");

			std::string filenameWithExtension;
			if (lastSlashPos != std::string::npos)
				filenameWithExtension = AssetsTab::payload.substr(lastSlashPos + 1);
			else
				filenameWithExtension = AssetsTab::payload;

			size_t lastDotPos = filenameWithExtension.find_last_of(".");

			std::string filename;
			if (lastDotPos != std::string::npos)
				filename = filenameWithExtension.substr(0, lastDotPos);
			else
				filename = filenameWithExtension;

			myMaterial.GetPSShader()->SetAOName(filename);

			myMaterial.SetAOTexture(lpwstr);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();


	ImGui::Text("AO");

	if (ImGui::BeginPopup("AO"))
	{
		std::string directoryPath = "Assets/Textures";

		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			PrintError("Directory for texture imgui checking does not exist or is not a directory");
		}

		std::vector<std::string> textures{ "None" };

		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			const auto& path = entry.path();
			if (fs::is_regular_file(entry))
			{
				textures.push_back(path.filename().string());
			}
		}

		ImGui::SeparatorText("AO");
		for (int i = 0; i < textures.size(); i++)
		{
			if (ImGui::Selectable(textures[i].c_str()))
			{
				if (textures[i] == "None")
				{
					myMaterial.SetAOTextureToNull();
				}

				int size = MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, nullptr, 0);
				LPWSTR lpwstr = new wchar_t[size];
				MultiByteToWideChar(CP_UTF8, 0, textures[i].c_str(), -1, lpwstr, size);

				std::wstring name(lpwstr);

				std::wstring path = L"Assets/Textures/" + name;

				std::string pathN = "Assets/Textures/" + textures[i];

				size_t lastSlashPos = pathN.find_last_of("/\\");

				std::string filenameWithExtension;
				if (lastSlashPos != std::string::npos)
					filenameWithExtension = pathN.substr(lastSlashPos + 1);
				else
					filenameWithExtension = pathN;

				size_t lastDotPos = filenameWithExtension.find_last_of(".");

				std::string filename;
				if (lastDotPos != std::string::npos)
					filename = filenameWithExtension.substr(0, lastDotPos);
				else
					filename = filenameWithExtension;

				myMaterial.GetPSShader()->SetAOName(filename);

				myMaterial.SetAOTexture(path);
			}
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	ImGui::DragFloat("AO strength", &myMaterial.GetAOStrength(), 0.1f);

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	if (ImGui::ColorButton("Albedo Color", ImVec4(myMaterial.GetColor().x, myMaterial.GetColor().y, myMaterial.GetColor().z, myMaterial.GetColor().w), NULL, ImVec2(150, 17)))
		ImGui::OpenPopup("Color");

	ImGui::SameLine();

	ImGui::Text("Albedo Color");

	if (ImGui::BeginPopup("Color"))
	{
		ImGui::ColorPicker4(" ", &myMaterial.GetColor()[0]);

		ImGui::EndPopup();
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	ImGui::DragFloat("Metallic", &myMaterial.GetMetallic(), 0.01f, 0.f, 1.f);

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	ImGui::DragFloat("Roughness", &myMaterial.GetRoughness(), 0.01f, 0.f, 1.f);

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	ImGui::DragFloat("Bloom strength", &myMaterial.GetBloomStrength(), 0.01f, 0.f);

	if (ImGui::InputInt("Layer", &myLayer))
	{
		SetLayer(myLayer);
	}
	if (ImGui::InputInt("RemoveLayer", &myLayerToRemove))
	{
		RemoveLayer(myLayerToRemove);
	}
	

	//static char buffer[64];

	//ImGui::InputText("ShaderName", buffer, sizeof(buffer) - 1);

	//if (ImGui::IsItemDeactivatedAfterEdit())
	//{
	//    wchar_t wideBuffer[128];
	//    int len = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer, 128);
	//    if (len > 0) 
	//    {
	//        myMaterial.SetShader(wideBuffer);
	//    }
	//    else 
	//    {
	//        myMaterial.SetShader(L"defaultTexture.dds");
	//    }
	//}
}

MeshRenderer::MeshRenderer(): myCastShadow(true)
{
	myMaterial = Material(L"Assets/Shaders/Deffered_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
	myMaterial.SetColor(CommonUtilities::Vector4<float>(1, 1, 1, 1));

	shaderItems = new ShaderResources();

	myDeffered = false;
}

TGA::FBX::Mesh* MeshRenderer::GetMesh() const
{
	return myMesh;
}

Material& MeshRenderer::GetMaterial()
{
	return myMaterial;
}

void MeshRenderer::HandleSave(rapidjson::Value& outJsonObj, rapidjson::Document& aDocument)
{ 
	std::string meshNameString = myMesh->Name;
	rapidjson::Value meshName(meshNameString.c_str(), aDocument.GetAllocator());
	outJsonObj.AddMember("Mesh", meshName, aDocument.GetAllocator());

	auto& pxShader = myMaterial.GetPSShader();
	std::wstring pxshaderPath = pxShader->GetShaderPath();
	std::string pxShaderPath = std::filesystem::path(pxshaderPath).string();
	rapidjson::Value pxshaderValue(pxShaderPath.c_str(), aDocument.GetAllocator());
	outJsonObj.AddMember("Pixel shader", pxshaderValue, aDocument.GetAllocator());

	auto& vxShader = myMaterial.GetVXShader();
	std::wstring vxshaderPath = vxShader->GetShaderPath();
	std::string vxShaderPath = std::filesystem::path(vxshaderPath).string();
	rapidjson::Value vxshaderValue(vxShaderPath.c_str(), aDocument.GetAllocator());
	outJsonObj.AddMember("Vertex shader", vxshaderValue, aDocument.GetAllocator());

	std::string diffusePath = pxShader->GetDiffusePath();
	rapidjson::Value diffuse(diffusePath.c_str(), aDocument.GetAllocator());
	outJsonObj.AddMember("Diffuse texture", diffuse, aDocument.GetAllocator());

	std::string normalPath = pxShader->GetNormalPath();
	rapidjson::Value normal(normalPath.c_str(), aDocument.GetAllocator());
	outJsonObj.AddMember("Normal texture", normal, aDocument.GetAllocator());

	std::string roughnessPath = pxShader->GetRoughnessPath();
	rapidjson::Value roughness(roughnessPath.c_str(), aDocument.GetAllocator());
	outJsonObj.AddMember("Roughness texture", roughness, aDocument.GetAllocator());

	std::string metallicPath = pxShader->GetMetallicPath();
	rapidjson::Value metallic(metallicPath.c_str(), aDocument.GetAllocator());
	outJsonObj.AddMember("Metallic texture", metallic, aDocument.GetAllocator());

	std::string aoPath = pxShader->GetAOPath();
	rapidjson::Value ao(aoPath.c_str(), aDocument.GetAllocator());
	outJsonObj.AddMember("AO texture", ao, aDocument.GetAllocator());

	rapidjson::Value albedo(rapidjson::kArrayType);
	albedo.PushBack(myMaterial.GetColor().x, aDocument.GetAllocator());
	albedo.PushBack(myMaterial.GetColor().y, aDocument.GetAllocator());
	albedo.PushBack(myMaterial.GetColor().z, aDocument.GetAllocator());
	albedo.PushBack(myMaterial.GetColor().w, aDocument.GetAllocator());
	outJsonObj.AddMember("Albedo", albedo, aDocument.GetAllocator());
}

void MeshRenderer::HandleLoad(const rapidjson::Value& outJsonObj)
{
	std::string meshName = outJsonObj["Mesh"].GetString();

	if (!meshName.empty())
		myMesh = CreateModel(meshName);
	else
		myMesh = nullptr;

	myDeffered = true;

	std::wstring pxShaderPath = std::filesystem::path(outJsonObj["Pixel shader"].GetString()).wstring();
	myMaterial.SetPixelShader(pxShaderPath);
	std::wstring vxShader = std::filesystem::path(outJsonObj["Vertex shader"].GetString()).wstring();
	myMaterial.SetVertexShader(vxShader);
	std::wstring diffusePath = std::filesystem::path(outJsonObj["Diffuse texture"].GetString()).wstring();
	myMaterial.SetDiffuseTexture(diffusePath);
	std::wstring normalPath = std::filesystem::path(outJsonObj["Normal texture"].GetString()).wstring();
	myMaterial.SetNormalTexture(normalPath);
	std::wstring roughnessPath = std::filesystem::path(outJsonObj["Roughness texture"].GetString()).wstring();
	myMaterial.SetRoughnessTexture(roughnessPath);
	std::wstring metallicPath = std::filesystem::path(outJsonObj["Metallic texture"].GetString()).wstring();
	myMaterial.SetMetallicTexture(metallicPath);
	std::wstring aoPath = std::filesystem::path(outJsonObj["AO texture"].GetString()).wstring();
	myMaterial.SetAOTexture(aoPath);

	Vector4<float> color{
		outJsonObj["Albedo"][0].GetFloat(),
		outJsonObj["Albedo"][1].GetFloat(),
		outJsonObj["Albedo"][2].GetFloat(),
		outJsonObj["Albedo"][3].GetFloat()
	};

	myMaterial.SetColor(color);
}
