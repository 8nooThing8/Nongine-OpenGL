#pragma once

#include <vector>
#include <string>

#include "memory"

struct ID3D11ShaderResourceView;

class Console
{
public:
	enum TypeOfMessage
	{
		Error,
		Warning,
		Log
	};

	Console() = default;
	~Console() = default;

	static void Init();


	struct Message
	{
	public:
		friend class Console;

		TypeOfMessage messageType;
		std::string message;

		Message(const TypeOfMessage aType, std::string aMessage, const unsigned aRowNumber, std::string aFile) : messageType(aType), message(aMessage), line(aRowNumber), file(aFile)
		{

		}

	private:
		unsigned line;
		std::string file;
	};

	static __forceinline void AddOutput(Message aMessage)
	{
		aQueuedMessages.push_back(std::move(aMessage));
	}

	static void Render();

private:
	static inline std::vector<Message> aQueuedMessages = std::vector<Message>();

	static inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myWaningSymbol = nullptr;
	static inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myErrorSymbol = nullptr;
	static inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myLogSymbol = nullptr;
};

#define PrintC(type, message) Console::AddOutput(Console::Message(type, message, __LINE__, __FILE__))

#define PrintNoLine(type, message) Console::AddOutput(Console::Message(type, message, -1, ""))

#define PrintLog(message) Console::AddOutput(Console::Message(Console::TypeOfMessage::Log, message, __LINE__, __FILE__))
#define PrintError(message) Console::AddOutput(Console::Message(Console::TypeOfMessage::Error, message, __LINE__, __FILE__))