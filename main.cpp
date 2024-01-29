// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

// EECS 281, Project 1 Letterman
// Learn about:
//   1) Command-line processing with getopt_long()
//   2) Using enum to add readability to a limited option set
//   2) The vector member functions .resize() and .reserve()
//   3) The proper way to read data until end-of-file

#include <getopt.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <limits>
#include <tuple>

using namespace std;

// These modes represent the different ways to process the input data.
// There are three modes and the state where no mode has been selected.
enum class RouteMode
{
    kNone = 0,
    kStack,
    kQueue
}; // Mode{}

ostream &operator<<(ostream &os, const RouteMode &m)
{
    switch (m)
    {
    case RouteMode::kNone:
        os << "kNone";
        break;

    case RouteMode::kStack:
        os << "kStack";
        break;
    case RouteMode::kQueue:
        os << "kQueue";
        break;
    }
    return os;
}

enum class OutputMode
{
    kNone = 0,
    kWords,
    kMorph
}; // Mode{}

ostream &operator<<(ostream &os, const OutputMode &m)
{
    switch (m)
    {
    case OutputMode::kNone:
        os << "kNone";
        break;

    case OutputMode::kWords:
        os << "kWords";
        break;
    case OutputMode::kMorph:
        os << "kModifications";
        break;
    }
    return os;
}

enum class ModificationType
{
    kNone = 0,
    kChange,
    kLength,
    kSwap
};

// TODO: copy for all enums
ostream &operator<<(ostream &os, const ModificationType &m)
{
    switch (m)
    {
    case ModificationType::kNone:
        os << "kNone";
        break;

    case ModificationType::kLength:
        os << "kLength";
        break;
    case ModificationType::kChange:
        os << "kChange";
        break;
    case ModificationType::kSwap:
        os << "kSwap";
        break;
    }
    return os;
}

// Print help for the user when requested.
// argv[0] is the name of the currently executing program
void printHelp(char *argv[])
{
    cout << "Usage: " << argv[0] << " [-m resize|reserve|nosize] | -h\n";
    cout << "This program is to help you learn command-line processing,\n";
    cout << "reading data into a vector, the difference between resize and\n";
    cout << "reserve and how to properly read until end-of-file." << endl;
} // printHelp()

// This struct is used to pass options read from the command line, to
// the functions that actually do the work. Project 0 is simple enough to
// not need this, but it is a technique that could be helpful in doing more
// complicated command-line processing in future projects.
struct Options
{
    RouteMode routeMode = RouteMode::kNone;
    OutputMode outputMode = OutputMode::kNone;
    vector<ModificationType> Modifications;
    string begin;
    string end;
}; // Options{}
struct DictionaryEntry
{
    string word;

    // Add any other relevant members based on your requirements
    bool discovered;
    size_t previousIndex;
    ModificationType modType;
    size_t modIndex;
    char modLetter;

    // Constructor
    DictionaryEntry(const string &w) : word(w), discovered(false), previousIndex(numeric_limits<size_t>::max()) {}
};

// Define the Dictionary class
class Dictionary
{
public:
    // Add member functions as needed

    // Read Simple Dictionary
    void readDictionary()
    {
        char dictType;
        int numLines;

        // Read dictionary type (S for simple)
        cin >> dictType;
        // cout << "Dict type: " << dictType << endl;
        if (dictType != 'S' && dictType != 'C')
        {
            cerr << "Error: Invalid dictionary type." << endl;
            exit(1);
        }

        // Read the number of lines in the dictionary
        cin >> numLines;
        entries.reserve(static_cast<size_t>(numLines));
        // cout << "Number of Lines: " << numLines << endl;
        cin.ignore(); // Ignore the newline character after the number of lines

        // Read each word in the dictionary
        // char c;
        int i = 0;
        string word = "";
        string line = "";
        while (getline(cin, line) && i < numLines)
        {
            if (line[0] == '/')
            {
                // cout << "Skipping comment line: " << endl;
                // cout << "\t" << line << endl;
                continue;
            }

            // switch for line[i]
            // cout << "processing word: " << line << endl;
            i++;
            if (dictType == 'S')
            {
                addEntry(line);
            }
            else
            {
                string prefix = "";
                string specialChars = "";
                string postFix = "";
                string word = "";
                for (size_t j = 0; j < line.size(); j++)
                {
                    switch (line[j])
                    {

                    case '\n':
                        if (word.size() != 0)
                        {
                            addEntry(word);
                            cout << "Adding Word to Entries: " << word << endl;
                            word = "";
                            i++;
                        }
                        break;

                    case '&':
                        // Reversal: add both the word and its reversal to the dictionary
                        specialChars = "&";
                        addEntry(word);
                        reverse(word.begin(), word.end());
                        addEntry(word);

                        break;

                    case '[':
                        // Insert-each: generate words with each character inserted
                        // Example: "tre[an]d" => "tread" and "trend"
                        {
                            prefix = word;
                            j++;
                            while (line[j] != ']')
                            {
                                specialChars += line[j];
                                j++;
                            }
                            // get rest of line
                            j++;
                            postFix = line.substr(j, line.size() - j);
                            for (const char &c : specialChars)
                            {
                                word = prefix + c + postFix;
                                addEntry(word);
                            }
                            j = line.size();
                        }
                        break;

                    case '!':
                        // Swap: add both the original string and the string with swapped characters
                        prefix = word.substr(0, word.size() - 2);
                        specialChars = word.substr(word.size() - 2, 2);
                        j++;
                        postFix = line.substr(j, line.size() - j);
                        word = prefix + specialChars + postFix;
                        addEntry(word);
                        swap(specialChars[0], specialChars[1]); // Swap the last two characters
                        word = prefix + specialChars + postFix;
                        addEntry(word);
                        j = line.size();

                        break;

                    case '?':
                        // Double: add both the original string and the string with the last character doubled
                        prefix = word;
                        specialChars = word[word.size() - 1];
                        j++;
                        postFix = line.substr(j, line.size() - j);
                        word = prefix + postFix;
                        addEntry(word);
                        word = prefix + specialChars + postFix; // Double the last character
                        addEntry(word);
                        j = line.size();

                        break;

                    default:
                        // assume character is part of the word
                        word += line[j];
                        break;
                    }
                }
                if (specialChars == "")
                {
                    addEntry(word);
                }
            }
        }

        // cout << "Done reading dictionary. Number of Entries read: " << entries.size() << endl;
    }
    // Function to add an entry to the dictionary
    void addEntry(const string &word)
    {
        entries.push_back(DictionaryEntry(word));
    }

    // Function to print all entries in the dictionary
    void printDictionary() const
    {
        for (const auto &entry : entries)
        {
            cout << entry.word << endl;
        }
    }
    // should I pass in opt by reference?
    void search(Options opt)
    {
        bool beginFound = false;
        bool endFound = false;
        deque<size_t> path;
        vector<tuple<char, int, char>> output;

        for (size_t i = 0; i < entries.size(); i++)
        {
            if (entries[i].word == opt.begin)
            {
                beginFound = true;
                beginWordIndex = i;
            }
            else if (entries[i].word == opt.end)
            {
                endFound = true;
                endWordIndex = i;
            }

            if (beginFound && endFound)
            {
                // Both begin and end words are found, stop searching
                break;
            }
        }

        if (!beginFound)
        {
            cerr << "Error: Begin word '" << opt.begin << "' not found in the dictionary." << endl;
            exit(1);
        }

        if (!endFound)
        {
            cerr << "Error: End word '" << opt.end << "' not found in the dictionary." << endl;
            exit(1);
        }

        int discoveredCount;
        path = searchAssist(opt, discoveredCount, output);

        
        if (path.empty()) {
            // print no solution
            cout << "No solution, " << discoveredCount << " words discovered." << endl;
        }
        else 
        {
            // print out message
            cout << "Words in morph: " << path.size() << endl;

            // OutputMode::kModiciations::kMorph
            if (opt.outputMode == OutputMode::kMorph) {
                cout << entries[0].word << "\n";
                for (const auto& id: path) {
                    cout << get<0>(output[id])<< "," << get<1>(output[id]) << "," << get<2>(output[id]) << endl;
                }

            }
            
            else { // Default OutputMode::kWords
                for (const auto& id: path) {
                    cout << entries[id].word << "\n";
                }
            }
            
        }
    }

    deque<size_t> searchAssist(Options opt, int &discoveredCount, vector<tuple<char, int, char>> &output)
    {
        discoveredCount = 0;
        // create vector to store path
        deque<size_t> path;
        // add start word to search container, mark as discovered, and set previous index
        deque<size_t> sc{beginWordIndex};
        entries[beginWordIndex].discovered = true;

        // loop while elements are left to search
        while (!sc.empty() && path.empty())
        {
            // get the next element from the search container
            size_t nextID;
            // do so based on route mode
            if (opt.routeMode == RouteMode::kQueue)
            {
                nextID = sc.front();
                sc.pop_front();
            }
            else
            { // stack
                nextID = sc.back();
                sc.pop_back();
            }

            // check neighbors of next element
            // loop through all entris of the dictionary and check neighbor methods
            for (size_t i = 0; i < entries.size(); i++)
            {
                // check to see if word has already been discovered
                if (entries[i].discovered)
                {
                    // if no, skip it
                    continue;
                }
                // check to see if word is neighbor

                if (get<0>(isNeighbor(nextID, i, opt.Modifications)) != 'F')
                {
                    output.push_back(isNeighbor(nextID, i, opt.Modifications));
                    entries[i].discovered = true;
                    entries[i].previousIndex = nextID;
                    discoveredCount++;
                    // if is end word, you're done
                    if (i == endWordIndex)
                    {
                        path.push_back(endWordIndex);
                        break;
                    }
                    // else add neighbor to search container
                    sc.push_back(i);
                }
                
            }
        }

        // if you've gotten this far and have not found end word
        if (path.empty())
        {
            // there's no solution and we can return an empty path
            return path;
        }

        // otherwise, build the path from end back to beginning and reverse
        while (entries[path.front()].previousIndex != numeric_limits<size_t>::max())
        {
            path.push_front(entries[path.front()].previousIndex);
        }
        return path;
    }


    tuple<char, int, char> isNeighbor(size_t sourceID, size_t otherID, const vector<ModificationType> &mods)
    {
        string w1 = entries[sourceID].word;
        string w2 = entries[otherID].word;
        int sizeComp = static_cast<int>(w1.size() - w2.size());
        int swaps = 0;
        // variables for Tuple to be returned
        int diffs = 0;
        size_t j = 0;
        char c = 'F';
        // if the sizes are off by more than one, return false
        if (sizeComp > 1 || sizeComp < -1) return {'F', -1, 'F'};
         // check indifidual characters to see how many are different
        for (size_t i = 0; i < min(w1.size(), w2.size()) - 1; i++)
        {
            if (w1[i] != w2[i]) {
                diffs++;
                j = i;
                c = w2[j];
            }
            if ((w1[i] == w2[i + 1]) && (w1[i + 1] == w2[i])) swaps++;
        }
        
        // last character check for diffs
        if (w1[w1.size() - 1] != w2[w1.size() - 1]) diffs++;
        // last character check for swappable elements
        if (w1[w1.size() - 1] == w2[w2.size() - 2] && w1[w1.size() - 1] == w2[w2.size() - 2]) swaps++;

        // Change
        if (find(mods.begin(), mods.end(), ModificationType::kChange) != mods.end()) {
            if (diffs == 1 && sizeComp == 0) return {'c', j, c};
        }

        // Length
        else if (find(mods.begin(), mods.end(), ModificationType::kLength) != mods.end()) {
            if ((sizeComp == 1 || sizeComp == -1) && diffs == 1) return {'l', j, c};
        }

        // Swap
        else if (find(mods.begin(), mods.end(), ModificationType::kSwap) != mods.end()) {
            if (swaps == 1 && diffs == 2 && sizeComp == 0) return {'p', j, c};
        }

        return {'F', -1, 'F'};
        }

private:
    vector<DictionaryEntry> entries;
    size_t beginWordIndex = numeric_limits<size_t>::max();
    size_t endWordIndex = numeric_limits<size_t>::max();
};

void getMode(int argc, char *argv[], Options &options)
{
    // These are used with getopt_long()
    opterr = false; // Let us handle all error output for command line options
    int choice;
    int index = 0;
    option long_options[] = {
        // TODO: Fill in two lines, for the "mode" ('m') and
        // the "help" ('h') options.
        {"help", no_argument, nullptr, 'h'},
        {"queue", no_argument, nullptr, 'q'},
        {"stack", no_argument, nullptr, 's'},
        {"begin", required_argument, nullptr, 'b'},
        {"end", required_argument, nullptr, 'e'},
        {"output", required_argument, nullptr, 'o'},
        {"change", no_argument, nullptr, 'c'},
        {"length", no_argument, nullptr, 'l'},
        {"swap", no_argument, nullptr, 'p'},
    }; // long_options[]

    // TODO: Fill in the double quotes, to match the mode and help options.
    while ((choice = getopt_long(argc, argv, "hqsb:e:o:clp", long_options, &index)) != -1)
    {
        switch (choice)
        {
        case 'h':
            printHelp(argv);
            exit(0);

        case 'o':
        { // Need a block here to declare a variable inside a case
            string arg{optarg};
            if (arg != "M" && arg != "W")
            {
                // The first line of error output has to be a 'fixed' message
                // for the autograder to show it to you.
                cerr << "Error: invalid mode" << endl;
                // The second line can provide more information, but you
                // won't see it on the AG.
                cerr << "  I don't recognize: " << arg << endl;
                exit(1);
            } // if ..arg valid

            if (arg[0] == 'M')
            {
                options.outputMode = OutputMode::kMorph;
            }
            else if (arg[0] == 'W')
            {
                options.outputMode = OutputMode::kWords;
            }
            break;
        } // case 'm'
        case 'b':
        {
            string arg{optarg};
            if (arg == "")
            {
                // The first line of error output has to be a 'fixed' message
                // for the autograder to show it to you.
                cerr << "Error: must supply a begin word" << endl;
                exit(1);
            } // if ..arg valid
            options.begin = arg;
            break;
        }

        case 'e':
        {
            string arg{optarg};
            if (arg == "")
            {
                // The first line of error output has to be a 'fixed' message
                // for the autograder to show it to you.
                cerr << "Error: must supply an end word" << endl;
                exit(1);
            } // if ..arg valid
            options.end = arg;
            break;
        }

        case 'q':
        {
            if (options.routeMode != RouteMode::kNone)
            {
                cerr << "Error: You cannot specify queue and stack to routeMode at the same time" << endl;
                exit(1);
            }
            options.routeMode = RouteMode::kQueue;
            break;
        }
        case 's':
        {
            if (options.routeMode != RouteMode::kNone)
            {
                cerr << "Error: You cannot specify queue and stack to routeMode at the same time" << endl;
                exit(1);
            }
            options.routeMode = RouteMode::kStack;
            break;
        }
        case 'c':
        {
            options.Modifications.push_back(ModificationType::kChange);
            break;
        }

        case 'l':
        {
            options.Modifications.push_back(ModificationType::kLength);
            break;
        }

        case 'p':
        {
            options.Modifications.push_back(ModificationType::kSwap);
            break;
        }

        default:
            cerr << "Error: invalid option" << endl;
            exit(1);
        } // switch ..choice
    }     // while
    if (options.routeMode == RouteMode::kNone)
    {
        cerr << "Error: no mode specified - Must specify routeMode like queue or stack" << endl;
        exit(1);
    } // if ..mode
    if (options.Modifications.empty())
    {
        cerr << "Error: no modifications specified - Must specify at least one of change(c), length(l), or swap(p)" << endl;
        exit(1);
    }
    if (options.end.size() != options.begin.size() && find(options.Modifications.begin(), options.Modifications.end(), ModificationType::kLength) == options.Modifications.end())
    {
        cerr << "Error: Must specify length modification when begin and end words are different lengths" << endl;
    }
} // getMode()

// This function is already done.
int main(int argc, char *argv[])
{
    Options opt;
    getMode(argc, argv, opt);
    // cout << "begin: " << opt.begin << endl;
    // cout << "end: " << opt.end << endl;
    // cout << "output mode: " << opt.outputMode << endl;
    // cout << "route mode: " << opt.routeMode << endl;
    // cout << "Modifications: " << endl;
    // for (auto mod: opt.Modications) {
    //     cout << mod << endl;
    // }
    Dictionary d;
    d.readDictionary();
    // cout << "Dictionary: \n";
    // d.printDictionary();
    // cout << "Search Start: \n";
    d.search(opt);
    return 0;

} // main()
