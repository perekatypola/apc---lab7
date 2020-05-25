#define _SCL_SECURE_NO_WARNINGS
#include <cstdio>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;
const int time_ = 1000;
void Server(char* path)
{
	string name = "COM1";
	STARTUPINFO si;															// Структура для описания процесса клиента.
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION childProcessInfo;									// Структура для описания процесса клиента.
	ZeroMemory(&childProcessInfo, sizeof(childProcessInfo));
	HANDLE handler;
	char buffer[20];
	int bufferSize = sizeof(buffer);
	string message;
	cout << "1st User\n\n";
	HANDLE childSemaphore = CreateSemaphoreA(NULL, 0, 1, "childSemaphore");
	handler = CreateFile(
		name.c_str(),														// Имя открываемого файла.
		GENERIC_READ | GENERIC_WRITE,										// Тип доступа к файлу.
		0,																	// Параметры совместного доступа.
		NULL,																// Атрибуты защиты файла.
		OPEN_EXISTING,														// Режим автосоздания.
		FILE_ATTRIBUTE_NORMAL,												// Асинхронный режим работы.
		NULL																// Описатель файла шаблона.
	);
	CreateProcess(
		path,																// Имя исполняемого модуля.
		(LPSTR)" Client",												    // Параметры командной строки.
		NULL,																// Определение атрибутов защиты для нового приложения.
		NULL,																// Определение атрибутов защиты для первого потока созданного приложением.
		FALSE,																// Флаг наследования от процесса производящего запуск.
		CREATE_NEW_CONSOLE,													// Новый процесс получает новую консоль вместо того, чтобы унаследовать родительскую. 
		NULL,																// Указывает на блок среды. Блок среды это список переменных имя=значение в виде строк с нулевым окончанием.
		NULL,																// Указывает текущий диск и каталог.
		&si,																// Используется для настройки свойств процесса, например расположения окон и заголовок.
		&childProcessInfo													// Структура PROCESS_INFORMATION с информацией о процессе. Будет заполнена Windows.
	);

	SetCommMask(handler, EV_RXCHAR);										// Устанавливаем маску на события порта.
	SetupComm(handler, 1500, 1500);											// Инициализирует коммуникационные параметры для заданного устройства (Дескриптор, буфер ввода-вывода)
	COMMTIMEOUTS CommTimeOuts;												// Структура, характеризующая временные параметры последовательного порта.
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;							// Mаксимальное время для интервала между поступлением двух символов по линии связи.
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;							// Множитель, используемый, чтобы вычислить полный период времени простоя для операций чтения.
	CommTimeOuts.ReadTotalTimeoutConstant = time_;						// Константа, используемая, чтобы вычислить полный (максимальный) период времени простоя для операций чтения.
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;							// Множитель, используемый, чтобы вычислить полный период времени простоя для операций записи.
	CommTimeOuts.WriteTotalTimeoutConstant = time_;						// Константа, используемая, чтобы вычислить полный период времени простоя для операций записи.
	if (!SetCommTimeouts(handler, &CommTimeOuts))
	{
		CloseHandle(handler);
		handler = INVALID_HANDLE_VALUE;
		return;
	}
	DCB ComDCM;																// Структура, характеризующая основные параметры последовательного порта. 
	memset(&ComDCM, 0, sizeof(ComDCM));										// Выделение памяти под структуру.
	ComDCM.DCBlength = sizeof(DCB);											// Задает длину, в байтах, структуры.
	GetCommState(handler, &ComDCM);											// Извлекает данные о текущих настройках управляющих сигналов для указанного устройства.
	ComDCM.BaudRate = DWORD(9600);											// Скорость передачи данных.
	ComDCM.ByteSize = 8;													// Определяет число информационных бит в передаваемых и принимаемых байтах.
	ComDCM.Parity = NOPARITY;												// Определяет выбор схемы контроля четности (Бит честности отсутствует).
	ComDCM.StopBits = ONESTOPBIT;											// Задает количество стоповых бит (Один бит). 
	ComDCM.fAbortOnError = TRUE;											// Задает игнорирование всех операций чтения/записи при возникновении ошибки.
	ComDCM.fDtrControl = DTR_CONTROL_DISABLE;								// Задает режим управления обменом для сигнала DTR.
	ComDCM.fRtsControl = RTS_CONTROL_DISABLE;								// Задает режим управления потоком для сигнала RTS.
	ComDCM.fBinary = TRUE;													// Включает двоичный режим обмена.
	ComDCM.fParity = FALSE;													// Включает режим контроля четности.
	ComDCM.fInX = FALSE;													// Задает использование XON/XOFF управления потоком при приеме.
	ComDCM.fOutX = FALSE;													// Задает использование XON/XOFF управления потоком при передаче.
	ComDCM.XonChar = 0;														// Задает символ XON используемый как для приема, так и для передачи.
	ComDCM.XoffChar = (unsigned char)0xFF;									// Задает символ XOFF используемый как для приема, так и для передачи.
	ComDCM.fErrorChar = FALSE;												// Задает символ, использующийся для замены символов с ошибочной четностью.
	ComDCM.fNull = FALSE;													// Указывает на необходимость замены символов с ошибкой четности на символ задаваемый полем ErrorChar.
	ComDCM.fOutxCtsFlow = FALSE;											// Включает режим слежения за сигналом CTS.
	ComDCM.fOutxDsrFlow = FALSE;											// Включает режим слежения за сигналом DSR.
	ComDCM.XonLim = 128;													// Задает минимальное число символов в приемном буфере перед посылкой символа XON.
	ComDCM.XoffLim = 128;													// Определяет максимальное количество байт в приемном буфере перед посылкой символа XOFF.
	if (!SetCommState(handler, &ComDCM))
	{
		CloseHandle(handler);
		handler = INVALID_HANDLE_VALUE;
		return;
	}
	while (true)
	{
		ReleaseSemaphore(childSemaphore, 1, //на сколько изменять счетчик
			0);
		DWORD NumberOfBytesWritten;
		cout << "Enter message: ";
		cin.clear();
		getline(cin, message);
		if (message == "Quit")
		{	
			TerminateProcess(childProcessInfo.hProcess , 0);
			break;
		}
		int NumberOfBlocks = message.size() / bufferSize + 1;
		WriteFile(handler, &NumberOfBlocks, sizeof(NumberOfBlocks), &NumberOfBytesWritten, NULL);
		int size = message.size();
		WriteFile(handler, &size, sizeof(size), &NumberOfBytesWritten, NULL);

		for (int i = 0; i < NumberOfBlocks; i++)
		{
			message.copy(buffer, bufferSize, i*bufferSize);
			if (!WriteFile(handler, buffer, bufferSize, &NumberOfBytesWritten, NULL))
				cout << "Error.";
		}
		WaitForSingleObject(childSemaphore, INFINITE);						// Ожидание перехода в несигнальное состояние.
	}
	CloseHandle(handler);
	CloseHandle(childSemaphore);
	return;
}

void Client()
{
	HANDLE hMyPipe;
	char buffer[20];
	int bufferSize = sizeof(buffer);
	string message;
	string name = "COM2";
	HANDLE childSemaphore = OpenSemaphoreA(EVENT_ALL_ACCESS, FALSE, "childSemaphore");
	cout << "2nd User\n\n";
	hMyPipe = CreateFile(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	while (true)
	{
		ReleaseSemaphore(childSemaphore, 1, 0);
		DWORD NumberOfBytesRead;
		message.clear();
		int NumberOfBlocks;
		if (!ReadFile(hMyPipe, &NumberOfBlocks, sizeof(NumberOfBlocks), &NumberOfBytesRead, NULL)) break;
		int size;
		if (!ReadFile(hMyPipe, &size, sizeof(size), &NumberOfBytesRead, NULL)) break;
		for (int i = 0; i < NumberOfBlocks; i++)
		{
			if (!ReadFile(hMyPipe, buffer, bufferSize, &NumberOfBytesRead, NULL))
			{
				break;
			}
			message.append(buffer, bufferSize);
		}
		message.resize(size);
		cout << message<< endl;
		WaitForSingleObject(childSemaphore, INFINITE);
	}
	CloseHandle(hMyPipe);
	CloseHandle(childSemaphore);
	return;
}

int main(int argc, char* argv[])
{
	switch (argc) 
	{
	case 1:																	
		Server(argv[0]);
		break;
	default:
		Client();
		break;
	}
}
