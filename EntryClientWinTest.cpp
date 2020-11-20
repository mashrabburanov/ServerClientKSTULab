//Client
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
	wc.lpszClassName = L"Client";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	if (hwnd = CreateWindow(wc.lpszClassName, L"Client", WS_OVERLAPPEDWINDOW, 0, 0, 600, 600, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
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
			if (hMapObject = OpenFileMappingA(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, "myServerMapFile"); hMapObject == NULL)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü îòêðûòü îáúåêò îòîáðàæåíèÿ", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (lpMap = MapViewOfFile(hMapObject, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 1024); lpMap == NULL)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü îòîáðàçèòü ôàéë íà ïàìÿòü", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, "myServerEvent"); hEvent == NULL)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü ñîçäàòü ñîáûòèå", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventGetType = OpenEventA(EVENT_ALL_ACCESS, FALSE, "myServerEventGetType"); hEventGetType == NULL)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü ñîçäàòü ñîáûòèå", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventDispatchComplete = OpenEventA(EVENT_ALL_ACCESS, FALSE, "myServerEventDispatchComplete"); hEventDispatchComplete == NULL)
			{
				MessageBoxA(nullptr, "Íå ïîëó÷èëîñü ñîçäàòü ñîáûòèå", "Îøèáêà", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventClose = OpenEventA(EVENT_ALL_ACCESS, FALSE, "myServerEventClose"); hEventClose == NULL)
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
				UnmapViewOfFile(lpMap); CloseHandle(hMapObject); CloseHandle(hEvent); CloseHandle(hEventGetType); CloseHandle(hEventDispatchComplete); CloseHandle(hEventClose);
			}
			PostQuitMessage(EXIT_SUCCESS);
		}
		return 0;
	}

	if (DWORD retCode = WaitForSingleObject(hEventClose, 1); retCode == WAIT_OBJECT_0)
		PostQuitMessage(EXIT_SUCCESS);

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
