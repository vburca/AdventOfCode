#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;


int part1(string filename)
{
    ifstream infile(filename);
    string line;
    getline(infile, line);

    int n;
    istringstream iss(line);
    iss >> n;

    infile.close();

    return -1;
}

int part2(string filename)
{
    ifstream infile(filename);
    string line;

    istringstream iss(line);

    infile.close();

    return -1;
}


int main(int argc, char* argv[])
{
    cout << part1(argv[1]) << endl;
    cout << part2(argv[1]) << endl;

    return 0;
}