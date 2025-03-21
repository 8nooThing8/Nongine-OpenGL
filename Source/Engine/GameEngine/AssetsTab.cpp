#include "../Engine.pch.h"

#include "AssetsTab.h"

#include <ImGui/imgui.h>
#include "../GraphicsEngine/GraphicsEngine.h"
#include "../GraphicsEngine/DDSTextureLoader11.h"

#include "Windows.h"

#include <filesystem>
#include "Console.h"

#include <codecvt>

#include "../GameEngine/MainSingleton.h"

#include "SpriteEditor.h"
#include "SceneManagerInternal.h"

namespace fs = std::filesystem;

void AssetsTab::Init()
{
	DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Sprites/Folder.dds", texture, myFolderIcon.GetAddressOf());
	DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Sprites/EmptyFile.dds", texture, myEmptyFileIcon.GetAddressOf());
	DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Sprites/HLSLFileIcon.dds", texture, myHlslIcon.GetAddressOf());
	DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Sprites/HLSLIFileIcon.dds", texture, myHlsliIcon.GetAddressOf());
	DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Sprites/3dModelExample.dds", texture, myNoPreviewFBXIcon.GetAddressOf());
}

// move this to a helper class so it already has one in modelviewer.cpp
LPWSTR ConvertStringToWString(const std::string& str)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	LPWSTR lpwstr = new wchar_t[size];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, lpwstr, size);
	return lpwstr;
}

void AssetsTab::Render()
{
	ImGui::Begin("Assets");

	ImGui::Text("Assets");

	if (!opnedFolder)
	{
		filesInFolder.clear();

		if (!fs::exists(baseDirectory + currentDirectoryPath) || !fs::is_directory(baseDirectory + currentDirectoryPath))
		{
			PrintError("The directory (" + baseDirectory + currentDirectoryPath + ") Does not exist, [AssetTab]");
		}

		for (const auto& entry : fs::directory_iterator(baseDirectory + currentDirectoryPath))
		{
			const auto& path = entry.path();

			filesInFolder.push_back(FileS(path.filename().string(), path.string(), path.extension().string()));
			
			if (fs::is_regular_file(path.string()))
			{
				if (path.extension().string() == ".png" || path.extension().string() == ".dds")
				{
					std::wstring wstring = ConvertStringToWString(path.string());
					DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), wstring.c_str(), texture, filesInFolder.back().texture.GetAddressOf());
				}
				else if (path.extension().string() == ".fbx")
				{
					filesInFolder.back().texture = myNoPreviewFBXIcon;
				}
				else if (path.extension().string() == ".hlsl")
				{
					filesInFolder.back().texture = myHlslIcon;
				}
				else if (path.extension().string() == ".hlsli")
				{
					filesInFolder.back().texture = myHlsliIcon;
				}
				else
				{
					filesInFolder.back().texture = myEmptyFileIcon;
				}
			}
			else
			{
				filesInFolder.back().texture = myFolderIcon;
			}
		}

		opnedFolder = true;
	}

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
	{
		opnedFolder = false;

		currentDirectoryPath.clear();
		assetPathList.clear();
	}

	for (auto& folderName : assetPathList)
	{
		ImGui::SameLine();
		ImGui::Text(">");

		ImGui::SameLine();
		ImGui::Text(folderName.c_str());

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		{
			std::vector<std::string> assetPathListNew(assetPathList);

			opnedFolder = false;

			currentDirectoryPath.clear();
			assetPathList.clear();

			for (auto& folder : assetPathListNew)
			{
				currentDirectoryPath += folder + "/";
				assetPathList.push_back(folder);

				if (folderName == folder)
				{
					break;
				}
			}
		}
	}

	for (int i = 0; i < filesInFolder.size(); i++)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.85f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

		ImGui::BeginGroup();

		ImGui::ImageButton((ImTextureID)filesInFolder[i].texture.Get(), ImVec2(75, 75));

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			if (fs::is_regular_file(baseDirectory + currentDirectoryPath + filesInFolder[i].name))
			{
				if (std::filesystem::path(filesInFolder[i].name).extension() == ".dds")
				{
					MainSingleton::Get()->spriteEditor->Initilize(filesInFolder[i].texture.Get(), filesInFolder[i].path);
					MainSingleton::Get()->mySpriteEditorOpen = true;
				}
				if (std::filesystem::path(filesInFolder[i].name).extension() == ".no")
				{
					MainSingleton::Get()->activeScene = SceneManagerInternal::LoadScene(filesInFolder[i].path);
				}
				else
				{
					std::string finalPath = baseDirectory + currentDirectoryPath + filesInFolder[i].name;
					std::string absolute = std::filesystem::absolute(finalPath).string();

					ShellExecuteA(NULL, "open", absolute.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
			}
			else
			{
				assetPathList.push_back(filesInFolder[i].name);
				currentDirectoryPath += filesInFolder[i].name + "/";

				opnedFolder = false;
			}
		}

		if (hasPayload && !ImGui::IsMouseDown(0))
		{
			hasPayload = false;
			payload = "";
			payloadExt = "";
		}

		if (ImGui::IsItemHovered() && !hasPayload)
		{
			if (ImGui::BeginDragDropSource())
			{
				hasPayload = true;

				payload = filesInFolder[i].path;
				payloadExt = filesInFolder[i].fileExtension;

				ImGui::SetDragDropPayload(filesInFolder[i].fileExtension.c_str(), &payload, sizeof(int), ImGuiCond_Once);

				ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(150, 150, 150, 255));
				ImGui::Button(filesInFolder[i].name.c_str(), ImVec2(75, 75));
				ImGui::PopStyleColor();


				ImGui::EndDragDropSource();
			}
		}
		

		if (filesInFolder[i].name.size() > 10)
		{
			std::string characterCutOff(filesInFolder[i].name.begin(), filesInFolder[i].name.begin() + 10);
			ImGui::Text((characterCutOff + "...").c_str());
		}
		else
		{
			ImGui::Text(filesInFolder[i].name.c_str());
		}
		
		
		ImGui::EndGroup();

		int amountOfObjects = static_cast<int>(ImGui::GetWindowSize().x) / 100 - 1;

		if (amountOfObjects > 0)
		{
			if (i % amountOfObjects != 0 || i == 0)
			{
				ImGui::SameLine();
			}
		}
	}

	ImGui::End();
}