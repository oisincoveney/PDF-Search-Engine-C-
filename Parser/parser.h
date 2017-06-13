#pragma once


#include <unordered_set>
#include <unordered_map>
#include <string>
#include <experimental/filesystem>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-rectangle.h>
#include <stemmer.h>
#include <chrono>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <Index/indexinterface.h>
#include <ncurses.h>
#include <list>
#include <cstdlib>

class IndexInterface;

    using namespace std::chrono;

/**
 *
 * The Parser class is a static class that contains the functionality
 * for parsing a PDF and adding it to an IndexInterface index. The Parser class
 * also keeps track of the total number of files, words, and pages parsed,
 * as well as a table of documents and their respective word counts.
 *
 * NOTE: The word counts are the counts of words that have been parsed,
 *       which does not include words shorter than 3, or stop words.
 *
 * By: Oisin Coveney
 *
 */
class Parser
{
    private:

        //Stopword list
        static std::unordered_set<std::string> stopwords;

        //Gets maximum possible size for PDF to capture all data
        static poppler::rectf rectangle;

        //Steps 1 & 2 of parsing files and developing index
        static int getFileList(std::string directory, std::vector<std::string>& files);
        static void parseFile(std::string& file, IndexInterface*& index);

    public:

        //Counters for files, words, pages, and table for doc-wordount
        static int numFiles;
        static bool extraFiles;
        static int numWords;
        static int numPages;
        static std::unordered_map<std::string, int> totalWordsInDoc;

        //Checking validity of words
        static bool isStopWord(std::string& word);
        static bool invalidLength(std::string& word);

        //Parsing functions
        static void parse(std::string& directory,
                          IndexInterface*& index, int row, int col);
        static void addExtraDoc(std::string& file, IndexInterface*& index);
        static void addExtraDirectory(std::string& directory,
                                      IndexInterface*& index, int row, int col);
        static void directoryParser(std::vector<std::__cxx11::string> files,
                                    IndexInterface*&index, int row, int col);

        //Getters and setters
        static int getNumFiles();
        static void setNumFiles(int value);
        static int getNumPages();
        static void setNumPages(int value);
        static int getNumWords();
        static void setNumWords(int value);
};


