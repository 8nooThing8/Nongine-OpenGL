#include "../../Engine.pch.h"

#include "SpriteRenderer.h"

#include "../../GraphicsEngine/GraphicsEngine.h"
#include "../../GraphicsEngine/Buffers/ObjectBuffer.h"
#include "../../GraphicsEngine/DDSTextureLoader11.h"

#include "../GraphicCommands/RenderMeshCommand.h"
#include "../ShaderResources.h"
#include "../Console.h"
#include "../ModelSprite/Sprite.h"
#include "../ModelSprite/ModelFactory.h"
#include "../MainSingleton.h"
#include "../AssetsTab.h"
#include "../../GraphicsEngine/Shader.h"
#include "Transform.h"

#include "GameObject.h"

#include <ImGui/imgui.h>
#include <codecvt>
#include <iostream>
#include <Windows.h>
#include <filesystem>

namespace fs = std::filesystem;

SpriteRenderer::SpriteRenderer(Sprite* aSprite, Material aMaterial) : myMaterial(aMaterial), mySprite(aSprite)
{
    Component();

    myMaterial.SetColor(CommonUtilities::Vector4<float>(1, 1, 1, 1));
}

SpriteRenderer::SpriteRenderer(const SpriteRenderer& aSprite) : myMaterial(aSprite.myMaterial), mySprite(aSprite.mySprite) {
    Component();
}

SpriteRenderer::SpriteRenderer()
{
    Component();

    myMaterial = Material(L"Assets/Shaders/Sprite_UnLit_PS.hlsl", L"Assets/Shaders/Sprite_VS.hlsl");
    myMaterial.SetColor(CommonUtilities::Vector4<float>(1, 1, 1, 1));
}

SpriteRenderer::~SpriteRenderer() = default;

void SpriteRenderer::SetCastShadow(const bool aValue)
{
    myCastShadow = aValue;
}

void SpriteRenderer::LateUpdateEditor(float /*aDeltaTime*/)
{
    if (mySprite)
    {
        Draw();
    }
}

void SpriteRenderer::Draw()
{
    GraphicsEngine::GetRHI()->SetInputLayout(mySprite->myInputLayout);

    ObjectBuffer& objectBuffer = gameObject->transform->GetObjBuffer();
    objectBuffer.albedo = myMaterial.GetColor();

    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBuffer);
    GraphicsEngine::Get().Rendersprite(mySprite, myMaterial);
}

void SpriteRenderer::Destroy()
{
    std::cout << "Destroy" << std::endl;
}

Sprite*& SpriteRenderer::GetSprite()
{
    return mySprite;
}

Material& SpriteRenderer::GetMaterial()
{
    return myMaterial;
}

void SpriteRenderer::RenderImGUI()
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

    ImGui::Dummy(ImVec2(12.0f, 0.f));
    ImGui::SameLine();

    if (AssetsTab::payloadExt == ".dds" || AssetsTab::payloadExt == ".png")
    {
        ImGui::PushStyleColor(0, ImVec4(1.f, 0.843f, 0.f, 1.f));
    }

    if (!myMaterial.GetPSShader()->GetDiffusePath().empty())
    {
        if (ImGui::Button(myMaterial.GetPSShader()->GetDiffuseName().c_str()))
            ImGui::OpenPopup("Texture");
    }
    else
    {
        if (ImGui::Button("<None>"))
            ImGui::OpenPopup("Texture");
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
    ImGui::Text("Texture");

    if (ImGui::BeginPopup("Texture"))
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

                ID3D11Resource** tex = nullptr;

                HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), path.c_str(), tex, mySprite->aTexture.GetAddressOf());

                if (FAILED(result))
                {
                    PrintC(Console::TypeOfMessage::Error, "Unable to initiate                    a Texture at path");
                }
            }
        }
        ImGui::EndPopup();
    }

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
}

