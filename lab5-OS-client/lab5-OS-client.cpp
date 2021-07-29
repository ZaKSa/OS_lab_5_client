#include <Windows.h>
#include <iostream>
#include <string>

using namespace std;

const int PATH_SIZE = 80,
PASSWORD_SIZE = 4,
MAX_COUNT = 3,
EVENT_COUNT = 31;


HANDLE hEvent[EVENT_COUNT]; //создали массив событий
HANDLE hSemaphore;
LPCWSTR namesEvent[EVENT_COUNT] = { L"0",L"1",L"2",L"3",L"4", L"5", L"6", L"7",L"8",L"9",
L"!",L"@",L"#",L"$",L"%",L",",L"&",L"*",L"(",L")",L"=",L"+", L"№", L";", L"%",  L":",
L"?", L"<", L">", L"-",L"." };

string symbol = "0123456789!@#$%,&*()=+№;%:?<>-";

LPCWSTR start = L"start";
LPCWSTR end = L"end";


string correct_characters(string str) {
	string result = "";
	for (int i = 0;i < str.length(); i++)
		for (int j = 0; j < symbol.length();j++)
			if (str[i] == symbol[j])
				result += str[i];
	return result;
}

int main()
{
	hSemaphore = OpenSemaphore(SYNCHRONIZE,//Полученный идентификатор можно будет использовать в любых функциях ожидания события
		FALSE, //Если этот параметр равен TRUE, идентфикатор может наследоваться дочерними процессами, FALSE не может
		L"hSemaphore");

	if (hSemaphore == NULL) {
		cerr << "Error of open semaphore" << endl
			<< "Error code> " << GetLastError() << endl;
		return 0;
	}

	string in;
	int number = 0;
	HANDLE hNamedPipe;
	wchar_t pipeName[] = L"\\\\.\\pipe\\NamedPipe";


	char msg[4] = { '0','0','0','0' };


	DWORD dwBytes;

	bool ok = true;
	while (ok) {
		cout << "Enter password: ";
		cin >> in;
		in = correct_characters(in);
		cout << "Correct symbols> " << in << endl;
		for (int i = 0; i < in.length();i++)
			msg[i] = in[i];
		hEvent[30] = OpenEvent(EVENT_MODIFY_STATE, FALSE, namesEvent[30]);
		SetEvent(hEvent[30]);
		hNamedPipe = CreateFile(TEXT("\\\\.\\pipe\\mynamedpipe"),
			GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL);

		WriteFile(hNamedPipe, msg, sizeof(msg), &dwBytes, NULL);
		ReadFile(hNamedPipe, &msg, sizeof(msg), &dwBytes, NULL);
		if (msg[0] == '!') {
			cout << "Wrong password. Wait 40 seconds" << endl;
			Sleep(40000);
		}
		else {
			number = (int)msg[0];
			cout << "Correct. You are #" << number << endl;
			ok = false;
		}
		ResetEvent(hEvent[30]);
		DisconnectNamedPipe(hNamedPipe);
		CloseHandle(hNamedPipe);
	}

	WaitForSingleObject(hSemaphore, INFINITE);

	do {
		cin >> in;
		in = correct_characters(in);
		cout << "correct symbols: " << in << endl;
		for (int k = 0;k < in.length();k++)
		{
			for (int i = 0; i < 30;i++)
				if (in[k] == symbol[i])
				{
					hEvent[i] = OpenEvent(EVENT_MODIFY_STATE,//EVENT_MODIFY_STATE - обеспечивает возможность использования дескриптора объекта события в функциях SetEvent и ResetEvent,
						//изменяющих состояние объекта события;
						FALSE,//определяет возможность наследования возвращаемого дескриптора. 
						//Если этот аргумент имеет значение TRUE, то процесс, созданный функцией CreateProcess, может наследовать дескриптор.
						namesEvent[i]);


					SetEvent(hEvent[i]);

					hNamedPipe = CreateFile(TEXT("\\\\.\\pipe\\mynamedpipe"),
						GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL);


					msg[0] = number;
					WriteFile(hNamedPipe, msg, sizeof(msg), &dwBytes, NULL);
					DisconnectNamedPipe(hNamedPipe);
					CloseHandle(hNamedPipe);

					if (i == 29) {
						for (int i = 0; i < EVENT_COUNT; i++)
							CloseHandle(hEvent[i]);
						return 0;
					}

				}
			Sleep(10);
			for (int i = 0; i < EVENT_COUNT;i++)
				ResetEvent(hEvent[i]);
		}
	} while (true);
	for (int i = 0;i < EVENT_COUNT;i++)
		CloseHandle(hEvent[i]);
}
