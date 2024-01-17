#include<string>
#include<cstring>
#include<iostream>

//Hayden Coble
//
//After spending hours trying to find a good way to do this,
//I ended up finding *a* solution on Chegg (although I despise doing so) 
//because all my attempts left me at a loss, plus or minus waaaaay too many lines of garbage. 
//I've modified it a bit and left comments to show my understanding of how it works.
//One of the big changes is that I tried to fix some memory leaks.
//The original can be found here: https://www.chegg.com/homework-help/questions-and-answers/char-parsecommandline-string-acommandline-function-takes-input-parameter-splits-string-arr-q43837138

bool isOp(char x)
{
    return x == ' ' || x== '|' || x== '<' || x== '>';
}

void printKeywords(char** a, int b)
{
    for (int i =0; i < b; i++)
        std::cout << a[i] << "\n";
}
int countKeywords(char** a)
{
    int b = 0;
    for (int i = 0; i < 50; i++)
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
char ** parseCommandLine(std::string aCommandLine)
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

        //Store and increment itr
        output[outputItr++] = keyword;
        //Set starting itr/input itr for next word
        inputItr=endOfWord;
    }

    //Prove it works
    printKeywords(output, outputItr);
    return output;
}

int main()
{
    char** x = parseCommandLine("grep x < myfile.textbfin > myfile.out");
    cleanPtrs(x);

    return 0;
}