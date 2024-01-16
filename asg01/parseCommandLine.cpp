#include<string>
#include<iostream>


char** parseCommandLine(std::string aCommandLine)
{
    //Create 
    int numKeywords = 0;
    char* word = new char[1];
    char* op = new char[1];
    char** keywords = &word;
    char** operators = &op;

    std::size_t prev = 0, pos = aCommandLine.find_first_of("<>| ");



}


int main()
{

}