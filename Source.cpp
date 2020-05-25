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
	STARTUPINFO si;															// ��������� ��� �������� �������� �������.
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION childProcessInfo;									// ��������� ��� �������� �������� �������.
	ZeroMemory(&childProcessInfo, sizeof(childProcessInfo));
	HANDLE handler;
	char buffer[20];
	int bufferSize = sizeof(buffer);
	string message;
	cout << "1st User\n\n";
	HANDLE childSemaphore = CreateSemaphoreA(NULL, 0, 1, "childSemaphore");
	handler = CreateFile(
		name.c_str(),														// ��� ������������ �����.
		GENERIC_READ | GENERIC_WRITE,										// ��� ������� � �����.
		0,																	// ��������� ����������� �������.
		NULL,																// �������� ������ �����.
		OPEN_EXISTING,														// ����� ������������.
		FILE_ATTRIBUTE_NORMAL,												// ����������� ����� ������.
		NULL																// ��������� ����� �������.
	);
	CreateProcess(
		path,																// ��� ������������ ������.
		(LPSTR)" Client",												    // ��������� ��������� ������.
		NULL,																// ����������� ��������� ������ ��� ������ ����������.
		NULL,																// ����������� ��������� ������ ��� ������� ������ ���������� �����������.
		FALSE,																// ���� ������������ �� �������� ������������� ������.
		CREATE_NEW_CONSOLE,													// ����� ������� �������� ����� ������� ������ ����, ����� ������������ ������������. 
		NULL,																// ��������� �� ���� �����. ���� ����� ��� ������ ���������� ���=�������� � ���� ����� � ������� ����������.
		NULL,																// ��������� ������� ���� � �������.
		&si,																// ������������ ��� ��������� ������� ��������, �������� ������������ ���� � ���������.
		&childProcessInfo													// ��������� PROCESS_INFORMATION � ����������� � ��������. ����� ��������� Windows.
	);

	SetCommMask(handler, EV_RXCHAR);										// ������������� ����� �� ������� �����.
	SetupComm(handler, 1500, 1500);											// �������������� ���������������� ��������� ��� ��������� ���������� (����������, ����� �����-������)
	COMMTIMEOUTS CommTimeOuts;												// ���������, ��������������� ��������� ��������� ����������������� �����.
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;							// M����������� ����� ��� ��������� ����� ������������ ���� �������� �� ����� �����.
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;							// ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
	CommTimeOuts.ReadTotalTimeoutConstant = time_;						// ���������, ������������, ����� ��������� ������ (������������) ������ ������� ������� ��� �������� ������.
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;							// ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
	CommTimeOuts.WriteTotalTimeoutConstant = time_;						// ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
	if (!SetCommTimeouts(handler, &CommTimeOuts))
	{
		CloseHandle(handler);
		handler = INVALID_HANDLE_VALUE;
		return;
	}
	DCB ComDCM;																// ���������, ��������������� �������� ��������� ����������������� �����. 
	memset(&ComDCM, 0, sizeof(ComDCM));										// ��������� ������ ��� ���������.
	ComDCM.DCBlength = sizeof(DCB);											// ������ �����, � ������, ���������.
	GetCommState(handler, &ComDCM);											// ��������� ������ � ������� ���������� ����������� �������� ��� ���������� ����������.
	ComDCM.BaudRate = DWORD(9600);											// �������� �������� ������.
	ComDCM.ByteSize = 8;													// ���������� ����� �������������� ��� � ������������ � ����������� ������.
	ComDCM.Parity = NOPARITY;												// ���������� ����� ����� �������� �������� (��� ��������� �����������).
	ComDCM.StopBits = ONESTOPBIT;											// ������ ���������� �������� ��� (���� ���). 
	ComDCM.fAbortOnError = TRUE;											// ������ ������������� ���� �������� ������/������ ��� ������������� ������.
	ComDCM.fDtrControl = DTR_CONTROL_DISABLE;								// ������ ����� ���������� ������� ��� ������� DTR.
	ComDCM.fRtsControl = RTS_CONTROL_DISABLE;								// ������ ����� ���������� ������� ��� ������� RTS.
	ComDCM.fBinary = TRUE;													// �������� �������� ����� ������.
	ComDCM.fParity = FALSE;													// �������� ����� �������� ��������.
	ComDCM.fInX = FALSE;													// ������ ������������� XON/XOFF ���������� ������� ��� ������.
	ComDCM.fOutX = FALSE;													// ������ ������������� XON/XOFF ���������� ������� ��� ��������.
	ComDCM.XonChar = 0;														// ������ ������ XON ������������ ��� ��� ������, ��� � ��� ��������.
	ComDCM.XoffChar = (unsigned char)0xFF;									// ������ ������ XOFF ������������ ��� ��� ������, ��� � ��� ��������.
	ComDCM.fErrorChar = FALSE;												// ������ ������, �������������� ��� ������ �������� � ��������� ���������.
	ComDCM.fNull = FALSE;													// ��������� �� ������������� ������ �������� � ������� �������� �� ������ ���������� ����� ErrorChar.
	ComDCM.fOutxCtsFlow = FALSE;											// �������� ����� �������� �� �������� CTS.
	ComDCM.fOutxDsrFlow = FALSE;											// �������� ����� �������� �� �������� DSR.
	ComDCM.XonLim = 128;													// ������ ����������� ����� �������� � �������� ������ ����� �������� ������� XON.
	ComDCM.XoffLim = 128;													// ���������� ������������ ���������� ���� � �������� ������ ����� �������� ������� XOFF.
	if (!SetCommState(handler, &ComDCM))
	{
		CloseHandle(handler);
		handler = INVALID_HANDLE_VALUE;
		return;
	}
	while (true)
	{
		ReleaseSemaphore(childSemaphore, 1, //�� ������� �������� �������
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
		WaitForSingleObject(childSemaphore, INFINITE);						// �������� �������� � ������������ ���������.
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
