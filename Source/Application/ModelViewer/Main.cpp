#include <iostream>

#include <cstdio>
#include <exception>

#include "Application.h"
#include "ModelViewer.h"

#include "Windows.h"

//#include "Utilities/StringHelpers.h"

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

int GuardedMain()
{
    // Redirect stdout and stderr to the console.
    FILE* consoleOut;
	FILE* consoleErr;
    AllocConsole();
    freopen_s(&consoleOut, "CONOUT$", "w", stdout);  // NOLINT(cert-err33-c)
    setvbuf(consoleOut, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)
	freopen_s(&consoleErr, "CONOUT$", "w", stderr);  // NOLINT(cert-err33-c)
    setvbuf(consoleErr, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)

    /*
     * A note about strings:
     * Strings in C++ are terrible things. They come in a variety of formats which causes issues.
     * Many modern libraries expect UTF-8 (ImGui, FMOD, FBX SDK, etc) but Windows defaults to UTF-16LE
     * which is not compatible 1=1.
     *
     * To avoid weird problems with compatibility, mangled characters and other problems it is highly
     * recommended to store everything as UTF-8 encoded strings whenever possible. This means that
     * when we store i.e. the name of a Model, Level, Sound File, etc we do so in a UTF-8 formatted
     * std::string, and when we need to communicate with the Windows (or DirectX) API we need to use
     * std::wstring which represents a UTF-16LE string.
     *
     * There are functions available for conversion between these formats in the str namespace in
     * StringHelpers.h.
     *
     * The provided Logging library expects UTF-8 format strings which should provide minimal headaches
     * for any involved situation. For anything non-unicode (like non swedish signs, accents, etc) you
     * can just use normal strings as you would anywhere since UTF-8 is backwards compatible with ASCii
     *
     * SetConsoleOutputCP(CP_UTF8) tells the Windows Console that we'll output UTF-8. This DOES NOT
     * affect file output in any way, that's a whole other can of worms. But if you always write and
     * read your strings in the same format, and always treat them as byte blocks, you'll be fine.
     */
    SetConsoleOutputCP(CP_UTF8);
    //MVLOG(Log, "ModelViewer starting...");

    std::cout << "ModelViewer starting..." << std::endl;

	const HWND consoleWindow = GetConsoleWindow();
    RECT consoleSize;
    GetWindowRect(consoleWindow, &consoleSize);
    MoveWindow(consoleWindow, consoleSize.left, consoleSize.top, 1920, 1080, true);

    constexpr SIZE windowSize = { 1920, 1080 };

    constexpr LPCWSTR windowTitle = L"TGP ModelViewer";

    ModelViewer MV;
    MV.Initialize(windowSize, WinProc, windowTitle);
    return MV.Run();
}

int APIENTRY wWinMain(_In_ const HINSTANCE hInstance,
                      _In_opt_ const HINSTANCE hPrevInstance,
                      _In_ const LPWSTR    lpCmdLine,
                      _In_ const int       nCmdShow){
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    try
	{
		return GuardedMain();
	}
    catch(const std::exception& e)
    {
        std::string message = e.what();
       /* if(!str::is_valid_utf8(message))
        {
	        message = str::acp_to_utf8(message);
        }*/
        //MVLOG(Error, "Exception caught!\n{}", message);
        return -1;
    }

    return 0;
}

//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool shick = false;

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ const UINT uMsg, _In_ const WPARAM wParam, _In_ const LPARAM lParam)
{
   /* if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;*/

    //for (auto& gameobject : activeScene.gameObjects)
    //{
    //    MeshRenderer* comp = gameobject->GetComponent<MeshRenderer>();

    //    if (comp)
    //    {
    //        comp->GetMaterial().CompileShader();
    //    }
    //}

    if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
    {
        PostQuitMessage(0);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}