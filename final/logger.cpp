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
#include <random>

//Sockets Reference
//https://www.geeksforgeeks.org/socket-programming-in-cpp/

//Directory path is current directory of .cpp/.exe                                  CRITICAL
const std::filesystem::path DIRPATH = "./";
std::mutex DMX;

//Max file size = 2kb ~=2000 bytes
const int MAX_FILE_SIZE = 1000;

//Flag to stop threads                                                              CRITICAL
bool CONTINUE = true;


class Logger
{
    private:
        int clientSocket;
    
    public:
        Logger()
        {
           clientSocket = socket(AF_INET, SOCK_STREAM, 0);

           // specifying address 
            sockaddr_in serverAddress; 
            serverAddress.sin_family = AF_INET; 
            serverAddress.sin_port = htons(6982); 
            serverAddress.sin_addr.s_addr = INADDR_ANY;

            connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        }
        ~Logger()
        {
            close(clientSocket); 
        }

        void urgent(int id, std::string message)
        {
            std::string temp = "urgent," + std::to_string(id) + "," + message;
            send(clientSocket, temp.c_str(), strlen(temp.c_str()), 0); 
        }
        void critical(int id, std::string message)
        {
            std::string temp = "critical," + std::to_string(id) + "," + message;
            send(clientSocket, temp.c_str(), strlen(temp.c_str()), 0); 
        }
         void error(int id, std::string message)
        {
            std::string temp = "error," + std::to_string(id) + "," + message;
            send(clientSocket, temp.c_str(), strlen(temp.c_str()), 0); 
        }
         void warning(int id, std::string message)
        {
            std::string temp = "warning," + std::to_string(id) + "," + message;
            send(clientSocket, temp.c_str(), strlen(temp.c_str()), 0); 
        }

};

std::string mkdir()
{
    std::string name;
    std::cout << "Enter the name of the subfolder to store log files: ";
    std::cin >> name;

    if (!std::filesystem::exists(DIRPATH / name))
        std::filesystem::create_directory(name);

    //CREATE LOG FILES SO APPEND IN SERVER THREAD WORKS PROPERLY    
    std::ofstream file(DIRPATH / name / "urgent.log");
    file.close();
    file.open(DIRPATH / name / "critical.log");
    file.close();
    file.open(DIRPATH / name / "error.log");
    file.close();
    file.open(DIRPATH / name / "warning.log");
    file.close();

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
            std::size_t notbackup = entry.path().string().find("_backup.log");
            
            if(notbackup == std::string::npos)
            {
                //std::cout << entry.path().string() << std::endl;
                std::ifstream file(entry.path());
                std::streampos begin = file.tellg();
                file.seekg (0, std::ios::end);
                std::streampos end = file.tellg();

                if (end - begin >= MAX_FILE_SIZE)
                {
                    //std::cout << "Watcher: File size exceeded.\n";
                    //RENAME TO BACKUP
                    std::string newName = entry.path().string();
                    //remove .log file extension
                    newName.erase(newName.end() - 4, newName.end());
                    newName += "_backup.log";
                    rename(entry.path(), newName); 
                    //NEW FILE, OLD NAME
                    std::ofstream newFile(entry.path());
                    newFile.close();
                    file.close();
                }
            }
        } 
        DMX.unlock();
        //std::cout << "Watcher: Sleeping...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void serverThreadFunc(const std::string folderName)
{   
    const std::filesystem::path folderPath = DIRPATH / folderName;
    std::string level, msg;
    short id;
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
        int clientSocket  = accept(serverSocket, nullptr, nullptr); 

        // recieving data 
        char buffer[1024] = { 0 }; 
        recv(clientSocket, buffer, sizeof(buffer), 0);
        
        //Parse Message
        std::string unparsedMsg(buffer);
        if(unparsedMsg != "")
        {
            level = unparsedMsg.substr(0, unparsedMsg.find(','));
            unparsedMsg = unparsedMsg.substr(unparsedMsg.find(',')+1);
            id = std::stoi(unparsedMsg.substr(0, unparsedMsg.find(',')));
            msg = unparsedMsg.substr(unparsedMsg.find(',') + 1);
            
            DMX.lock();
                std::ofstream fileOut(folderPath / (level + ".log"), std::ios_base::app);                               //Open appropriate file
                fileOut << std::setw(12) << std::left << level << std::setw(12) << std::left << id << msg <<std::endl;
                fileOut.close();
            DMX.unlock();
        }
         
    }
	// closing the socket. ONCE THREAD IS TO BE TERMINATED
	close(serverSocket); 
}

int main()
{
    const std::string folderName =  mkdir();
    
    std::thread watcher(watcherThreadFunc, folderName);
    watcher.detach();

    std::thread server(serverThreadFunc, folderName);
    server.detach();

    int choice, id =0;
    std::string message = "Some string of words";
    auto start = std::chrono::steady_clock::now();
    //5 minute end time
    auto end = std::chrono::milliseconds(3*60000);  
    do 
    {
        Logger logObj;
        
        std::random_device rd{};
        std::mt19937 gen{rd()};
        std::uniform_int_distribution<int> uD(1,4);

        choice = uD(gen);
        switch(choice)
        {
            case 1:
                logObj.warning(id++, message);
                break;
            case 2:
                logObj.error(id++, message);
                break;
            case 3:
                logObj.critical(id++, message);
                break;
            case 4:
                logObj.urgent(id++, message);
                break;
            default:
                std::cout << "Invalid level value, try again.\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    while(std::chrono::steady_clock::now() - start < end);
    CONTINUE = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << --id << " messages should be logged.\n";
    return 0;
}
