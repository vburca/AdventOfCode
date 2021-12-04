#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "BingoBoard.h"

using namespace std;

void readBoards(istream& inputFile, vector<size_t>& chosenNumbers, vector<BingoBoard>& boards)
{
    bool firstLine = true;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (firstLine)
        {
            istringstream iss(line);
            string numStr;
            while (getline(iss, numStr, ','))
            {
                const size_t num = stoi(numStr);
                chosenNumbers.push_back(num);
            }

            firstLine = false;
        }
        else if (line.size() == 0)
        {
            auto board = BingoBoard();
            size_t row = 0;
            while (row < MAX_ROWS)
            {
                size_t col = 0;

                getline(inputFile, line);
                istringstream iss(line);
                size_t num;
                while (iss >> num)
                {
                    board.add(row, col, num);
                    col += 1;
                }

                row += 1;
            }

            boards.push_back(board);
        }
    }
}

void part1(istream& inputFile)
{
    vector<size_t> chosenNumbers;
    vector<BingoBoard> boards;

    readBoards(inputFile, chosenNumbers, boards);

    for (const auto chosenNumber : chosenNumbers)
    {
        for (auto& board : boards)
        {
            const auto score = board.mark(chosenNumber);

            if (score > 0)
            {
                cout << score << endl;
                return;
            }
        }
    }
}

void part2(istream& inputFile)
{
    vector<size_t> chosenNumbers;
    vector<BingoBoard> boards;

    readBoards(inputFile, chosenNumbers, boards);

    size_t lastWinnerScore = 0;
    for (const auto chosenNumber : chosenNumbers)
    {
        for (auto& board : boards)
        {
            if (board.isWinner())
            {
                continue;
            }

            const auto score = board.mark(chosenNumber);

            if (score > 0)
            {
                lastWinnerScore = score;
            }
        }
    }

    cout << lastWinnerScore << endl;
}

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cout << "Must specify input file!" << endl;
        return 0;
    }

    cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    cout << "Part 1: " << endl;
    part1(inputFile);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    cout << "Part 2: " << endl;
    part2(inputFile);

    inputFile.close();

    return 0;
}