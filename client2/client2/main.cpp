#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<string>
#include<thread>
using namespace std;
#pragma comment(lib,"ws2_32.lib")
//Initialize Winsock
bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
//Function to receive messages from server
void sendMessage(SOCKET s)
{
	cout << "Enter Your Chat name: ";
	string chatName;
	cin >> chatName;
	while (true)
	{
		string message, msg;
		cout << "\n:";
		getline(cin, msg);
		message = "\n" + chatName + ": " + msg; //It'll show like ex:  Swarnab: Hello there!
		//Now send the message to server
		int bytesSent = send(s, message.c_str(), message.size() + 1, 0);
		if (bytesSent == SOCKET_ERROR)
		{
			cout << "Sending Data Failed" << endl;
			break;
		}
		if (msg == "close")
		{
			cout << "Closing Application";
			break; //On Typing close, the client will stop sending messages
		}

	}
	closesocket(s);
	WSACleanup();
}
void receiveMessage(SOCKET s)
{
	while (true)
	{
		char buffer[4096];
		int bytesReceived = recv(s, buffer, sizeof(buffer), 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cout << "Receiving Data Failed" << endl;
			break;
		}
		else if (bytesReceived == 0)
		{
			cout << "Server Disconnected" << endl;
			break;
		}
		else
		{
			cout << string(buffer, 0, bytesReceived);
		}
	}
}

//Main Code
int main()
{
	cout << "Client Program Started" << endl;
	if (!Initialize())
	{
		cout << "Initialization Failed" << endl;

		return 1;
	}
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		cout << "Invalid Socket Creation" << endl;
		return 1;
	}

	string serveraddress = "127.0.0.1";
	int port = 8080;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &serveraddr.sin_addr);
	//Now we'll connect the client to the server
	if (connect(s, (sockaddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "Connection to Server Failed" << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}
	cout << "Connected to Server Successfully" << endl;
	//Send/Recieve messages using multithreading.

	thread senderthread(sendMessage, s);
	thread receiverthread(receiveMessage, s);
	senderthread.join();
	receiverthread.join();
	return 0;

}