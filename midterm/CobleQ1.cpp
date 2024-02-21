#include<string>
#include<cstring>
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

//COMMENT FROM ASG01
//Hayden Coble
//
//After spending hours trying to find a good way to do this,
//I ended up finding *a* solution on Chegg (although I despise doing so) 
//because all my attempts left me at a loss, plus or minus waaaaay too many lines of garbage. 
//I've modified it a bit and left comments to show my understanding of how it works.
//One of the big changes is that I tried to fix some memory leaks.
//The original can be found here: 
//https://www.chegg.com/homework-help/questions-and-answers/char-parsecommandline-string-acommandline-function-takes-input-parameter-splits-string-arr-q43837138


std::string PATH = "/bin/";
bool isOp(char x)
{
    return x == ' ' || x== '|' || x== '<' || x== '>';
}


int countKeywords(char** a)
{
    int b = 0;
    for (int i = 0; i < 64; i++)
    {
        if(a[i] != nullptr)
            b++;
        else
            delete[] a[i];
    }
    return b;
}

void cleanPtrs(char** x)
{
    for (int i =0; i < countKeywords(x); i++)
        delete[] x[i];

    delete[] x;
}
char ** parseCommandLine(std::string aCommandLine, size_t& numArgs)
{
    int inputItr =0, outputItr =0;
    char** output = new char* [50];
    for (int i = 0; i < 50; i++)
        output[i] = nullptr;

    while (inputItr < aCommandLine.length())
    {
        //Handle operators
        if(isOp(aCommandLine[inputItr]))
        {
            switch(aCommandLine[inputItr])
            {
                case '<':
                    if (aCommandLine[inputItr+1] == '<')    // <<
                    {
                        inputItr++; //increment extra time to skip next <
                    }
                    else    //single <
                        break;
                    break;

                case '>':  
                    break;

                case '|':    
                    break;
            }
            inputItr++;
            continue;
        }

        //Find end of keyword
        int endOfWord=inputItr;
        while(endOfWord<aCommandLine.length() && !isOp(aCommandLine[endOfWord]))
        {
            endOfWord++;
        }

        //store keyword
        std::string temp = aCommandLine.substr(inputItr, endOfWord-inputItr);

        //Convert to c string
        char* keyword = new char[temp.length()+2];  //+2 for \0 at end
        strcpy(keyword, temp.c_str());              //strcpy because c_str --> const char*
        strcat(keyword,"\0");
        //Store and increment itr
        output[outputItr++] = keyword;
        numArgs++;
        //Set starting itr/input itr for next word
        inputItr=endOfWord;
    }

    //output[outputItr] = NULL;
    return output;
}

void mySystem(const char* command)
{
    size_t num;
    const std::string e = "exit\0";
    char* cmdpath = new char [64];
    strcpy(cmdpath,PATH.c_str());
    char** argv = parseCommandLine(command,num);
    
    if(e == argv[0])
    {
        cleanPtrs(argv);
        exit(0);
    }
    strcat(cmdpath,argv[0]);
    strcpy(argv[0],cmdpath);
    pid_t child_pid = fork();
    if(child_pid == 0)
    {
        //Child - Exec Command then exit
        execv(argv[0],argv);
        cleanPtrs(argv);
    }
    else if(child_pid > 0)
    {
        //Parent
        wait(NULL);
    }
    else
        std::cerr << "ERROR: Failure to fork.\n";
    
    return;
}
int main(int argc, char *argv[], char *env[] )
{
    std::string userCmd;
    pid_t child_pid;
    std::cout << "CS415 Exam01 Q1: Enter \"exit\" to exit.\n\n";
    while (true)
    {
        std::cout << "ExamSys# ";
        std::getline(std::cin, userCmd);
        mySystem(userCmd.c_str());
    }
    return 0;
}