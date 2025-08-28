#ifndef IMPLEMENT_MAIN
#define IMPLEMENT_MAIN

#include <exception>

#include "Gameworld.h"

#include <filesystem>

#include <stdio.h>

static int GuardedMain()
{
    GameWorld MV;
    MV.Initialize();
    return MV.Run();
}

#ifdef NONWIN32

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

void ConsoleInit()
{
    char selfdir[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, selfdir, MAX_PATH);
    std::filesystem::path workingDir(selfdir);
    workingDir.remove_filename();
    std::filesystem::current_path(workingDir);

    // Redirect stdout and stderr to the console.
    FILE *consoleOut;
    FILE *consoleErr;
    AllocConsole();
    freopen_s(&consoleOut, "CONOUT$", "w", stdout); // NOLINT(cert-err33-c)
    setvbuf(consoleOut, nullptr, _IONBF, 1024);     // NOLINT(cert-err33-c)
    freopen_s(&consoleErr, "CONOUT$", "w", stderr); // NOLINT(cert-err33-c)
    setvbuf(consoleErr, nullptr, _IONBF, 1024);     // NOLINT(cert-err33-c)
    SetConsoleOutputCP(CP_UTF8);

    const HWND consoleWindow = GetConsoleWindow();
    RECT consoleSize;
    GetWindowRect(consoleWindow, &consoleSize);
    MoveWindow(consoleWindow, consoleSize.left, consoleSize.top, 1280, 720, true);
}

static int APIENTRY wWinMain(_In_ const HINSTANCE hInstance,
    _In_opt_ const HINSTANCE hPrevInstance,
    _In_ const LPWSTR    lpCmdLine,
    _In_ const int       nCmdShow) {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    try
	{
        ConsoleInit();
		return GuardedMain();
	}
    catch(const std::exception& /*e*/)
    {
        return -1;
    }
}
#else
int main(int argc, char *args[])
{
    try
    {
        return GuardedMain();
    }
    catch (const std::exception & /*e*/)
    {
        return -1;
    }
}
#endif

#endif