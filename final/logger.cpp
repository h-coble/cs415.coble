#include <thread>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <mutex>
#include <algorithm>


//Sockets Reference
//https://www.geeksforgeeks.org/socket-programming-in-cpp/

//Directory path is current directory of .cpp/.exe                                  CRITICAL
const std::filesystem::path DIRPATH = "./";
std::mutex DMX;

//Max file size = 2kb ~=2000 bytes
const int MAX_FILE_SIZE = 2000;

//Flag to stop threads                                                              CRITICAL
bool CONTINUE = true;

std::string mkdir()
{
    std::string name;
    std::cout << "Enter the name of the subfolder to store log files: ";
    std::cin >> name;

    if (!std::filesystem::exists(DIRPATH / name))
        std::filesystem::create_directory(name);

    return name;
}

void watcherThreadFunc(const std::string folderName)
{
    std::filesystem::path folderPath = DIRPATH / folderName;
    while(CONTINUE)
    {
        DMX.lock();
        for (const auto& entry :  std::filesystem::directory_iterator(folderPath)) 
        { 
            std::ifstream file(entry.path());
            
            std::streampos begin = file.tellg();
            file.seekg (0, std::ios::end);
            std::streampos end = file.tellg();

            if (end - begin > MAX_FILE_SIZE)
            {
                //RENAME TO BACKUP
                std::string newName = entry.path().string();
                newName.erase(newName.end() -4, newName.end());
                newName += "_backup.";
                
                //NEW FILE, OLD NAME
                std::ofstream newFile(entry.path());
                newFile.close();
                file.close();
            }
        } 
        DMX.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
}

void serverThreadFunc(const std::string folderName)
{
    const std::filesystem::path folderPath = DIRPATH / folderName;
    std::string level;


    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 

    // specifying the address 
	sockaddr_in serverAddress; 
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(6982); 
	serverAddress.sin_addr.s_addr = INADDR_ANY; 

	// binding socket. 
	bind(serverSocket, (struct sockaddr*)&serverAddress, 
		sizeof(serverAddress)); 

    while (CONTINUE)
	{
        // listening to the assigned socket 
        listen(serverSocket, 5); 

        // accepting connection request 
        int clientSocket 
            = accept(serverSocket, nullptr, nullptr); 

        // recieving data 
        char buffer[1024] = { 0 }; 
        recv(clientSocket, buffer, sizeof(buffer), 0);

        //Parse into tokens
         
    }
	// closing the socket. 
	close(serverSocket); 
}


int main()
{
    const std::string folderName =  mkdir();

    std::cout << "enter main\n";
    //Refer to a file by filesystem/path
    std::filesystem::path filepath;

    //Create subfolder in current dir by name
    std::filesystem::create_directory("subfolder");

    //Navigate new filepath then create file (ofstream)
    filepath = DIRPATH / "subfolder/ex.txt";
    std::ofstream fileO(filepath);

    fileO << "stuff\n";
    fileO.close();


    for (const auto& entry : 
            std::filesystem::directory_iterator(DIRPATH)) { 
            // Output the path of the file or subdirectory 
            std::cout << "File: " << entry.path() << std::endl; 
        } 

    std::ifstream file(filepath);

    //Size of file
    std::streampos begin = file.tellg();
    file.seekg (0, std::ios::end);
    std::streampos end = file.tellg();


    if(file.is_open())
        std::cout << end - begin << " bytes.\n";
    file.close();
    return 0;
}
