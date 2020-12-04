#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <istream>
#include <regex>

using namespace std;

const regex PASSPORT_FIELD("([a-z]+):(.*)");

class Passport
{
    public:
        void setByr(string s)
        {
            byr = s;
            hasByr = true;

            if (byr.size() == 4)
            {
                size_t year;
                istringstream ss(byr);
                ss >> year;

                if (year >= 1920 && year <= 2002)
                {
                    byrValid = true;
                }
            }
        }

        void setIyr(string s)
        {
            iyr = s;
            hasIyr = true;

            if (iyr.size() == 4)
            {
                size_t year;
                istringstream ss(iyr);
                ss >> year;

                if (year >= 2010 && year <= 2020)
                {
                    iyrValid = true;
                }
            }
        }

        void setEyr(string s)
        {
            eyr = s;
            hasEyr = true;

            if (eyr.size() == 4)
            {
                size_t year;
                istringstream ss(eyr);
                ss >> year;

                if (year >= 2020 && year <= 2030)
                {
                    eyrValid = true;
                }
            }
        }

        void setHgt(string s)
        {
            hgt = s;
            hasHgt = true;

            regex rx("([0-9]+)(cm|in)");
            smatch match;
            if (regex_match(hgt, match, rx))
            {
                if (match[2].str() == "cm")
                {
                    size_t value;
                    istringstream ss(match[1].str());
                    ss >> value;

                    if (value >= 150 && value <= 193)
                    {
                        hgtValid = true;
                    }
                }
                else if (match[2].str() == "in")
                {
                    size_t value;
                    istringstream ss(match[1].str());
                    ss >> value;

                    if (value >= 59 && value <= 76)
                    {
                        hgtValid = true;
                    }
                }
            }
        }

        void setHcl(string s)
        {
            hcl = s;
            hasHcl = true;

            regex rx("#[a-z0-9]{6}");
            smatch match;
            if (regex_match(hcl, match, rx))
            {
                hclValid = true;
            }
        }

        void setEcl(string s)
        {
            ecl = s;
            hasEcl = true;

            if (ecl == "amb" ||
                ecl == "blu" ||
                ecl == "brn" ||
                ecl == "gry" ||
                ecl == "grn" ||
                ecl == "hzl" ||
                ecl == "oth")
            {
                eclValid = true;
            }
        }

        void setPid(string s)
        {
            pid = s;
            hasPid = true;

            regex rx("[0-9]{9}");
            smatch match;
            if (regex_match(pid, match, rx))
            {
                pidValid = true;
            }
        }

        void setCid(string s)
        {
            cid = s;
            hasCid = true;
        }

        bool isValidSimple()
        {
            return
                hasByr &&
                hasIyr &&
                hasEyr &&
                hasHgt &&
                hasHcl &&
                hasEcl &&
                hasPid;
        }

        bool isValidWithRules()
        {
            return
                byrValid &&
                iyrValid &&
                eyrValid &&
                hgtValid &&
                hclValid &&
                eclValid &&
                pidValid;
        }

    public:
        string byr;
        bool hasByr = false;
        bool byrValid = false;

        string iyr;
        bool hasIyr = false;
        bool iyrValid = false;

        string eyr;
        bool hasEyr = false;
        bool eyrValid = false;

        string hgt;
        bool hasHgt = false;
        bool hgtValid = false;

        string hcl;
        bool hasHcl = false;
        bool hclValid = false;

        string ecl;
        bool hasEcl = false;
        bool eclValid = false;

        string pid;
        bool hasPid = false;
        bool pidValid = false;

        string cid;
        bool hasCid = false;
        bool cidValid = false;
};

ostream& operator<<(ostream &strm, const Passport& p)
{
    return strm << "Passport( ( " << p.byr << ", " << p.byrValid << " ), "
                        << "( " << p.iyr << ", " << p.iyrValid << " ), "
                        << "( " << p.eyr << ", " << p.eyrValid << " ), "
                        << "( " << p.hgt << ", " << p.hgtValid << " ), "
                        << "( " << p.hcl << ", " << p.hclValid << " ), "
                        << "( " << p.ecl << ", " << p.eclValid << " ), "
                        << "( " << p.pid << ", " << p.pidValid << " ), "
                        << "( " << p.cid << ", " << p.cidValid << " ) )";
}

void part1(istream& inputFile)
{
    size_t validPassports = 0;
    vector<Passport> passports;
    bool newPassport = true;

    while (!inputFile.eof())
    {
        // read one line from the file
        string line;
        getline(inputFile, line);

        if (newPassport)
        {
            passports.emplace_back();
        }

        if (line.size() == 0)
        {
            newPassport = true;
        }
        else
        {
            newPassport = false;
            Passport& p = passports.back();

            stringstream ss(line);

            string fieldValuePair;
            while (getline(ss, fieldValuePair, ' '))
            {
                smatch match;
                if (regex_match(fieldValuePair, match, PASSPORT_FIELD))
                {
                    if (match[1].str() == "byr")
                    {
                        p.setByr(match[2].str());
                    }
                    else if (match[1].str() == "iyr")
                    {
                        p.setIyr(match[2].str());
                    }
                    else if (match[1].str() == "eyr")
                    {
                        p.setEyr(match[2].str());
                    }
                    else if (match[1].str() == "hgt")
                    {
                        p.setHgt(match[2].str());
                    }
                    else if (match[1].str() == "hcl")
                    {
                        p.setHcl(match[2].str());
                    }
                    else if (match[1].str() == "ecl")
                    {
                        p.setEcl(match[2].str());
                    }
                    else if (match[1].str() == "pid")
                    {
                        p.setPid(match[2].str());
                    }
                    else if (match[1].str() == "cid")
                    {
                        p.setCid(match[2].str());
                    }
                    else
                    {
                        cout << "Unknown field : [ " << match[1].str() << " ][ " << match[2].str() << " ]" << endl;
                    }
                }
            }
        }
    }

    for_each(passports.begin(), passports.end(),
        [&validPassports](auto& p)
        {
            validPassports += p.isValidSimple() ? 1 : 0;
        });

    cout << validPassports << endl;
}

void part2(istream& inputFile)
{
    size_t validPassports = 0;
    vector<Passport> passports;
    bool newPassport = true;

    while (!inputFile.eof())
    {
        // read one line from the file
        string line;
        getline(inputFile, line);

        if (newPassport)
        {
            passports.emplace_back();
        }

        if (line.size() == 0)
        {
            newPassport = true;
        }
        else
        {
            newPassport = false;
            Passport& p = passports.back();

            stringstream ss(line);

            string fieldValuePair;
            while (getline(ss, fieldValuePair, ' '))
            {
                smatch match;
                if (regex_match(fieldValuePair, match, PASSPORT_FIELD))
                {
                    if (match[1].str() == "byr")
                    {
                        p.setByr(match[2].str());
                    }
                    else if (match[1].str() == "iyr")
                    {
                        p.setIyr(match[2].str());
                    }
                    else if (match[1].str() == "eyr")
                    {
                        p.setEyr(match[2].str());
                    }
                    else if (match[1].str() == "hgt")
                    {
                        p.setHgt(match[2].str());
                    }
                    else if (match[1].str() == "hcl")
                    {
                        p.setHcl(match[2].str());
                    }
                    else if (match[1].str() == "ecl")
                    {
                        p.setEcl(match[2].str());
                    }
                    else if (match[1].str() == "pid")
                    {
                        p.setPid(match[2].str());
                    }
                    else if (match[1].str() == "cid")
                    {
                        p.setCid(match[2].str());
                    }
                    else
                    {
                        cout << "Unknown field : [ " << match[1].str() << " ][ " << match[2].str() << " ]" << endl;
                    }
                }
            }
        }
    }

    for_each(passports.begin(), passports.end(),
        [&validPassports](auto& p)
        {
            // cout << p << endl;
            validPassports += p.isValidWithRules() ? 1 : 0;
        });

    cout << validPassports << endl;
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