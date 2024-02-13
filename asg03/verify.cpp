#include <map>
#include <fstream>
#include <iostream>
#include <vector>
#include<string>

int main()
{
    std::ifstream in;

    int temp=0;
    int found=-1;
    std::string buffer, path="";
    std::map<int, int> m;
    for (int i = 0; i <10; i++)
    {
        path = "a (" + std::to_string(i+1);
        path+=").csv";
        in.open(path);
        if(!in.is_open())
            return -9;
        while(in >> temp >> buffer)
        {

            if (in.eof())
                continue;
            if(temp != 0)
                in >> buffer;
            if (!m.count(temp))
                m.insert(std::pair<int,int>(temp,temp));
            else
            {
                std::cerr << "Duplicate entry: inputsim is not threadsafe\n";
                in.close();
                return -1;
            }
        }
        in.close();
        std::cout << "\nNo duplicates found in " << path << "\n";

    }

    std::map<int, int>::iterator it = m.begin();
    std::vector<int> v;
    while (it != m.end()) 
    {
        if (it == m.begin())
        {
            ++it;
            continue;
        }
        else
        {
            auto l = --it;
            ++it;
            if (l->first != it->first - 1)
            {
                std::cerr << "Missing value\n";
                v.push_back(it->first - 1);
            }
        }
        ++it;
    }
    return v.size();
}