#pragma once

#include <vector>

#include <string>

#include <wrl.h>

struct ID3D11ShaderResourceView;
struct ID3D11Resource;

struct FileS
{
	std::string name;
	std::string path;
	std::string fileExtension;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	FileS(std::string aName, std::string aPath, std::string aFileExtension)
	{
		name = aName;
		path = aPath;
		fileExtension = aFileExtension;
	}
};

class AssetsTab
{
public:
	AssetsTab() = default;
	~AssetsTab() = default;

	void Init();

	void Render();

private:
	std::string baseDirectory = "Assets/";
	std::string currentDirectoryPath;
	std::vector<std::string> assetPathList;

	std::vector<FileS> filesInFolder;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myFolderIcon;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myEmptyFileIcon;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myNoPreviewFBXIcon;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myHlslIcon;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myHlsliIcon;

	ID3D11Resource** texture = nullptr;

	bool hasPayload = false;

	bool opnedFolder = false;

public:
	static inline std::string payload = "";
	static inline std::string payloadExt = "";
};