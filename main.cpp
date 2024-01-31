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
#include <limits>
#include <algorithm>

#include <deque>
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
    kSwap,
    kDelete,
    kInsert
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
        os << "l";
        break;
    case ModificationType::kChange:
        os << "c";
        break;
    case ModificationType::kSwap:
        os << "s";
        break;
    case ModificationType::kDelete:
        os << "d";
        break;
    case ModificationType::kInsert:
        os << "i";
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

bool manualFind(ModificationType t, const vector<ModificationType> &mods) {
    for (auto it = mods.begin(); it != mods.end(); it++) {
        if (*it == t) {
            return true;  // Return iterator to the found element
        }
    }
    return false;  // Return end iterator if the element is not found
}
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
    string mod;

    // Constructor
    DictionaryEntry(const string &w) : word(w), 
    discovered(false), previousIndex(numeric_limits<size_t>::max()), 
    modType(ModificationType::kNone), modIndex(numeric_limits<size_t>::max()), 
    mod("") {}
};

// Define the Dictionary class
class Dictionary
{
public:
    // Add member functions as needed
    string reverseString(const string& str) {
        string reversedStr = str;
        size_t len = reversedStr.length();
        for (size_t i = 0; i < len / 2; ++i) {
            swap(reversedStr[i], reversedStr[len - i - 1]);
        }
        return reversedStr;
    }

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

                    case '&':
                        // Reversal: add both the word and its reversal to the dictionary
                        specialChars = "&";
                        addEntry(word);
                        addEntry(reverseString(word));

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
            cout << entry.word << '\n';
        }
    }
    // should I pass in opt by reference?
    void search(Options opt)
    {
        bool beginFound = false;
        bool endFound = false;
        deque<size_t> path;

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
            // spec error message
            cerr << "Beginning word does not exist in the dictionary" << endl;
            cerr << " Beginning word: " << opt.begin << endl;
            exit(1);
        }

        if (!endFound)
        {
            // spec error message
            cerr << "Ending word does not exist in the dictionary" << endl;
            cerr << " Ending Word: " << opt.end << endl;
            exit(1);
        }

        int discoveredCount;
        path = searchAssist(opt, discoveredCount);

        
        if (path.empty()) {
            // print no solution
            cout << "No solution, " << discoveredCount << " words discovered." << endl;
        }
        else 
        {
            // print out message
            cout << "Words in morph: " << path.size() << '\n';

            // OutputMode::kModiciations::kMorph
            if (opt.outputMode == OutputMode::kMorph) {
                cout << opt.begin << endl;
                path.pop_front();
                for (const auto& id: path) {
                    cout << entries[id].modType << "," << entries[id].modIndex << "," << entries[id].mod << '\n';
                }

            }
            
            else { // Default OutputMode::kWords
                for (const auto& id: path) {
                    cout << entries[id].word << "\n";
                }
            }
            
        }
    }

    deque<size_t> searchAssist(Options opt, int &discoveredCount)
    {
        // int count = 0; // for testing
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
                // check to see if word has already been discovered or first word
                if (entries[i].discovered)
                {
                    // if no, skip it
                    continue;
                }
                // check to see if word is neighbor

                if ((isNeighbor(nextID, i, opt.Modifications)))
                {
                    entries[i].discovered = true;
                    entries[i].previousIndex = nextID;
                    // entries[i].modLetter = entries[i].word[entries[i].modIndex];
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
        if (!path.empty())
        {
            // there's no solution and we can return an empty path
        // otherwise, build the path from end back to beginning and reverse
        // int count = 0; // testing
        while (entries[path.front()].previousIndex != numeric_limits<size_t>::max())
        {
            // Output mode words
            path.push_front(entries[path.front()].previousIndex);

            // Output mode Morph
            // if (opt.outputMode == OutputMode::kMorph && count < path.size()) {
            //     entries[count].modLetter = (entries[path.front()].previousIndex);
            // }
        }
        }
        return path;
    }


    bool isNeighbor(size_t sourceID, size_t otherID, const vector<ModificationType> &mods)
    {
        string w1 = entries[sourceID].word;
        string w2 = entries[otherID].word;
        int sizeComp = static_cast<int>(w1.size() - w2.size());
        int swaps = 0;
        vector<size_t> diffs;
        // if the sizes are off by more than one, return false
        if (sizeComp > 1 || sizeComp < -1) return false;

        bool checkChange = manualFind(ModificationType::kChange, mods);
        bool checkLength = manualFind(ModificationType::kLength, mods);
        bool checkSwap = manualFind(ModificationType::kSwap, mods);
        // check indifidual characters to see how many are different
        if (sizeComp == 0 && (checkChange || checkSwap)) {
            for (size_t i = 0; i < w1.size(); i++) {
                if (w1[i] != w2[i]) {
                    diffs.push_back(i);

                    if ((i < w1.size() - 1) && (w1[i] == w2[i + 1]) && (w1[i + 1] == w2[i])) swaps++;
                }
            }
        }

        // Change
        if (!diffs.empty()) {
            switch (diffs.size())
            {
            case 1: 
                 if (!checkChange) break;
                 entries[otherID].modIndex = diffs[0];
                 entries[otherID].modType = ModificationType::kChange;
                 entries[otherID].mod = w2[diffs[0]];
                 return true;
            case 2:
                if (!checkSwap) break;
                entries[otherID].modIndex = diffs[0];
                entries[otherID].modType = ModificationType::kSwap;
                entries[otherID].mod = w2.substr(diffs[0], 2);
                return true;

            default:
                break;
            }
        }

        // Length
        if (checkLength) {
            if (sizeComp == 1) {
                entries[otherID].modType = ModificationType::kDelete;
                return true;
            } else if (sizeComp == -1) {
                entries[otherID].modType = ModificationType::kInsert;
                return true;
            }
        }

        // Swap
        /*
        else if (checkSwap) {
            if (swaps == 1) {
                entries[otherID].modType = ModificationType::kSwap;
                return true;
            }
        }
*/
        return false;
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
                // spec error message
                // The first line of error output has to be a 'fixed' message
                // for the autograder to show it to you.
                cerr << "Invalid output mode specified" << endl;
                // The second line can provide more information, but you
                // won't see it on the AG.
                cerr << "  Output mode: " << arg << endl;
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
                cerr << "Beginning word not specified" << endl;
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
                cerr << "Ending word not specified" << endl;
                exit(1);
            } // if ..arg valid
            options.end = arg;
            break;
        }

        case 'q':
        {
            if (options.routeMode != RouteMode::kNone)
            {
                cerr << "Conflicting or duplicate stack and queue specified" << endl;
                exit(1);
            }
            options.routeMode = RouteMode::kQueue;
            break;
        }
        case 's':
        {
            if (options.routeMode != RouteMode::kNone)
            {
                cerr << "Conflicting or duplicate stack and queue specified" << endl;
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
            cerr << "Unknown command line option" << endl;
            exit(1);
        } // switch ..choice
    }     // while
    if (options.routeMode == RouteMode::kNone)
    {
        cerr << "Must specify one of stack or queue" << endl;
        exit(1);
    } // if ..mode
    if (options.Modifications.empty())
    {
        cerr << "Must specify at least one modification mode (change length swap)" << endl;
        exit(1);
    }
    if (options.end.size() != options.begin.size() && manualFind(ModificationType::kChange, options.Modifications))
    {                                       
        cerr << "The first and last words must have the same length when length mode is off" << endl;
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
