#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <iostream>
#include <clocale>
#include <fcntl.h>

using namespace std;

enum states {WaitingOfConnection, Win, Lose, Combat};
enum Msg_type {result_of_shot, state_for_client};
enum ResultOfShot {not_hit, hit};

struct Shot
{
	int16_t PosX;
	int16_t PosY;
};

struct Message
{
	int16_t type;
	int16_t Result;
	int16_t PosX;
	int16_t PosY;
};

struct StateForClient
{
	int16_t type = static_cast<int16_t>(state_for_client);
	int16_t state;
};

int FirstPlayer, SecondPlayer;
states stateOfFirstPlayer = WaitingOfConnection, stateOfSecondPlayer = WaitingOfConnection;

void* DataFromFirstClient(void* NullData)
{
	while(1);
	pthread_exit(0);
}

void* DataFromSecondClient(void* NullData)
{
	while(1);
	pthread_exit(0);
}

int main()
{
	setlocale(LC_ALL, "Russian");

	StateForClient stateFP, stateSP;

	//---------------Создание слушающего сокета---------------------
	int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in MasterAddr;
	MasterAddr.sin_family = AF_INET;
	MasterAddr.sin_port = htons(25678);
	MasterAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(MasterSocket, (struct sockaddr*) (&MasterAddr), sizeof(MasterAddr));

	listen(MasterSocket, 1);
	//-------------------------------------------------------------
	
	//----------------Соединение клиентов---------------------
	
	FirstPlayer = accept(MasterSocket, 0, 0);

	if(FirstPlayer > 0)
	{
		printf("Первый игрок подключился!\n");

		stateFP.state = static_cast<int16_t>(stateOfFirstPlayer);
		send(FirstPlayer, &stateFP, sizeof(stateFP), MSG_NOSIGNAL);
	}
	else
	{
		printf("Соединение провалено!\n");
		return 0;
	}

	cout << "После подключения первого игрока: " << stateOfFirstPlayer << " " << stateOfSecondPlayer << "\n";

	SecondPlayer = accept(MasterSocket, 0, 0);

	if(SecondPlayer > 0)
	{
		printf("Второй игрок подключился!\n");
		
		stateOfFirstPlayer = stateOfSecondPlayer = Combat;
		
		stateFP.state = stateOfFirstPlayer;
		stateSP.state = stateOfSecondPlayer;

		send(FirstPlayer, &stateFP, sizeof(stateFP), MSG_NOSIGNAL);
		send(SecondPlayer, &stateSP, sizeof(stateSP), MSG_NOSIGNAL);
	}
	else
	{
		printf("Соединение провалено!\n");
		return 0;
	}
	//------------------------------------------------------
	
	cout << "После подключения всех игроков: " << stateOfFirstPlayer << " " << stateOfSecondPlayer << "\n";

	//----------------------Процесс игры----------------------
	void* Null = NULL;
	
	pthread_t FirstThread, SecondThread;

	pthread_create(&FirstThread, 0, DataFromFirstClient, Null);
	pthread_create(&SecondThread, 0, DataFromSecondClient, Null);

	pthread_join(FirstThread, 0);
	pthread_join(SecondThread, 0);
	//--------------------------------------------------------

	return 0;
}
