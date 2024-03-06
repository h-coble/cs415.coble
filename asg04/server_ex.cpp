// C++ program to show the example of server application in 
// socket programming 
#include <cstring> 
#include <iostream> 
#include <string>
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <vector>
#include <fstream>
using namespace std; 

vector<string> readFile()
{
	ifstream file("fortune.txt");
	string temp;
	vector<string> v;
	while(getline(file,temp))
	{
		v.push_back(temp);
		getline(file,temp);
	}	
	cout << v[0]+"\n" << v[1] +"\n"<< v[2]+"\n"<< "EAT MY ASS";
	file.close();
	return v;
}
int main() 
{ 
	vector<string> fortune = readFile();
	// creating socket 
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 

	// specifying the address 
	sockaddr_in serverAddress; 
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(9876); 
	serverAddress.sin_addr.s_addr = INADDR_ANY; 

	// binding socket. 
	bind(serverSocket, (struct sockaddr*)&serverAddress, 
		sizeof(serverAddress)); 

	// listening to the assigned socket 
	listen(serverSocket, 5); 

	// accepting connection request 
	int clientSocket 
		= accept(serverSocket, nullptr, nullptr); 

	// recieving data 
	char buffer[1024] = { 0 }; 
	recv(clientSocket, buffer, sizeof(buffer), 0); 
	cout << "Message from client: " << buffer 
			<< endl; 

	// closing the socket. 
	close(serverSocket); 

	return 0; 
}
