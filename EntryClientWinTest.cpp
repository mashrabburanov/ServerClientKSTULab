//Client
/*для современных стилей для кнопок етк*/
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>

/*функция winProc для обработки сообщений*/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/*точка входа*/
int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	//стандартная инициация
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
	//ивенты и файла отображения
	static HANDLE hMapObject = nullptr, hEventDispatchComplete = nullptr, hEventStartDispatch = nullptr, hEventSetType = nullptr, hEventClose = nullptr;
	static LPVOID lpMap = nullptr; LPINT lpIntMap = nullptr;

	//идентификатор таймера №1 (пока только один)
	static const UINT IDT_TIMER1 = 1;

	//тип передаваемый на сервер я предпологаю, что тип будет задаваться в кнопке или еще в чем нибудь
	//там же будет передаваться на сервер, код для передачи указан ниже
	static INT type;
	//переданый массив
	static INT iBuff[256];

	switch (uMsg)
	{
		//при создании
		case WM_CREATE:
		{
			//открываем события/отображения етк
			if (hMapObject = OpenFileMappingA(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, "myServerMapFile"); hMapObject == NULL)
			{
				MessageBoxA(nullptr, "Не получилось открыть объект отображения", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (lpMap = MapViewOfFile(hMapObject, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 1024); lpMap == NULL)
			{
				MessageBoxA(nullptr, "Не получилось отобразить файл на память", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventStartDispatch = OpenEventA(EVENT_ALL_ACCESS, FALSE, "myServerEventStartDispatch"); hEventStartDispatch == NULL)
			{
				MessageBoxA(nullptr, "Не получилось создать событие", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventSetType = OpenEventA(EVENT_ALL_ACCESS, FALSE, "myServerEventGetType"); hEventSetType == NULL)
			{
				MessageBoxA(nullptr, "Не получилось создать событие", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventDispatchComplete = OpenEventA(EVENT_ALL_ACCESS, FALSE, "myServerEventDispatchComplete"); hEventDispatchComplete == NULL)
			{
				MessageBoxA(nullptr, "Не получилось создать событие", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (hEventClose = OpenEventA(EVENT_ALL_ACCESS, FALSE, "myServerEventClose"); hEventClose == NULL)
			{
				MessageBoxA(nullptr, "Не получилось создать событие", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			if (UINT uRetCode = SetTimer(hWnd, IDT_TIMER1, 10, (TIMERPROC)NULL); uRetCode == 0)
			{
				MessageBoxA(NULL, "Не получилось создать таймер", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
			}
		}
		return 0;
		
		//При уничтожении клиента/сервера
		case WM_DESTROY:
		{
			//Закрываем все
			if (lpMap != nullptr)
				UnmapViewOfFile(lpMap);
			if (hEventStartDispatch != nullptr)
				CloseHandle(hEventStartDispatch);
			if (hEventSetType != nullptr)
				CloseHandle(hEventSetType);
			if (hEventDispatchComplete != nullptr)
				CloseHandle(hEventDispatchComplete);
			if (hEventClose != nullptr)
				CloseHandle(hEventClose);
			if (hMapObject != nullptr)
				CloseHandle(hMapObject);

			//уничтожаем таймер
			KillTimer(hWnd, IDT_TIMER1);
			PostQuitMessage(EXIT_SUCCESS);
		}
		return 0;

		//таймер
		case WM_TIMER:
		{
			//если будут добавлены еще таймеры
			switch (wParam)
			{
				//первый таймер
				case IDT_TIMER1:
				{
					//если ивент Закрыть включен, то закрываем окно
					if (DWORD retCode = WaitForSingleObject(hEventClose, 1); retCode == WAIT_OBJECT_0)
						PostQuitMessage(EXIT_SUCCESS);
				}
				break;
			}
		}
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//+++++++++++++++++++++++++++++++++++++++ВНИМАНИЕ!!!+++++++++++++++++++++++++++++++++++++++

/*код для кнопки передачи типа функции*/
/*
*	//записываем тип в память
*	*lpIntMap = type;		
*	//вклюаем событие ПередатьТип
*	SetEvent(hEventSetType);	
*/

/*код кнопки для начала передачи данных*/
/*	//Вкл ивент начать передачу
*	SetEvent(HEventStartDispatch);		
*	//Ждем (бесконечно) до включения события ПередачаЗавершена
*	if (DWORD retCode = WaitForSingleObject(hEventDispatchComplete, INFINITE); retCode == WAIT_OBJECT_0)	
*	{
*		//записываем в массив данные
*		LPINT lpIntTemp = lpIntMap;
*		INT size = *lpIntMap++;
*		for (int i = 0; i < size; i++){
*			iBuff = *lpIntTemp++;
*		}
*		ResetEvent(hEventDispatchComplete);
*	}
*/

//+++++++++++++++++++++++++++++++++++++++ВНИМАНИЕ!!!+++++++++++++++++++++++++++++++++++++++
