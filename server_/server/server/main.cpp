#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;
#pragma comment(lib,"ws2_32.lib")
//This function initializes the Winsock library, which is necessary for socket programming on Windows.
void InteractwithClient(SOCKET clientSocket, vector<SOCKET> &clients)
{
 //Sending a welcome message to the connected client
	while (1)
	{
		char buffer[4096]; //Buffer to store received data
		int bytesrecieved = recv(clientSocket, buffer, sizeof(buffer), 0); //Receiving data from the client (not processing it here)
		if(bytesrecieved<=0)
		{
			cout << "Client Disconnected! " << endl; 
			break; //Exit the loop if the client disconnects or an error occurs
		}
		string receivedMessage(buffer, bytesrecieved); //Creating a string from the received data
		cout << "Received message: " << receivedMessage << endl; //Displaying the received message
		for(auto client:clients)
		{
			if(client!=clientSocket) //Avoid sending the message back to the sender
			{
				send(client, receivedMessage.c_str(), receivedMessage.size(), 0); //Broadcasting the message to all other connected clients
			}
		}
	}
	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it); //Removing the disconnected client from the list
	}
	closesocket(clientSocket); //Closing the client socket
	
}

bool initiaize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
} //This will return true if initialization is successful, otherwise false.

//MAIN CODE: 

int main() {
	//Now i'll call the initialize function
	if (!initiaize())
	{
		cout << "Initialization failed" << endl; //If initialization fails, this will be showed.
		return -1;//Error code
	} // i.e. Initialization done
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); //Creating a TCP socket using IPv4
	if(sock==INVALID_SOCKET)
	{
		cout << "Socket creation failed" << endl; //If socket creation fails, this will be showed.
		//WSACleanup();Not Now
		return -1; //Exiting with error code
	}
	//Now Ill create a adress structure to bind the socket
	int port = 8080; //Defining the port number to bind the socket
	sockaddr_in serverAddr; //Creating a sockaddr_in structure for the server address
	serverAddr.sin_family = AF_INET; //Setting the address family to IPv4
	serverAddr.sin_port = htons(port); //Setting the port number to 8080, converting to network byte order
	
	//Now Converting the IP address to binary form and assigning it to sin_addr
	if (InetPton(AF_INET, _T("0.0.0.0"),&serverAddr.sin_addr) != 1) {
		cout << "Invalid IP address" << endl; //If IP address conversion fails, this will be showed.
		closesocket(sock); //Closing the socket
		WSACleanup(); //Cleaning up Winsock resources
		return -1; //Exiting with error code
	}
	//Now, Bind the ip with the socket
	if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) //We couldve used reinterpret_cast<sockaddr*> too
	{
		cout << "Bind failed" << endl; //If binding fails, this will be showed.
		closesocket(sock); //Closing the socket
		WSACleanup(); //Cleaning up Winsock resources
		return -1; //Exiting with error code
	}
	cout << "Socket successfully bound to IP and port" << endl; //If binding is successful, this will be showed.
	//Listen
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) //Listening for incoming connections with maximum backlog,where  SOMAXCONN is a constant definef
	{
		cout << "Listen failed" << endl; //If listening fails, this will be showed.
		closesocket(sock); //Closing the socket
		WSACleanup(); //Cleaning up Winsock resources	
		return -1; //Exiting with error code
	}
	cout << "Server is listening for" << port << endl; //If listening is successful, this will be showed.
	
	vector<SOCKET> clients;
	//Accept
	while (1)
	{
		SOCKET clientSocket = accept(sock, nullptr, nullptr); //Accepting an incoming connection
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Accept failed i.e INVALID CLIENT SOCKET" << endl; //If accepting fails, this will be showed.
			closesocket(sock); //Closing the server socket
			WSACleanup(); //Cleaning up Winsock resources
			return -1; //Exiting with error code
		}
		clients.push_back(clientSocket);
		thread t1(InteractwithClient, clientSocket,ref(clients));
		t1.detach(); //Detaching the thread to allow it to run independently
	}



	//Now I'll close the listen soxket :
	closesocket(sock); //Closing the server socket

	
	//Server Program is complete.
	
	WSACleanup(); //This function cleans up the Winsock library resources.
	return 0;
}