#include "../Engine.pch.h"

#include <ImGui/imgui.h>

#include "Console.h"

#include "../Engine/GraphicsEngine/GraphicsEngine.h"
#include "../Engine/GraphicsEngine/DDSTextureLoader11.h"



void Console::Init()
{
	ID3D11Resource** texture = nullptr;

	DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Sprites/ErrorSymbol.dds", texture, myErrorSymbol.GetAddressOf());
	DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Sprites/WarningSymbol.dds", texture, myWaningSymbol.GetAddressOf());
	DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetRHI()->myDevice.Get(), L"Assets/Sprites/LogSymbol.dds", texture, myLogSymbol.GetAddressOf());
}

void Console::Render()
{
	ImGui::Begin("Console");

	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(80, 80, 80, 255));

	if (ImGui::Button("Clear"))
	{
		aQueuedMessages.clear();
	}

	ImGui::PopStyleColor();

	std::string textMessage;

	std::reverse(aQueuedMessages.begin(), aQueuedMessages.end());

	int currentMessage = 0;

	for (auto& message : aQueuedMessages)
	{
		if (currentMessage >= 1000)
			break;

		ID3D11ShaderResourceView* symboltodisplay = nullptr;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tesrt;

		switch (message.messageType) {
		case TypeOfMessage::Error:
			symboltodisplay = myErrorSymbol.Get();
			textMessage = "An error has been encountered on line: " + std::to_string(message.line) + "  Filepath: " + message.file + "\n" + message.message;
			break;
		case TypeOfMessage::Warning:
			symboltodisplay = myWaningSymbol.Get();
			textMessage = "A warning has been called on line: " + std::to_string(message.line) + "  Filepath: " + message.file + "\n" + message.message;
			break;
		case TypeOfMessage::Log:
			symboltodisplay = myLogSymbol.Get();
			textMessage = "Line: " + std::to_string(message.line) + "  Filepath: " + message.file + "\n" + message.message;
			break;
		}

		ImVec2 textSize = ImGui::CalcTextSize(textMessage.c_str());
		float padding = 8.0f;

		ImVec2 cursorStartPos = ImGui::GetCursorScreenPos();
		float rectWidth = ImGui::GetContentRegionAvail().x;

		ImGui::GetWindowDrawList()->AddRect(cursorStartPos, ImVec2(cursorStartPos.x + rectWidth, cursorStartPos.y + textSize.y + 2 * padding), IM_COL32(100, 100, 100, 255));

		ImGui::SetCursorScreenPos(ImVec2(cursorStartPos.x + padding, cursorStartPos.y + padding - 2));

		ImGui::Image((ImTextureID)symboltodisplay, ImVec2(30, 30));

		ImGui::SetCursorScreenPos(ImVec2(cursorStartPos.x + padding + 38, cursorStartPos.y + padding));

		ImGui::TextWrapped(textMessage.c_str());

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding);

		currentMessage++;
	}

	std::reverse(aQueuedMessages.begin(), aQueuedMessages.end());

	ImGui::End();
}