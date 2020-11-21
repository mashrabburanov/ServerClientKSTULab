//Server
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
	//ивенты и файла отображения
	static HANDLE hMapObject = nullptr, hEventDispatchComplete = nullptr, hEventStartDispatch = nullptr, hEventGetType = nullptr, hEventClose = nullptr;
	static HANDLE hEvents[] = { hEventDispatchComplete, hEventGetType };
	static LPVOID lpMap = nullptr; LPINT lpIntMap = nullptr;

	//идентификатор таймера №1 (пока только один)
	static const UINT IDT_TIMER1 = 1;

	//тип передаваемый принимаемый с клиента, обработка принятия происходит в WM_TIMER
	static INT type;
	//массив для сгенерированных чисел
	static INT iBuff[256];
	//размер передаваемого массива
	static INT size = 0;

	switch (uMsg)
	{
		//при создании
		case WM_CREATE:
		{
			//создаем объект отображение на виртуальную память размером 1024 байт(0xFFFFFFFF - позволяет сразуотображать на память файл без его создания на диске)
			if (hMapObject = CreateFileMappingA((HANDLE)0xFFFFFFFF, nullptr, PAGE_READWRITE, 0, 1024, "myServerMapFile"); hMapObject == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Не получилось создать объект отображения", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			//отображаем объект на память
			if (lpMap = MapViewOfFile(hMapObject, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 1024); lpMap == NULL)
			{
				MessageBoxA(nullptr, "Не получилось отобразить файл на память", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			//создаем событие НачалоПередачи
			if (hEventStartDispatch = CreateEventA(NULL, FALSE, FALSE, "myServerEventStartDispatch"); hEventStartDispatch == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Не получилось создать событие", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			//создаем событие ПолучитьТип
			if (hEventGetType = CreateEventA(NULL, FALSE, FALSE, "myServerEventGetType"); hEventGetType == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Не получилось создать событие", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			//создаем событие ПередачаЗавершена
			if (hEventDispatchComplete = CreateEventA(NULL, FALSE, FALSE, "myServerEventDispatchComplete"); hEventDispatchComplete == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Не получилось создать событие", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			//Создаем событие Закрыть
			if (hEventClose = CreateEventA(NULL, FALSE, FALSE, "myServerEventClose"); hEventClose == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBoxA(nullptr, "Не получилось создать событие", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
			//Инициируем таймер, каждые десять милисекунд будет приходить сообщение WM_TIMER
			if (UINT uRetCode = SetTimer(hWnd, IDT_TIMER1, 10, (TIMERPROC)NULL); uRetCode == 0)
			{
				MessageBoxA(NULL, "Не получилось создать таймер", "Ошибка", MB_OK);
				PostQuitMessage(EXIT_FAILURE);
				return GetLastError();
			}
		}
		return 0;

		//При уничтожении сервера
		case WM_DESTROY:
		{
			//Устанавливаем событие закрытие сервера
			SetEvent(hEventClose);

			//Закрываем все
			if (lpMap != nullptr)
				UnmapViewOfFile(lpMap);
			if (hEventStartDispatch != nullptr)
				 CloseHandle(hEventStartDispatch); 
			if (hEventGetType != nullptr)
				CloseHandle(hEventGetType); 
			if (hEventDispatchComplete != nullptr)
				CloseHandle(hEventDispatchComplete); 
			if (hEventClose != nullptr)
				CloseHandle(hEventClose);
			if (hMapObject != nullptr)
				CloseHandle(hMapObject);

			//Уничтожаем таймер
			KillTimer(hWnd, IDT_TIMER1);
			PostQuitMessage(EXIT_SUCCESS);
		}
		return 0;
		
		//при срабатывании таймера
		case WM_TIMER:
		{
			//если будем добавлять еще таймеры
			switch (wParam)
			{
				//таймер №1
				case IDT_TIMER1:
				{
					//Проверка 1 милисекунду включено ли событие ПолучениеТипа
					if (DWORD retCode = WaitForSingleObject(hEventGetType, 1); retCode == WAIT_OBJECT_0)
					{
						//если да, то записываем в переменную тип, из начала отображенного файла, переданный тип
						type = *lpIntMap;
						//выключаем ивент
						ResetEvent(hEventGetType);
					}
					if (DWORD retCode = WaitForSingleObject(hEventStartDispatch, 1); retCode == WAIT_OBJECT_0)
					{
						//+++++++++++++++++++++++++++++++++++++++ВНИМАНИЕ!!!+++++++++++++++++++++++++++++++++++++++

						/*запись в отображенный файл*/
						/*	здесь будет генерация в массив с учетом типа, далее передача
						*	LPINT lpIntTemp = lpIntMap;
						*	//передаем первым числом размер массива
						*	*lpIntTemp++ = размер_сгенерированного_массива; 
						*	//далее помещаем массив в память
						*	for (int i = 0; i < размер_сгенерированного_массива; i++)
						*	{
						*		*lpIntTemp++ = iBuff[i];
						*	}
						*	ResetEvent(hEventStartDispatch);
						*	//говорим что передачу окончили
						*	SetEvent(hEventDispatchComplete);
						*/

						//+++++++++++++++++++++++++++++++++++++++ВНИМАНИЕ!!!+++++++++++++++++++++++++++++++++++++++
					}
				}
				break;
			}
		}
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
