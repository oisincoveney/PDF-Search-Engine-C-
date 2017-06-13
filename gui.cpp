#include "gui.h"


/**
 * clearAll()
 *
 * Clears all variables associated with the PDF index, including variables
 * within the Parser class that contain analytic data
 */
void GUI::clearAll()
{
    delete index;
    index = nullptr;
    Parser::numFiles = 0;
    Parser::numPages = 0;
    Parser::numWords = 0;
    Parser::totalWordsInDoc.clear();
    directory = "";
}

/**
 * searchIndex()
 *
 * Provides a user interface to search the PDF index.
 *
 * When a user decides to search, the index is first checked to determine
 * whether the index is valid and should be searched. If not, the user
 * will be prompted to build the index.
 *
 * Otherwise, the user will input a simple prefix boolean search query
 * as a search query. The query is then processed and the results are
 * outputted to the console. The user then has the option to open
 * the PDFs by scrolling through the list, and can return to the main menu
 * through an "exit" option.
 */
void GUI::searchIndex()
{
    clear();
    windowBorder();

    //Builds index before any search is made
    if(index == nullptr || index->empty())
    {
        buildIndex();
    }

    //Prompt and example for the user
    attron(A_BOLD);
    mvprintw(row / 2 - 4, (col - 30) / 2, "Searching the index...");
    attroff(A_BOLD);
    mvprintw(row / 2, (col - 57) / 2, "Please enter a PREFIX BOOLEAN query "
                                      "using AND, OR, or NOT");
    mvprintw(row / 2 + 1, (col - 37) / 2, "to search for words "
                                          "within the index.");
    mvprintw(row / 2 + 2, (col - 62) / 2, "Examples: \"AND book Boston NOT "
                                          "Seattle\" or \"OR Boston Dallas\"" );
    mvprintw(row / 2 + 4, (col - 80) / 2, "-> ");

    refresh();

    //Allow user input
    echo();
    char input[150];
    getstr(input);
    std::string in{input};

    //Gets the word created by the query processor
    Word word{QueryProcessor::process(in, index)};
    noecho();
    //End user input

    clear();
    windowBorder();


    //If no results are found, the user receives a prompt saying that
    // no results are found.
    if(word.empty())
    {
        mvprintw(row / 2 - 1, (col - 10) / 2, "The search");
        attron(A_BOLD);
        mvprintw(row / 2, (col - in.size()) / 2, in.c_str());
        attroff(A_BOLD);
        mvprintw(row / 2 + 1, (col - 28) / 2, "did not receive any results.");
        mvprintw(row / 2 + 3, (col - 41) / 2, "Press any key to return "
                                              "to the main menu.");
        refresh();
        getch();
        clear();
        windowBorder();
        return;
    }


    //Otherwise, output the word

    //Header with bolded word
    attron(A_BOLD | A_UNDERLINE);
    mvprintw((row - 18) / 3, (col - word.getWord().size()) / 2,
             word.getWord().c_str());
    attroff(A_BOLD | A_UNDERLINE);

    //Subheader showing the directory
    std::string direct = "In " + directory;
    mvprintw((row - 18) / 3 + 2, (col - direct.size()) / 2, direct.c_str());


    //Header for list of documents
    attron(A_BOLD | A_UNDERLINE);
    mvprintw(row / 3 + 1, 25, "Documents");
    mvprintw(row / 3 + 1, 130, "Frequency");
    mvprintw(row / 3 + 1, 145, "Relevancy Ranking");
    attroff(A_BOLD | A_UNDERLINE);


    //Vector for outputting the files to the console
    std::vector<DocDetails> docs{word.getDocs()};
    int maxSize = (int) docs.size();

    int c{}, choice{0};

    //Menu for the list of files
    std::string file;
    //While the user has not pressed 'Enter' at the 'Exit' entry
    while(c != 10 || choice != 15)
    {
        //Outputs the files and highlights the selected choice by the user
        for(int i{}; i < 15 && i < maxSize; i++)
        {
            if(choice == i)
                attron(A_STANDOUT);

            DocDetails& d = docs[i];

            file = d.name;

            file = file.substr(directory.size());

            if(file.size() > 100)
            {
                file = "..." + file.substr(file.size() - 97);
            }

            mvprintw(row / 3 + 3 + i, 25, file.c_str());
            mvprintw(row / 3 + 3 + i, 130, std::to_string(d.frequency).c_str());
            mvprintw(row / 3 + 3 + i, 145, std::to_string(d.ranking).c_str());

            if(choice == i)
                attroff(A_STANDOUT);
        }

        //Outputs the exit option
        if(choice == 15)
        {
            attron(A_STANDOUT);
        }
        mvprintw(row / 3 + 19, 25, "Exit");

        if(choice == 15)
        {
            attroff(A_STANDOUT);
        }

        c = getch();

        //Allows for scrolling from top to bottom when overflowing from menu
        if(c == 258)
        {
            if(choice == 15)
                choice = 0;
            else
                choice ++;
        }
        else if(c == 259)
        {
            choice--;
            if(choice < 0)
                choice = 15;
        }

        //Opens the PDF in the preferred PDF reader
        if(c == 10 && choice != 15)
        {
            QDesktopServices::openUrl
                    (QUrl::fromLocalFile((docs[choice].name.c_str())));
        }

        refresh();
    }

    clear();
    windowBorder();

}

/**
 * buildIndex()
 *
 * The parent function that provides a user interface for building an index
 * from either a directory of PDF files or from a persistent index that
 * was previously created by a previous instance of the PDF index.
 *
 * The function contains multiple stages:
 * 1) If the index is non-existent, the user is given the option to choose
 *      a data structure for the index
 * 2) If the index already exists, the user is reminded that the index
 *      is already populated, and asks if the user wants to continue
 * 3) If there is a persistent index, the function will inform the user
 *      and ask if the user wants to load the index from the file.
 * 4) If the user chooses not to load the index file, the function
 *      will then allow the user to build the index from a directory of PDFs
 *
 */
void GUI::buildIndex()
{
    clear();
    windowBorder();

    //Choose data structure if the index is non-existent
    if(index == nullptr)
    {
        chooseIndex();
    }
    //If the index exists and has information in it, remind the user
    else if (!index->empty())
    {
        //Reminder prompt
        attron(A_BOLD);
        mvprintw(row / 2 - 1, (col - 37) / 2, "The index has already been populated.");
        attroff(A_BOLD);
        mvprintw(row / 2 + 1, (col - 42) / 2, "Are you sure you want to reload the index?");

        //Menu for yes/no question
        int c{}, choice{};
        while(c != 10)
        {
            highlightMenuItem(choice, boolChoice, 2, row / 2 + 3);
            c = getch();

            if(c == 258)
                choice++;

            if(c == 259)
                choice--;

            if(choice < 0)
                choice = 0;
            else if(choice > 1)
                choice = 1;

            refresh();
        }


        //If user chooses no, the function returns. Otherwise, everything
        //is cleared and the user chooses an index
        if(choice)
        {
            clear();
            windowBorder();
            return;
        }
        else
        {
            clearAll();
            chooseIndex();
        }
    }


    clear();
    windowBorder();


    //Checks if the persistent index file exists and is populated
    std::string directoryCheck = index->getDirectoryFromFile();

    //If it exists and is populated, ask if the
    // user if they want to load the file
    if(directoryCheck != "")
    {
        //Options for user
        const char* indexOptions[2] =
        {"Reload the index", "Load it from file"};

        //Header and menu for file loading choice
        int c{}, choice{};
        attron(A_BOLD);
        mvprintw(row / 2 - 3, (col - 13) / 2, "The index for");
        attroff(A_BOLD);
        mvprintw(row / 2 - 2, (col - directoryCheck.size()) / 2, directoryCheck.c_str());
        attron(A_BOLD);
        mvprintw(row / 2 - 1, (col - 32) / 2, " already exists in the "
                                              "text file.");
        attroff(A_BOLD);
        mvprintw(row / 2 + 1, (col - 58) / 2, "Do you want to reload the index, "
                                              "or load it from the file?");

        //Menu
        while(c != 10)
        {
            highlightMenuItem(choice, indexOptions, 2, row / 2 + 3);

            c = getch();

            if(c == 258)
                choice++;

            if(c == 259)
                choice--;

            if(choice < 0)
                choice = 0;
            else if(choice > 1)
                choice = 1;

            refresh();
        }

        //If the user chooses to load from file
        if(choice)
        {
            directory = directoryCheck;
            buildFromFile();
            return;
        }

    }

    //User has denied all options, and will reload index
    buildFromScratch();
    return;
}

/**
 * buildFromFile()
 *
 * Builds the index from the persistent index file created by the
 * IndexInterface object from previous sessions. The function assumes
 * that the index exists since the event of a nullptr index is handled
 * within the function's parent function buildIndex()
 *
 */
void GUI::buildFromFile()
{
    clear();
    windowBorder();

    //No prompt needed, gives the user indication of something happening
    mvprintw(2, 5, "Loading from file...");

    index->load();

    mvprintw(2, 5, "Done Loading. Press any key to return to the main menu.");
    refresh();
    getch();        //Pauses so user can read the indications
    clear();
    windowBorder();
}

/**
 * buildFromScratch()
 *
 * Builds the index by finding PDFs and parsing each PDF within the directory
 * given by the user. The function assumes that the index is not nullptr,
 * since that sitaution is handled by its parent function buildIndex().
 *
 */
void GUI::buildFromScratch()
{
    clear();
    windowBorder();

    //Header and prompt
    attron(A_BOLD);
    mvprintw(row / 2 - 3, (col - 30) / 2, "Loading the index from scratch");
    attroff(A_BOLD);
    mvprintw(row / 2 - 1, (col - 44) / 2, "Enter THE FULL PATH you would like to parse:");
    mvprintw(row / 2 + 2, (col - 80) / 2, "-> ");
    refresh();

    //Allow user input
    echo();
    char input[150];
    getstr(input);


    //Checks if the file is both valid and is a directory
    while(!std::experimental::filesystem::exists(input)
          && !std::experimental::filesystem::is_directory(input))
    {
        mvprintw(row / 2 + 4, (col - 91) / 2, "The directory you entered "
                                              "does not exist or is invalid. "
                                              "Please enter a different "
                                              "directory.");
        for(int i{3}; i < col - 3; i++)
        {
            mvaddch(row / 2 + 2, i, ' ');
        }
        mvprintw(row / 2 + 2, (col - 80) / 2, "-> ");
        refresh();
        getstr(input);
    }
    noecho();
    //end user input

    directory = input;

    clear();
    windowBorder();

    mvprintw(2, 5, "Parsing...");

    Parser::parse(directory, index, row, col);
    index->printToFile();

    mvprintw(2, 5, "Done Parsing. Press any key to return to the main menu.");
    refresh();
    getch();
    clear();
    windowBorder();
}

/**
 * addNewFiles()
 *
 * Provides a user interface to add either a file or directory to the index.
 * If the index is non-existent, the user will be prompted to determine an
 * index type and build the index, returning to the menu without adding more
 * files. Otherwise, a file or directory is added using the addFile or
 * addDirectory functions.
 *
 */
void GUI::addNewFiles()
{
    clear();
    windowBorder();

    //Builds the index if the index doesn't exist
    if(index == nullptr || index->empty())
    {
        buildIndex();
        return;
    }

    //Prompt
    attron(A_BOLD);
    mvprintw(row / 2-3, (col - 57) / 2, "Would you like to add a file "
                                        "or a directory to the index?");
    attroff(A_BOLD);


    //Menu for the prompt
    int c{}, choice{};
    const char* options[2]{"Add file", "Add directory"};
    while(c != 10)
    {
        highlightMenuItem(choice, options, 2, row / 2);
        c = getch();

        if(c == 258)
            choice++;

        if(c == 259)
            choice--;

        if(choice < 0)
            choice = 0;
        else if(choice > 1)
            choice = 1;

        refresh();
    }

    //Based on user choice
    if(choice)
    {
        addDirectory();
    }
    else
    {
        addFile();
    }

    //Resets screen
    clear();
    windowBorder();
    return;
}

/**
 * addDirectory()
 *
 * Provides a user interface for adding a complete directory of PDF files
 * to the current index. The function assumes that the index already exists,
 * which is handled by its parent function addNewFiles().
 *
 */
void GUI::addDirectory()
{
    clear();
    windowBorder();

    //Header and prompt
    attron(A_BOLD);
    mvprintw(row / 2 - 3, (col - 20) / 2, "Adding new directory");
    attroff(A_BOLD);
    mvprintw(row / 2 - 1, (col - 44) / 2, "Enter THE FULL PATH you "
                                          "would like to parse:");
    mvprintw(row / 2 + 2, (col - 80) / 2, "-> ");

    refresh();

    //Allow user input
    echo();
    char input[150];
    getstr(input);

    //Check if directory exists and is actually a directory
    while(!std::experimental::filesystem::exists(input)
          && !std::experimental::filesystem::is_directory(input))
    {
        mvprintw(row / 2 + 4, (col - 91) / 2, "The directory you entered "
                                              "does not exist or is invalid. "
                                              "Please enter a different "
                                              "directory.");
        for(int i{3}; i < col - 3; i++)
        {
            mvaddch(row / 2 + 2, i, ' ');
        }
        mvprintw(row / 2 + 2, (col - 80) / 2, "-> ");
        refresh();
        getstr(input);
    }
    noecho();
    //End user input

    std::string in{input};
    clear();
    windowBorder();

    //Presents the parsing screen for a beautiful representation of parsing
    mvprintw(2, 5, "Parsing new directory...");

    Parser::addExtraDirectory(in, index, row, col);
    index->printToFile();

    mvprintw(2, 5, "Done Parsing. Press any key to return to the main menu.");
    refresh();
    getch();                                //Pauses for user input
    clear();
    windowBorder();
}


/**
 * addFile()
 *
 * Gives the user a user interface to add a file to the index. The user is
 * prompted for a file name, which is then added to the index using the
 * static class Parser's parseFile function.
 *
 */
void GUI::addFile()
{
    clear();
    windowBorder();

    //Header and prompt for user
    attron(A_BOLD);
    mvprintw(row / 2 - 3, (col - 13) / 2, "Adding a file");
    attroff(A_BOLD);

    mvprintw(row / 2 - 1, (col - 56) / 2, "Enter THE FULL PATH of the file "
                                          "you would like to parse:");
    mvprintw(row / 2 + 2, (col - 80) / 2, "-> ");

    refresh();

    //Allow user input
    echo();
    char input[150];
    getstr(input);


    //Check if the file is valid for parsing
    while(!std::experimental::filesystem::exists(input)
          && !std::experimental::filesystem::is_regular_file(input))
    {
        mvprintw(row / 2 + 4, (col - 86) / 2, "The file you entered does "
                                              "not exist or is invalid. "
                                              "Please enter a different "
                                              "directory.");
        for(int i{3}; i < col - 3; i++)
        {
            mvaddch(row / 2 + 2, i, ' ');
        }
        mvprintw(row / 2 + 2, (col - 80) / 2, "-> ");
        refresh();
        getstr(input);
    }
    noecho();
    //End user input

    std::string in{input};
    refresh();


    clear();
    windowBorder();

    //Parses the file and provides prompt
    Parser::addExtraDoc(in, index);

    attron(A_BOLD);
    mvprintw(row / 2, (col - 40) / 2, "New file added. Press any key "
                                      "to return.");
    attroff(A_BOLD);
    refresh();
    getch();

    clear();
    windowBorder();
}

/**
 * Clears the index, telling the user if the index is already clear if
 * index is non-existent. Otherwise, all variables relating to the index
 * will be cleared.
 */
void GUI::clearIndex()
{

    if(index == nullptr)
    {
        mvprintw(3 * row / 4, (col - 23) / 2, "Index is already clear.");
        refresh();
        return;
    }
    else
    {
        //Tells the user about the index clearing
        mvprintw(3 * row / 4, (col - 17) / 2, "Clearing Index...");
        refresh();

        //clear all variables
        clearAll();

        mvprintw(3 * row / 4, (col - 26) / 2, "      Index cleared.      ");

        //Deletes the indicator of index on top left of window
        for(int i{}; i < col - 5; i++)
        {
            mvaddch(2, i, ' ');
            mvaddch(3, i, ' ');
        }
        refresh();
        return;
    }
}

/**
 * chooseIndex()
 *
 * Allows the user to choose the data structure used for the PDF index. The
 * function first checks if the index is either empty or non-existent,
 * and will remind the user about their current index if there exists an
 * incumbent index.
 *
 * After this check, the function will clear the current index, and
 * change the index to the data structure selected by the user.
 *
 */
void GUI::chooseIndex()
{
    clear();
    windowBorder();

    //Check if the index exists and is populated
    if(index != nullptr && !index->empty())
    {
        std::string indexType{index->getDataType()};
        std::string inform{"An index with a"};

        if(indexType == "AVL tree")
            inform += "n";

        inform += ' ' + indexType + " already exists.";


        //Reminds the user about the current index
        attron(A_BOLD);
        mvprintw(row / 2 - 2, (col - inform.size()) / 2, inform.c_str());
        attroff(A_BOLD);
        mvprintw(row / 2 + 1, (col - 37) / 2, "Do you want to change the index type?");


        //Menu to determine the user's choice in keeping the index
        int c{}, choice{};
        while(c != 10)
        {
            highlightMenuItem(choice, boolChoice, 2, row / 2 + 3);
            c = getch();

            if(c == 258)
                choice++;

            if(c == 259)
                choice--;

            if(choice < 0)
                choice = 0;
            else if(choice > 1)
                choice = 1;

            refresh();
        }

        //Returns if the user decides to keep the index
        if(choice)
        {
            clear();
            windowBorder();
            return;
        }
    }


    //Clears the screen again for a new prompt
    clear();
    windowBorder();

    int c{}, choice{};

    //choice between data structures
    const char* indexOptions[2] = {"AVL Tree", "Hash Table"};

    //Tells the user about the current state of their index
    if(index == nullptr || !index->empty())
    {
        attron(A_BOLD);
        mvprintw(row / 2 - 5, (col - 34) / 2,"The index has not been loaded yet.");
        attroff(A_BOLD);
    }
    else
    {
         mvprintw(row / 2, (col - 38) / 2, "(This operation will clear the index.)");
    }

    //Prompt to get the index
    mvprintw(row / 2 - 3, (col - 50) / 2, "Would you like to use an AVL Tree or Hash Index as");
    mvprintw(row / 2 - 2, (col - 44) / 2, "the underlying data structure for the index?");

    //Menu to choose between data structures
    while(c != 10)
    {
        highlightMenuItem(choice, indexOptions, 2, row / 2 + 3);

        c = getch();

        if(c == 258)
            choice++;

        if(c == 259)
            choice--;

        if(choice < 0)
            choice = 0;
        else if(choice > 1)
            choice = 1;

        refresh();
    }

    //Clears the current data structure
    delete index;
    index = nullptr;
    Parser::numFiles = 0;
    Parser::numPages = 0;
    Parser::numWords = 0;
    directory = "";

    choice ? index = new HashIndex() : index = new AVLIndex;

    clear();
    windowBorder();
}

/**
 * displayAnalytics()
 *
 * Displays an analysis of the current index of PDF files. If the index
 * does not contain anything or is non-existent, the function will return
 * and will not display anything. Otherwise, it will display the total number
 * of words, files, and pages within the PDFs parsed, as well as the 50 most
 * frequent words within the index.
 *
 */
void GUI::displayAnalytics()
{

    //If the index hasn't been loaded, don't display anything.
    if(index == nullptr || index->empty())
    {
        mvprintw(3 * row / 4, (col - 34) / 2, "The index has not been loaded yet.");
        refresh();
        getch();
        return;
    }

    clear();
    windowBorder();


    //Otherwise, display the different elements of the analysis
    attron(A_BOLD);
    std::string str = ("Analytics for directory " + directory);
    mvprintw(5, (col - str.size()) / 2, str.c_str());
    attroff(A_BOLD);

    //Files
    str = "Total files parsed: " + std::to_string(Parser::numFiles);
    mvprintw(8, (col - str.size()) / 2, str.c_str());

    //Pages
    str = "Total pages parsed: " + std::to_string(Parser::numPages);
    mvprintw(9, (col - str.size()) / 2, str.c_str());

    //Words
    str = "Total words parsed: " + std::to_string(Parser::numWords);
    mvprintw(10, (col - str.size()) / 2, str.c_str());

    //Most frequent words
    mvprintw(15, (col - 20) / 2, "Most frequent words:");

    int maxSize{(int) index->frequentWords.size()};

    int i{};
    int offset{0};                             //Offset for displaying the words
    while(i < maxSize)
    {
        if(i == maxSize / 2)
            offset = 2;

        mvprintw(16 + (i % (maxSize / 2)), (2 + offset) * col / 7,
                 index->frequentWords[i]->getWord().c_str());
        mvprintw(16 + (i % (maxSize / 2)), (2 + offset) * col / 7 + 20,
                 std::to_string(index->frequentWords[i]->getTotalFrequency()).c_str());
        ++i;
    }

    refresh();
    getch();                                    //Pause
    clear();
    windowBorder();
}

/**
 * highlightMenuItem(int, const char**, int, int)
 *
 * Displays and highlights a specific item within the list of items given
 * through the const char** parameter. The list is displayed in the center of
 * the screen, while the integer x determines the height of the list on
 * the screen.
 *
 * @param highlight - the choice in the list to highlight
 * @param items     - the list of items to choose from
 * @param menuSize  - the size of the preceding list
 * @param x         - the row to display the first element of the list
 */
void GUI::highlightMenuItem(int highlight, const char** items, int menuSize, int x)
{
    attron(A_NORMAL);
    for(int i{}; i < menuSize; i++)
    {
        //highlights the chosen menu item from the parameter highlight
        if(i == highlight)
        {
            attron(A_STANDOUT);
            mvprintw(x + i, (col - strlen(items[i])) / 2, items[i]);
            attroff(A_STANDOUT);
        }
        else
            mvprintw(x + i, (col - strlen(items[i])) / 2, items[i]);

    }
}

/**
 * windowBorder()
 *
 * Creates a border of asterisks around the terminal screen to enclose
 * all UI elements of the program.
 */
void GUI::windowBorder()
{
    //horizontal
    for(int i{}; i < row; i++)
    {
        mvaddch(i, col - 2, '*');
        mvaddch(i, 0, '*');
    }

    //vertical
    for(int i{}; i < col; i++)
    {
        mvaddch(0, i, '*');
        mvaddch(row - 1, i, '*');
    }
}

/**
 * Constructor for the GUI
 */
GUI::GUI() : index{nullptr}, row{}, col{}
{
    initscr();                  //initializes the screen
    curs_set(0);                //removes cursor indicator
    cbreak();
    keypad(stdscr, TRUE);       //allows keyboard input
    noecho();                   //disables echo of user input
    getmaxyx(stdscr,row,col);   //gets the window size
    menu();                     //displays the menu
    endwin();                   //ends the ncurse session
}

/**
 * Destructor
 */
GUI::~GUI()
{
    if(index != nullptr)
        delete index;
}

/**
 *
 * menu()
 *
 * Displays the menu for the program.
 *
 * The menu contains 7 options:
 * 1) search
 * 2) build
 * 3) add files
 * 4) analytics
 * 5) choose index structure
 * 6) exiting the program
 *
 *
 * The menu will continue running until the user chooses to exit the program.
 *
 */
void GUI::menu()
{
    clear();                //clears the screen
    int c{}, choice{};
    while(true)
    {
        windowBorder();     //window border

        //Welcome screen
        attron(A_BOLD);
        mvprintw(row/3 - 1, (col - 33) / 2,"Welcome to the PDF Search Engine!");
        mvprintw(row/3 + 1, (col - 67) / 2,"Version 42 a.k.a. The \"I'm "
                                           "tired and I want to go to sleep\" "
                                           "edition");
        mvprintw(row/3 + 2, (col - 17) / 2, "By Oisin Coveney");
        attroff(A_BOLD);


        //highlights the item chosen by the user
        highlightMenuItem(choice, options, 7, row / 3 + 6);

        if(directory != "")
        {
            std::string str = "Current Directory: " + directory;
            mvprintw(2, 3, str.c_str());
            str = ("Files: " + to_string(Parser::getNumFiles()) + "\tPages: ");
            str += (to_string(Parser::getNumPages()) + "\tWords: ");
            str += to_string(Parser::getNumWords());
            mvprintw(3, 3, str.c_str());

        }

        //get user input
        c = getch();

        //highlight different menu items depending on input
        if(c == 258 && choice <= 5)
            choice++;

        if(c == 259 && choice >= 1)
            choice--;

        //When the user presses 'enter', the switch statement is activated
        if(c == 10)
        {
            switch(choice)
            {
                case 0:
                    searchIndex();
                    break;
                case 1:
                    buildIndex();
                    break;
                case 2:
                    addNewFiles();
                    break;
                case 3:
                    clearIndex();
                    break;
                case 4:
                    displayAnalytics();
                    break;
                case 5:
                    chooseIndex();
                    break;
                case 6:
                    printw("Exit");
                    return;
            }
        }
        refresh();
    }
    return;
}
