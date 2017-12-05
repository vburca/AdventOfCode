#include <iostream>
#include <string>

using namespace std;


int part1(string input)
{
    int sum = 0;

    for (string::size_type i = 0; i < input.size(); i++)
    {
        if (input[i] == input[(i + 1) % input.size()])
        {
            sum += input[i] - '0';
        }
    }

    return sum;
}

int part2(string input)
{
    int sum = 0;
    int wrap_around = input.size() / 2;

    for (string::size_type i = 0; i < input.size(); i++)
    {
        if (input[i] == input[(i + wrap_around) % input.size()])
        {
            sum += input[i] - '0';
        }
    }

    return sum;
}


int main()
{
    string input;
    cin >> input;

    cout << part1(input) << endl;
    cout << part2(input) << endl;

    return 0;
}