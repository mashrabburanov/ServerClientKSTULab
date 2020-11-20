//Server
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	MSG msg{};
	HWND hwnd{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"Server";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	if (hwnd = CreateWindow(wc.lpszClassName, L"Server", WS_OVERLAPPEDWINDOW, 0, 0, 600, 600, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HANDLE hMapObject = nullptr, hEventDispatchComplete = nullptr, hEvent = nullptr, hEventGetType = nullptr, hEventClose = nullptr;
	static HANDLE hEvents[] = { hEvent, hEventGetType };
	static LPVOID lpMap = nullptr; LPINT lpIntMap = nullptr;

	static INT int_buff[256];
	static INT type;

	switch (uMsg)
	{
		case WM_CREATE:
		{
			if (hMapObject = CreateFileMappingA((HANDLE)0xFFFFFFFF, nullptr, PAGE_READWRITE, 0, 1024, "myServerMapFile"); hMapObject == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü ñîçäàòü îáúåêò îòîáðàæåíèÿ", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (lpMap = MapViewOfFile(hMapObject, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 1024); lpMap == NULL)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü îòîáðàçèòü ôàéë íà ïàìÿòü", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEvent = CreateEventA(NULL, FALSE, FALSE, "myServerEvent"); hEvent == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü ñîçäàòü ñîáûòèå", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventGetType = CreateEventA(NULL, FALSE, FALSE, "myServerEventGetType"); hEventGetType == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü ñîçäàòü ñîáûòèå", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventDispatchComplete = CreateEventA(NULL, FALSE, FALSE, "myServerEventDispatchComplete"); hEventDispatchComplete == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü ñîçäàòü ñîáûòèå", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventClose = CreateEventA(NULL, FALSE, FALSE, "myServerEventClose"); hEventClose == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü ñîçäàòü ñîáûòèå", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
		}
		return 0;

		case WM_DESTROY:
		{
			if (lpMap != nullptr && hMapObject != nullptr && hEvent != nullptr && hEventGetType != nullptr && hEventDispatchComplete != nullptr && hEventClose != nullptr)
			{
				SetEvent(hEventClose);
				UnmapViewOfFile(lpMap); CloseHandle(hMapObject); CloseHandle(hEvent); CloseHandle(hEventGetType); CloseHandle(hEventDispatchComplete); CloseHandle(hEventClose);
			}
			PostQuitMessage(EXIT_SUCCESS);
		}
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
