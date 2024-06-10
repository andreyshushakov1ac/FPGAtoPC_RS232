/*
RS232FpgaTx.v - передатчик

gcc RS232PCrx.c -o RS232PCrx
./RS232PCrx.c

1)На плате ставим на свичах (0-7) число 
2)Компилируем эту программу -> данные не передаются 
3)Включаем свич10 (разрешение передачи данных)
И получаем символ по установленному коду (ASCII)

/// 
Два момента по работе: 
1. Передача не должна осуществляться раньше приёма
2. в ф-ции ReadFile должно корректно указываться кол-во байт к прочтению

*/


#include <windows.h> //подключение ф-ций прикладного интерфейса
#include <stdio.h>

int main (){

//бронируем место 	
HANDLE hComm; //под дескриптор
char buffer[100]; //под буфер данных
DWORD bytesRead = 0;//под кол-во полученных байт //DWORD 32-разрядное целое число без знака. Диапазон от 0 до 4294967295 десятичных знаков.
	
/// открывается порт СОМ_s для чтения, используется синхронный режим обмена.
// Проверяется успешность открытия порта
hComm = CreateFile ("COM4", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 

//Основные параметры последовательного порта описываются структурой DCB
//Резервируем место под структуру
DCB dcbSerialParams = {0};
//достаём из ядра текущее состояние интерфейса
dcbSerialParams.DCBlength = sizeof (dcbSerialParams);

//выведем на экран несоклько его параметров: скорость, размер инф.сообщения, бит четности
int status = GetCommState(hComm, &dcbSerialParams);// ф-ция для  получеия текущего состояния структуры из ядра

//printf ("BaudRate = %d, ByteSize = %d, Parity = %d \n", dcbSerialParams.BaudRate, dcbSerialParams.ByteSize, dcbSerialParams.Parity);

//теперь меняем параметры
dcbSerialParams.BaudRate = CBR_9600;
dcbSerialParams.ByteSize = 8;
dcbSerialParams.Parity = NOPARITY;
dcbSerialParams.StopBits = ONESTOPBIT;

//dcbSerialParams.fRtsControl = RTS_CONTROL_HANDSHAKE;
//dcbSerialParams.fOutxCtsFlow = TRUE;

//отправляем структуру в ядро
SetCommState (hComm, &dcbSerialParams);

//Проверка дескриптора
if (hComm == INVALID_HANDLE_VALUE)
{
	printf ("Error openning  serial port!\n");
	ExitProcess (1);
}	
//int i=0;
while (1)
{
	//ReadFiLe(дескриптор; адрес буфера, куда класть данных; кол-во байт, которые нужно прочитать; указатель на место, где записано, сколько байт прочитано )
	ReadFile (hComm, &buffer, 1, &bytesRead, NULL); 
	if (bytesRead==0) 
	{
		//Данные не передаются
		continue;
	}
	else
	{
		printf ("buffer = %c, bytesRead = %d\n", buffer[0], bytesRead);
		break; 
		//i++;
		//Sleep(1000);
	}
}

//закрываем устройство
CloseHandle(hComm);

	
	
return 0;	
}