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
#include <time.h>
#include <chrono>

#define TimeOfGame 60

using namespace std;

enum states {WaitingOfConnection, WaitingOfCombat, Win, Lose, Combat};
enum Msg_type {result_of_shot, state_for_client};
enum ResultOfShot {not_hit, hit};

struct ResultOfBung
{
	int16_t type = static_cast<int16_t>(result_of_shot);
	int16_t result;
};

struct Shot
{
	int16_t PosX;
	int16_t PosY;
};

struct StateForClient
{
	int16_t type = static_cast<int16_t>(state_for_client);
	int16_t state;
};

long start;
int FirstPlayer, SecondPlayer;
int ScoreOfFP, ScoreOfSP;
states stateOfFirstPlayer = WaitingOfConnection, stateOfSecondPlayer = WaitingOfConnection;

struct Threads
{
	pthread_t* firstThread;
	pthread_t* secondThread;
};

void* Timer(void* AllThreads)
{
	Threads* GameThreads = static_cast<Threads*>(AllThreads);

	while(time(NULL) <= start + TimeOfGame);

	cout << "Время вышло" << endl;

	pthread_cancel(*GameThreads->firstThread);
	pthread_cancel(*GameThreads->secondThread);

	pthread_exit(0);
}

void* DataFromFirstClient(void* NullData)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	StateForClient State;
	State.state = static_cast<int16_t>(Combat);
	send(FirstPlayer, &State, sizeof(State), MSG_NOSIGNAL);

	while(1)
	{
	}
}

void* DataFromSecondClient(void* NullData)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	StateForClient State;
	State.state = static_cast<int16_t>(Combat);
	send(SecondPlayer, &State, sizeof(State), MSG_NOSIGNAL);

	while(1)
	{
	}
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
		
		stateOfFirstPlayer = stateOfSecondPlayer = WaitingOfCombat;
		
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

	sleep(3);

	pthread_t FirstThread, SecondThread, TimerThread;

	Threads MyThreads;

	MyThreads.firstThread = &FirstThread;
	MyThreads.secondThread = &SecondThread;

	start = time(NULL);

	pthread_create(&FirstThread, 0, DataFromFirstClient, Null);
	pthread_create(&SecondThread, 0, DataFromSecondClient, Null);
	pthread_create(&TimerThread, 0, Timer, &MyThreads);

	pthread_join(TimerThread, 0);
	//--------------------------------------------------------

	shutdown(MasterSocket, SHUT_RDWR);
	close(MasterSocket);
	cout << "Игра завершилась" << endl;

	return 0;
}
