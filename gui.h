#pragma once

//C++ libraries
#include <ncurses.h>
#include <iostream>
#include <limits>
#include <experimental/filesystem>
#include <string>
#include <stemmer.h>
#include <cstring>

//Qt libraries
#include <qurl.h>
#include <qt5/QtGui/qdesktopservices.h>

//My libraries
#include <Index/indexinterface.h>
#include <Index/avlindex.h>
#include <Index/hashindex.h>
#include <Parser/parser.h>
#include <QueryProcessor/queryprocessor.h>


using namespace std;

/**
 * The GUI class presents the user with all the functionality of the PDF
 * search engine. The class uses the ncurses library to present the user
 * with a simple user interface consisting of menus operated by the keyboard.
 *
 * The class contains functionality to build, load, and search the index, and
 * will display analytics about the parsed PDFs when the index is built.
 *
 * This class will be used as a part of the PDF Search Engine final project
 * in SMU CSE 2341.
 *
 * By: Oisin Coveney
 */
class GUI
{
    private:

        ///The directory to parse files from
        string directory{""};

        ///The main menu options
        const char* options[7] =
        {"Search", "Build Index", "Add Files",
         "Clear Index", "Statistics", "Choose index type", "Exit"};

        /// For Yes/No questions
        const char* boolChoice[2] = {"Yes", "No"};

        /// The PDF index
        IndexInterface* index;

        /// Max height and width of the terminal window
        int row, col;

        ///Clears all variables of the GUI
        void clearAll();


        /// Searching the index
        void searchIndex();

        /// Build the index
        void buildIndex();
        void buildFromFile();
        void buildFromScratch();

        /// Add files
        void addNewFiles();
        void addDirectory();
        void addFile();

        /// Clear index
        void clearIndex();

        /// choose index type
        void chooseIndex();

        /// statistics
        void displayAnalytics();

        /// UI formatting
        void highlightMenuItem(int highlight, const char** items, int menuSize, int x);
        void windowBorder();

    public:

        GUI();
        ~GUI();

        //Main menu
        void menu();
};

