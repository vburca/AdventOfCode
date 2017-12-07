#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;


int part1(string filename)
{
    ifstream infile(filename);
    string line;

    int checksum = 0;

    while (getline(infile, line))
    {
        istringstream iss(line);

        bool set = false;
        int min = -1;
        int max = -1;

        int n;
        while (iss >> n)
        {
            if (!set)
            {
                min = n;
                max = n;
                set = true;
                continue;
            }

            if (n > max)
            {
                max = n;
                continue;
            }

            if (n < min)
            {
                min = n;
                continue;
            }
        }

        checksum += (max - min);
    }

    infile.close();

    return checksum;
}

int get_division(vector<int>* ns)
{
    for (int i = 0; i < ns->size() - 1; i++)
    {
        int a = ns->at(i);
        for (int j = i + 1; j < ns->size(); j++)
        {
            int b = ns->at(j);

            if (a % b == 0)
            {
                return a / b;
            }

            if (b % a == 0)
            {
                return b / a;
            }
        }
    }

    return -1;
}

int part2(string filename)
{
    ifstream infile(filename);
    string line;

    int checksum = 0;

    while (getline(infile, line))
    {
        istringstream iss(line);

        vector<int> ns;
        int n;

        while (iss >> n)
        {
            ns.push_back(n);
        }

        checksum += get_division(&ns);
    }

    infile.close();

    return checksum;
}


int main(int argc, char* argv[])
{
    cout << part1(argv[1]) << endl;
    cout << part2(argv[1]) << endl;

    return 0;
}