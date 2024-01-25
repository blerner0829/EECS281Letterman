// Project Identifier: 39E995D00DDE0519FDF5506EED902869AEC1C39E

// EECS 281, Project 0
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
using namespace std;

// These modes represent the different ways to process the input data.
// There are three modes and the state where no mode has been selected.
enum class RouteMode
{
    kNone = 0,
    kStack,
    kQueue
}; // Mode{}

ostream& operator<<(ostream& os, const RouteMode &m) {
    switch(m) 
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
    kModifications
}; // Mode{}

ostream& operator<<(ostream& os, const OutputMode &m) {
    switch(m) 
    {
        case OutputMode::kNone:
            os << "kNone";
            break;
    
        case OutputMode::kWords:
            os << "kWords";
            break;
        case OutputMode::kModifications:
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
ostream& operator<<(ostream& os, const ModificationType &m) {
    switch(m) 
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
    vector<ModificationType> Modications;
    string begin;
    string end;
}; // Options{}
struct DictionaryEntry
{
    string word;

    // Add any other relevant members based on your requirements

    // Constructor
    DictionaryEntry(const string &w) : word(w) {}
};

// Define the Dictionary class
class Dictionary
{
public:
    // Add member functions as needed

    // Read Simple Dictionary
    void readSimpleDictionary()
    {

        char dictType;
        int numLines;

        // Read dictionary type (S for simple)
        cin >> dictType;
        //cout << "Dict type: " << dictType << endl;
        if (dictType != 'S' && dictType != 'C')
        {
            cerr << "Error: Invalid dictionary type." << endl;
            exit(1);
        }

        // Read the number of lines in the dictionary
        cin >> numLines;
        //cout << "Number of Lines: " << numLines << endl;
        cin.ignore(); // Ignore the newline character after the number of lines

        // Read each word in the dictionary
        //char c;
        int i = 0;
        string word = "";
        string line = "";
        while (getline(cin, line) && i < numLines)
        {
            if (line[0] == '/')
            {
                //cout << "Skipping comment line: " << endl;
                //cout << "\t" << line << endl;
                continue;
            }

            // switch for c
            //cout << "processing word: " << line << endl;
            addEntry(line);
            i++;
            /*
            switch (c)
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
                addEntry(word);
                reverse(word.begin(), word.end());
                addEntry(word);
                word = "";
                i += 2; // Increment by 2 as both original and reversed words are added
                break;

            case '[':
                // Insert-each: generate words with each character inserted
                // Example: "tre[an]d" => "tread" and "trend"
                {
                    string baseWord = word;
                    cin >> c; // Read the first character inside square brackets
                    while (c != ']')
                    {
                        word = baseWord + c;
                        addEntry(word);
                        cin >> c; // Read the next character
                    }
                    word = "";
                    i += baseWord.size(); // Increment by the number of inserted words
                }
                break;

            case '!':
                // Swap: add both the original string and the string with swapped characters
                addEntry(word);
                swap(word[word.size() - 2], word[word.size() - 1]); // Swap the last two characters
                addEntry(word);
                word = "";
                i += 2; // Increment by 2 as both original and swapped words are added
                break;

            case '?':
                // Double: add both the original string and the string with the last character doubled
                addEntry(word);
                word += word[word.size() - 1]; // Double the last character
                addEntry(word);
                word = "";
                i += 2; // Increment by 2 as both original and doubled words are added
                break;

            default:
                // assume character is part of the word
                word += c;
                break;
            }
            */
        }
        
        //cout << "Done reading dictionary. Number of Entries read: " << entries.size() << endl;
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
    void search(Options opt) {
        bool beginFound = false;
        bool endFound = false;

        for (const auto& entry : entries) {
            if (entry.word == opt.begin) {
                beginFound = true;
            } else if (entry.word == opt.end) {
                endFound = true;
            }

            if (beginFound && endFound) {
                // Both begin and end words are found, stop searching
                break;
            }
        }

        if (!beginFound) {
            cerr << "Error: Begin word '" << opt.begin << "' not found in the dictionary." << endl;
            exit(1);
        }

        if (!endFound) {
            cerr << "Error: End word '" << opt.end << "' not found in the dictionary." << endl;
            exit(1);
        }
        if (opt.routeMode == RouteMode::kStack) {
            searchStack(opt);
        }
        if (opt.routeMode == RouteMode::kQueue) {
            searchQueue(opt);
        }
    }
    void searchStack(Options opt) {
    }
    void searchQueue(Options opt) {

    }


private:
    std::vector<DictionaryEntry> entries;
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
                options.outputMode = OutputMode::kModifications;
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
            options.Modications.push_back(ModificationType::kChange);
            break;
        }

        case 'l':
        {
            options.Modications.push_back(ModificationType::kLength);
            break;
        }

        case 'p':
        {
            options.Modications.push_back(ModificationType::kSwap);
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
    if (options.Modications.empty())
    {
        cerr << "Error: no modifications specified - Must specify at least one of change(c), length(l), or swap(p)" << endl;
        exit(1);
    }
    if (options.end.size() != options.begin.size() 
    && find(options.Modications.begin(), options.Modications.end(), ModificationType::kLength) == options.Modications.end()) 
    {
        cerr << "Error: Must specify length modification when begin and end words are different lengths" << endl;
    }
} // getMode()

// This function is already done.
int main(int argc, char *argv[])
{
    Options opt;
    getMode(argc, argv, opt);
    cout << "begin: " << opt.begin << endl;
    cout << "end: " << opt.end << endl;
    cout << "output mode: " << opt.outputMode << endl;
    cout << "route mode: " << opt.routeMode << endl;
    cout << "Modifications: " << endl;
    for (auto mod: opt.Modications) {
        cout << mod << endl;
    }
    Dictionary d;
    d.readSimpleDictionary();
    d.printDictionary();
    return 0;

} // main()
