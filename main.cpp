#include<iostream>
#include<WinSock2.h>
#include<fstream>
#include<istream>
#include<string>//getline
#include<cstring>
#include<fileapi.h>
#define MAX 500
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")
using namespace std;


int main()
{
	cout << "\t\t---------TCP SERVER-------\t\t" << endl;
	cout << endl;

	//local variable
	WSADATA winSockData;
	int iWsaStartup;
	int iWsaCleanup;
	string SenderBuffer;
	SOCKET TCPServerSocket;
	int iCloseSocket;

	struct sockaddr_in TCPServerAdd;
	struct sockaddr_in TCPClientAdd;
	int iTCPClientAdd = sizeof(TCPClientAdd);
	
	int iBind;
	int iListen;
	
	SOCKET sAcceptSocket;

	int iSend;
	
	
	int iRecv;
	char RecvBuffer[512];
	int irecvBuffer = strlen(RecvBuffer) + 1;

	//step1 WSAStartup function

	iWsaStartup = WSAStartup(MAKEWORD(2, 2), &winSockData);
	if (iWsaStartup != 0)
	{
		cout << "WSaStartup failed " << endl;
	}
	cout << "WSaStartup Success " << endl;

	//step2 Fillthe structure
	TCPServerAdd.sin_family = AF_INET;
	TCPServerAdd.sin_addr.s_addr = inet_addr("127.0.0.1");
	TCPServerAdd.sin_port = htons(8080);

	
	//step3 socket creation
	TCPServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPServerSocket == INVALID_SOCKET)
	{
		cout << "TCP serversocket creation failed " << WSAGetLastError() << endl;
	}
	cout << "TCP serversocket creation success " << endl;

	//step4 bind function
	iBind = bind(TCPServerSocket, (SOCKADDR*)&TCPServerAdd, sizeof(TCPServerAdd));
	if (iBind == SOCKET_ERROR)
	{
		cout << "Binding failed. ERROR -> " << WSAGetLastError() << endl;
	}
	cout << "Binding success " << endl;

	//step5 listen function
	iListen = listen(TCPServerSocket, 2);
	if (iListen == SOCKET_ERROR)
	{
		cout << "listen function failed. ERROR -> " << WSAGetLastError() << endl;
	}
	cout << "listen function success " << endl;

	while (1)
	{

	//step6 accept function
	sAcceptSocket = accept(TCPServerSocket, (SOCKADDR*)&TCPClientAdd, &iTCPClientAdd);
	//cout << endl << "-----------" << "-----------" << iTCPClientAdd;
	if (sAcceptSocket == INVALID_SOCKET)
	{
		cout << "Accept function failed. ERROR -> " << WSAGetLastError() << endl;
	}
	cout << "Accept function success " << endl;
	struct sockaddr_in *addr_in = (struct sockaddr_in *)&TCPClientAdd;
	char *ip = inet_ntoa(addr_in->sin_addr);

		//step8 receiving data from client
		iRecv = recv(sAcceptSocket, RecvBuffer, irecvBuffer, 0);
		if (iRecv == SOCKET_ERROR)
		{
			cout << "DATA receiving failed. ERROR -> " << WSAGetLastError() << endl;
		}
		cout << "DATA RECEIVED  " << endl << RecvBuffer << endl;

		//getting the filename from the client request
		char filename[20];
		int i,j, c = 0, flag = 0;
		for (i = 0; i < irecvBuffer; i++)
		{
			if (RecvBuffer[i] == '/')
			{
				flag = 1;
				for ( j = i + 1; RecvBuffer[j] != ' '; j++)
				{
					filename[c++] = RecvBuffer[j];
				}
				

			}
			if (flag == 1)
				break;
		}
		filename[c] = '\0';
		
		if (strlen(filename)==0)  //if url doesnot contain file then default file is index.html
		{
			strcpy(filename, "index.html");
		}
		
		
		cout << endl << "-------" << filename << "------" << endl;


		WIN32_FIND_DATA FindFileData;
		HANDLE hFile = FindFirstFileA(filename, &FindFileData); //finding thefile in directory
		if (hFile == INVALID_HANDLE_VALUE)
		{
			SenderBuffer = "HTTP/1.1 404 NOT FOUND\nContent-Type: text/plain\nContent-Length:15 \n\n file not found";

		}

		else
		{
			//reading the content from the file

			ifstream infile(filename);
			string s;
			string temp;
			while (!infile.eof())
			{
				getline(infile, temp);
				s = s + temp;
			}

			cout << "----------CONTENT OF FILE IS---------" << endl;
			cout << endl << s << endl << endl;


			std::string size = std::to_string(s.length());
			SenderBuffer = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
			SenderBuffer.append(size);
			SenderBuffer.append("\n\n");
			SenderBuffer.append(s);


		}//else

		int iSenderBuffer = strlen(SenderBuffer.c_str()) + 1;


		//step7 send data to client
		iSend = send(sAcceptSocket, SenderBuffer.c_str(), iSenderBuffer, 0);
		SYSTEMTIME lt;
		GetLocalTime(&lt);
		ofstream log("log.txt", ios::app);
		if (iSend == SOCKET_ERROR)
		{
			log << "IP:" << ip << " Time:" << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth<<"/" << lt.wYear << " Serve Status: Fail"<<" Requested file: "<<filename<< endl;
			cout << "sending failed. ERROR -> " << WSAGetLastError() << endl;
		}

		cout << "Data sending success " << endl;
		log << "IP:" << ip << " Time:" << lt.wHour << ":" << lt.wMinute << " Date:" << lt.wDay << "/" << lt.wMonth<<"/" << lt.wYear << " Serve Status: Success" <<" Requested file: " << filename << endl;

		/*if (iSend == SOCKET_ERROR)
		{
			cout << "sending failed. ERROR -> " << WSAGetLastError() << endl;
		}
		cout << "Data sending success " << endl;
*/

		//closefilehandle
		FindClose(hFile);
		
	}//while 

	 //step9 close socket
	iCloseSocket = closesocket(TCPServerSocket);
	if (iCloseSocket == SOCKET_ERROR)
	{
		cout << "closing socket failed. ERROR -> " << WSAGetLastError() << endl;
	}
	cout << "closing socket  success " << endl;

	//step10 cleanup
	iWsaCleanup = WSACleanup();
	if (iWsaCleanup == SOCKET_ERROR)
	{
		cout << "clean up failed. ERROR -> " << WSAGetLastError() << endl;
	}
	cout << "cleanup success " << endl;

	system("pause");
}