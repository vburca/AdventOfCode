#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <regex>
#include <numeric>

using namespace std;

const regex CONSTRAINTS_RX("(.*)\\: ([0-9]+)-([0-9]+) or ([0-9]+)-([0-9]+)");
const string YOUR_TICKET    = "your ticket:";
const string NEARBY_TICKETS = "nearby tickets:";

const vector<string> INTERESTING_FIELDS {
    "departure location",
    "departure station",
    "departure platform",
    "departure track",
    "departure date",
    "departure time"
};

const vector<string> TEST_INTERESTING_FIELDS {
    "class",
    "row"
};

void part1(istream& inputFile)
{
    map<string, pair<pair<uint32_t, uint32_t>, pair<uint32_t, uint32_t>>> constraints;
    vector<uint32_t> myTicket;

    size_t scanningErrorRate = 0;
    while (!inputFile.eof())
    {
        string line;

        // read constraints
        getline(inputFile, line);
        while (line.size() > 0)
        {
            smatch match;
            if (regex_match(line, match, CONSTRAINTS_RX))
            {
                auto field = match[1].str();
                uint32_t low1, low2, high1, high2;

                istringstream ss(match[2].str());
                ss >> low1;
                ss.clear();

                ss.str(match[3].str());
                ss >> high1;
                ss.clear();

                ss.str(match[4].str());
                ss >> low2;
                ss.clear();

                ss.str(match[5].str());
                ss >> high2;
                ss.clear();

                // cout << field << " : ( " << low1 << " , " << high1 << " ) or ( " << low2 << " , " << high2 << " )" << endl;

                constraints.emplace(field, pair(pair(low1, high1), pair(low2, high2)));
            }
            else
            {
                cout << "Unable to match constraints regex for line: " << line << endl;
                exit(1);
            }

            getline(inputFile, line);
        }

        // read "my ticket"
        getline(inputFile, line);
        // read the ticket numbers
        getline(inputFile, line);
        istringstream ss(line);

        // read each value
        string numStr;
        while (getline(ss, numStr, ','))
        {
            uint32_t num;
            istringstream numSs(numStr);
            numSs >> num;

            myTicket.push_back(num);
        }

        // read empty line
        getline(inputFile, line);

        // read "nearby tickets"
        getline(inputFile, line);
        while (!inputFile.eof())
        {
            // read the ticket numbers
            getline(inputFile, line);
            istringstream ss(line);

            // read each value
            string valueStr;
            bool isValid = true;
            while(getline(ss, valueStr, ',') && isValid)
            {
                // for each field value, assume that it is not valid
                isValid = false;

                uint32_t value;
                istringstream valueSs(valueStr);
                valueSs >> value;

                // check the field value against the field constraints
                for (auto keyValue = constraints.begin(); keyValue != constraints.end() && !isValid; ++keyValue)
                {
                    const auto [interval1, interval2] = keyValue->second;

                    if (((interval1.first <= value && value <= interval1.second) ||
                        (interval2.first <= value && value <= interval2.second)))
                    {
                        isValid = true;
                    }
                }

                if (!isValid)
                {
                    scanningErrorRate += value;
                }
            }
        }
    }

    cout << scanningErrorRate << endl;
}

void part2(istream& inputFile)
{
    vector<tuple<string, pair<uint32_t, uint32_t>, pair<uint32_t, uint32_t>>> constraints;
    vector<uint32_t> myTicket;
    vector<string> fieldNames;

    vector<vector<vector<bool>>> validTickets;

    while (!inputFile.eof())
    {
        string line;

        // read constraints
        getline(inputFile, line);
        while (line.size() > 0)
        {
            smatch match;
            if (regex_match(line, match, CONSTRAINTS_RX))
            {
                auto field = match[1].str();
                uint32_t low1, low2, high1, high2;

                istringstream ss(match[2].str());
                ss >> low1;
                ss.clear();

                ss.str(match[3].str());
                ss >> high1;
                ss.clear();

                ss.str(match[4].str());
                ss >> low2;
                ss.clear();

                ss.str(match[5].str());
                ss >> high2;
                ss.clear();

                // cout << field << " : ( " << low1 << " , " << high1 << " ) or ( " << low2 << " , " << high2 << " )" << endl;

                fieldNames.push_back(field);
                constraints.emplace_back(field, pair(low1, high1), pair(low2, high2));
            }
            else
            {
                cout << "Unable to match constraints regex for line: " << line << endl;
                exit(1);
            }

            getline(inputFile, line);
        }

        // read "my ticket"
        getline(inputFile, line);
        // read the ticket numbers
        getline(inputFile, line);
        istringstream ss(line);

        // read each value
        string numStr;
        while (getline(ss, numStr, ','))
        {
            uint32_t num;
            istringstream numSs(numStr);
            numSs >> num;

            myTicket.push_back(num);
        }

        // read empty line
        getline(inputFile, line);

        // create validTickets as column representation (i.e. a ticket i will be represented by validTickets[:][i])
        // validTickets will be of size (# of fields, # of tickets)
        validTickets.resize(fieldNames.size());

        // read "nearby tickets"
        getline(inputFile, line);
        while (!inputFile.eof())
        {
            // read the ticket numbers
            getline(inputFile, line);
            istringstream ss(line);

            vector<vector<bool>> ticket;    // vector of bools, where ticket[i][j] = true if value i satisfies constraints of field j
            bool isValid = true;

            // read each value
            string valueStr;
            while(getline(ss, valueStr, ',') && isValid)
            {
                // for each field value, assume that it is not valid
                isValid = false;

                uint32_t value;
                istringstream valueSs(valueStr);
                valueSs >> value;

                // check the field value against the field constraints
                vector<bool> constraintsSatisfied;
                for (const auto& constraint : constraints)
                {
                    const auto [fieldName, interval1, interval2] = constraint;

                    const auto constraintSatisfied =
                        (interval1.first <= value && value <= interval1.second) ||
                        (interval2.first <= value && value <= interval2.second);

                    constraintsSatisfied.push_back(constraintSatisfied);
                }

                isValid = accumulate(constraintsSatisfied.begin(), constraintsSatisfied.end(), 0) != 0;
                if (isValid)
                {
                    ticket.push_back(constraintsSatisfied);
                }
            }

            if (isValid)
            {
                for (size_t i = 0; i < ticket.size(); ++i)
                {
                    validTickets[i].push_back(ticket[i]);
                }
            }
        }
    }

    const auto numValidTickets = validTickets.front().size();

    // count how many tickets satisfy each field, for each particular value position
    // valuePosToSatisfiedFields will be of size (# of fields, # of fields)
    // where valuePosToSatisfiedFields[i][j] = # of tickets for which the value at position i satisfies field constraints j
    vector<vector<uint32_t>> valuePosToSatisfiedFields;
    for (const auto& valueRow : validTickets)
    {
        vector<uint32_t> sums(fieldNames.size(), 0);
        for (const auto& constraintsSatisfied : valueRow)
        {
            transform(sums.begin(), sums.end(), constraintsSatisfied.begin(), sums.begin(), plus<uint32_t>());
        }

        valuePosToSatisfiedFields.emplace_back(sums);
    }

    vector<pair<size_t, vector<size_t>>> valuePosToAvailableFields;
    for (size_t valuePos = 0; valuePos < fieldNames.size(); ++valuePos)
    {
        vector<size_t> availableFields;
        for (size_t fieldIdx = 0; fieldIdx < fieldNames.size(); ++fieldIdx)
        {
            if (valuePosToSatisfiedFields[valuePos][fieldIdx] == numValidTickets)
            {
                availableFields.push_back(fieldIdx);
            }
        }

        valuePosToAvailableFields.emplace_back(valuePos, availableFields);
    }

    // sort the vector of available fields decreasing, such that the last element is the most "constrained" value position
    // (i.e. it has the fewest options of fields to select from)
    sort(
        valuePosToAvailableFields.begin(),
        valuePosToAvailableFields.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return lhs.second.size() > rhs.second.size();
        });

    map<string, size_t> fieldToValuePos;  // map from field name to the value position on a ticket
    while (valuePosToAvailableFields.size() > 0)
    {
        // get most constrained value position (with fewest options to pick from for the field)
        const auto fewestOptionsValuePos = valuePosToAvailableFields.back();
        valuePosToAvailableFields.pop_back();

        // grab assignment of value position to field (pick first field that this value position satisfies)
        const auto satisfiedFieldIdx = fewestOptionsValuePos.second.front();

        // assign it
        fieldToValuePos[fieldNames[satisfiedFieldIdx]] = fewestOptionsValuePos.first;

        // for all the remaining value positions, remove this option from their options for field
        for (auto& e : valuePosToAvailableFields)
        {
            e.second.erase(remove(e.second.begin(), e.second.end(), satisfiedFieldIdx), e.second.end());
        }

        // re-sort the array
        sort(
            valuePosToAvailableFields.begin(),
            valuePosToAvailableFields.end(),
            [](const auto& lhs, const auto& rhs)
            {
                return lhs.second.size() > rhs.second.size();
            });
    }

    // print mapping
    // for (const auto& [fieldName, valuePos] : fieldToValuePos)
    // {
    //     cout << fieldName << " : " << valuePos << endl;
    // }

    uint64_t interestingFieldsValueProduct = 1;
    for (const auto& fieldName : INTERESTING_FIELDS)
    {
        const auto valuePos = fieldToValuePos[fieldName];
        interestingFieldsValueProduct *= myTicket[valuePos];
    }

    cout << interestingFieldsValueProduct << endl;
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