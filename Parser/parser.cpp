#include <Parser/parser.h>

/**
 *
 * getFileList(std::string, std::vector<std::string>)
 *
 * Gets the file list using the std::experimental::filesystem library,
 * finding all PDFs in a directory *recursively*.
 *
 * With some help from: https://goo.gl/AZV6UJ
 *
 * @param directory - the directory to search PDFs in
 * @param files     - the list of files to add PDF extensions to
 * @return          - the size of the files vector
 */
int Parser::getFileList(std::string directory, std::vector<std::string>& files)
{
    namespace fs = std::experimental::filesystem;
    for (fs::recursive_directory_iterator i(directory), end; i != end; ++i)
    {
        fs::path p = i->path();
        if (!fs::is_directory(p) && p.extension() == ".pdf"
            && p.filename().string().front() != '.')
        {
            files.push_back(p);
        }
    }
    return files.size();
}


/**
 *
 * parseFile(std::string&, IndexInterface*&)
 *
 * Parses a single PDF file and inserts the words in the document
 * into the IndexInterface provided through the parameter.
 *
 * @param file  - the string containing the full path of the file
 * @param index - a pointer reference to the IndexInterface with the PDF index
 */
void Parser::parseFile(std::string& file, IndexInterface*& index)
{
    //Get the document
    poppler::document* doc{poppler::document::load_from_file(file)};

    std::string text;

    //Places all text in the document into a string
    for(int i{}; i < doc->pages(); i++)
    {
        std::string pageStr = ("Pages: " + std::to_string(Parser::numPages));
        mvprintw(3, 25, pageStr.c_str());
        refresh();
        numPages++;
        poppler::page* page{doc->create_page(i)};
        text += page->text(rectangle).to_latin1();
        delete page;
    }

    //Removes all unwanted characters and makes all letters lowercase
    for(std::string::iterator it = text.begin(); it != text.end(); ++it)
    {
        char& c = *it;
        isalpha(c) ? c = tolower(c) : c = ' ';
    }

    std::stringstream buffer(text);

    std::string str;

    //Moves through each word in the string and adds to the file if the
    // word is longer than 3 and is not a stop word.
    while(buffer >> str)
    {
        if(!invalidLength(str) && !isStopWord(str))
        {
            std::string wordsStr = ("Words: "
                                    + std::to_string(Parser::numWords));
            mvprintw(3, 45, wordsStr.c_str());
            refresh();
            numWords++;
            totalWordsInDoc[file]++;
            Porter2Stemmer::stem(str);
            index->insert(str, file);
        }

    }
    delete doc;
}

int Parser::getNumWords()
{
    return numWords;
}

void Parser::setNumWords(int value)
{
    numWords = value;
}

int Parser::getNumPages()
{
    return numPages;
}

void Parser::setNumPages(int value)
{
    numPages = value;
}

int Parser::getNumFiles()
{
    return numFiles;
}

void Parser::setNumFiles(int value)
{
    numFiles = value;
}


/**
 *
 * isStopWord(std::string)
 *
 * Checks against the stopwords set if the given string matches one of the words
 *
 * @param word  - the word to check if it is a stop word
 * @return      - true if there is a match, false if not
 */
bool Parser::isStopWord(std::string& word)
{
    return stopwords.count(word);
}

/**
 *
 * invalidLength(std::string&)
 *
 * @param word  - the word to check
 * @return      - true if the word is less than 3 letters long, false if not
 */
bool Parser::invalidLength(std::string& word)
{
    return word.size() < 3;
}

/**
 *
 * parse(std::string, IndexInterface, int, int)
 *
 * Parses a directory and adds all words into the index interface,
 * providing an ncurses progress screen using the two int parameters
 * to delineate the size of the screen.
 *
 * @param directory - the directory to parse PDFs from
 * @param index     - the index to add words to
 * @param row       - the vertical size of the screen
 * @param col       - the horizontal size of the screen
 */
void Parser::parse(std::string& directory, IndexInterface*& index,
                   int row, int col)
{
    index->setIndexDirectory(directory);
    std::vector<std::string> files;
    numFiles = getFileList(directory, files);
    directoryParser(files, index, row, col);
}


/**
 *
 * directoryParser(vector<string>, IndexInterface, int, int)
 *
 * Parses a directory and adds all words that are PDFs into the index,
 * using the row and column variables to provide a progress screen
 * that tells the user the number of words and files being parsed.
 *
 * @param files     - the list of files to parse
 * @param index     - the index to add words to
 * @param row       - the vertical size of the screen
 * @param col       - the horizontal size of the screen
 */
void Parser::directoryParser(std::vector<std::string> files,
                             IndexInterface*& index, int row, int col)
{
    //Suppress all errors from poppler
    std::cerr.setstate(std::ios_base::failbit);

    double averageTime;

    for(int i{}; i < (int) files.size(); i++)
    {

        //Output current file
        std::string currentFile = ("File " + std::to_string(i+1) + "/" + std::to_string((int) files.size()));
        mvprintw(3, 5, currentFile.c_str());

        //Parse the current file
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        parseFile(files[i], index);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double, std::milli> milliseconds = t2 - t1;

        //Output the average parsing time
        averageTime = ((averageTime * i) + milliseconds.count()) / (i+1);
        currentFile = ("Average Parse Time (ms): " + std::to_string(averageTime));
        mvprintw(3, 65, currentFile.c_str());

        //If the filename is long, truncate it.
        if((int) files[i].size() > col - 25)
        {
            files[i].erase(col - 28, files[i].size());
            files[i] = ("..." + files[i]);
        }


        //Output for the stream of documents being parsed
        int j{};

        //Sets bottom of document stream
        if(i - (3 * row / 4) > 0)
            j = i - (3 * row / 4);

        for(;j < i+1; j++)
        {
            //Outputs the document
            for(int k{}; k < col - 2; k++)
            {
                mvaddch(5 + (i + 1 - j), 8 + k, ' ');
            }
            mvaddch(5 + (i + 1 - j), col - 2, '*');
            mvprintw(5 + (i + 1 - j), 8, files[j].c_str());
            refresh();


            //Clears each line so that the stream can be repopulated
            for(int k{col - 25}; k <col - 2; k++)
            {
                mvaddch(5 + (i + 1 - j), 8 + k, ' ');
            }
            mvaddch(5 + (i + 1 - j), col - 2, '*');
            refresh();

        }

        refresh();

    }
    //Allow for errors again
    std::cerr.clear();

    //Recalculate all ranks of the words
    index->recalculateRanking();
}

/**
 *
 * addExtraDoc(std::string, IndexInterface)
 *
 * Parses a file and adds its elements to the IndexInterface, calling
 * the parseFile() function.
 *
 * @param file      - the file to parse
 * @param index     - the index to add the file's contents to
 */
void Parser::addExtraDoc(std::__cxx11::string& file, IndexInterface*& index)
{
    numFiles++;
    parseFile(file, index);
    index->recalculateRanking();
}

/**
 *
 * addExtraDirectory(std::string, IndexInterface, int, int)
 *
 * Uses the directoryParser() function to add an extra directory to the
 * PDF IndexInterface given by the parameter.
 *
 * @param directory - the directory to parse
 * @param index     - the index to add elements to
 * @param row       - the vertical size of the screen
 * @param col       - the horizontal size of the screen
 */
void Parser::addExtraDirectory(std::__cxx11::string& directory,
                               IndexInterface*& index, int row, int col)
{
    std::vector<std::string> files;
    numFiles += getFileList(directory, files);
    directoryParser(files, index, row, col);
    index->recalculateRanking();
}


std::unordered_map<std::string, int> Parser::totalWordsInDoc{};
int Parser::numFiles{0};
bool Parser::extraFiles{false};
int Parser::numWords{0};
int Parser::numPages{0};

poppler::rectf Parser::rectangle{poppler::rectf(0, 0, 500, 500)};

std::unordered_set<std::string> Parser::stopwords
{"new","its", "why", "www", "let", "hed",
    "nor", "far", "few", "fix", "for","get", "got", "you", "had", "has",
    "itd", "her", "hes", "hid", "him","his", "how", "yet", "yes", "inc",
    "six", "que", "viz", "ran", "ref","run", "via", "saw", "say", "sec",
    "see", "she", "vol", "sub", "sup", "the", "til", "tip", "too",
    "try", "two", "ups", "use", "now","ltd", "who", "may", "mrs", "mug",
    "wed", "nay", "non", "way", "nos","not", "was", "off", "old",
    "one", "ord", "our", "out", "own","per", "put", "adj", "but", "edu",
    "etc", "ask", "due", "act", "can","are", "all", "any", "and", "end",
    "did", "com", "line", "come", "show","sure", "such", "mean", "some",
    "stop","made", "make", "soon", "many", "last","thou", "this",
    "ive", "just", "keep","date", "they", "know", "then", "them",
    "look", "that", "less", "lest", "than","lets", "like", "tell",
    "more", "take","part", "next", "nine", "away", "both","back",
    "biol", "none", "plus", "past","auth", "page", "over", "ours",
    "been","onto", "only", "ones", "once","okay","must", "shed", "miss",
    "kept", "sent","most", "self", "seen", "much", "seem","shes",
    "says", "came", "name", "aren", "will",
    "same", "near", "said", "need", "refs",
    "were", "whod", "even", "goes", "gone",
    "whim", "when", "have", "what", "whom",
    "went", "here", "want", "hers", "vols",
    "else", "very", "home", "from", "zero",
    "able", "five", "your", "youd", "abst",
    "wont", "four", "thru", "with", "wish",
    "gave", "ever", "gets", "whos", "give",
    "done", "down", "ill", "upon", "unto",
    "into", "took", "each", "does", "thus",
    "uses", "also", "used", "truly", "under",
    "tries", "theyd", "aside", "there", "right",
    "whose", "quite", "among", "arise", "these",
    "tried", "words", "proud", "world", "would",
    "pages", "above", "owing", "about", "think",
    "ought", "other", "youre", "yours", "begin",
    "those", "alone", "along", "added", "wasnt",
    "shown", "shows", "since", "wants", "tends",
    "shall", "taken", "after", "again", "sorry",
    "still", "value", "using", "arent", "well",
    "seven", "weve", "seems", "whats", "thank",
    "where", "which", "while", "thanx", "until",
    "thats", "their", "whole", "twice", "et-al",
    "heres", "every", "given", "known", "gives",
    "looks", "keeps", "comes", "noone", "hence",
    "itll", "could", "might", "makes", "isnt",
    "doing", "maybe", "index", "means", "never",
    "brief", "dont", "first", "knows", "being",
    "later", "often", "cant", "needs", "forth",
    "below", "found", "eight", "least", "fifth",
    "liked", "cause", "noted", "showed", "unless",
    "saying", "merely", "cannot", "unlike", "should",
    "effect", "shell", "hither", "seeing", "during",
    "almost", "useful", "seemed", "selves", "causes",
    "mostly", "anyone", "thence", "theirs", "lately",
    "thered", "begins", "latter", "thanks", "anyhow",
    "likely", "taking", "little", "theres", "showns",
    "theyre", "didnt", "itself", "though", "mainly",
    "throug", "inward", "toward", "anyway", "trying",
    "indeed", "always", "across", "except", "rather",
    "asking", "hasnt", "ninety", "within", "enough",
    "nobody", "whence", "hardly", "widely", "behind",
    "myself", "wheres", "become", "wholl", "became",
    "beyond", "gotten", "poorly", "please", "placed",
    "giving", "really", "namely", "youve", "youll",
    "nearly", "eighty", "either", "former", "before",
    "herein", "hereby", "obtain", "others", "ending",
    "having", "around", "werent", "recent", "beside",
    "wherein", "further", "whoever", "without", "follows",
    "couldnt", "wouldnt", "thereof", "getting", "thereto",
    "thereby", "therere", "therein", "willing", "whatll",
    "thatll", "himself", "usually", "various", "against",
    "howbeit", "herself", "affects", "however", "welcome",
    "hundred", "already", "havent", "theyll", "amongst",
    "instead", "towards", "whereas", "whereby", "another",
    "doesnt", "through", "whether", "happens", "thoughh",
    "whither", "theyve", "believe", "specify", "awfully",
    "quickly", "outside", "readily", "neither", "regards",
    "someone", "related", "somehow", "largely", "anyways",
    "similar", "nowhere", "results", "briefly", "section",
    "seeming", "certain", "several", "million", "omitted",
    "looking", "besides", "present", "suggest", "perhaps",
    "anymore", "thatve", "becomes", "between", "anybody",
    "because", "overall", "contain", "nothing", "possibly",
    "anywhere", "resulted", "formerly", "possible", "obtained",
    "usefully", "moreover", "normally", "whomever", "research",
    "hereupon", "affected", "wherever", "actually", "becoming",
    "recently", "probably", "promptly", "whatever", "provides",
    "whenever", "anything", "slightly", "somebody", "thousand",
    "followed", "somewhat", "together", "everyone", "meantime",
    "strongly", "although", "yourself", "announce", "somethan",
    "thereve", "contains", "unlikely", "therell", "latterly",
    "sometime", "different", "sometimes", "available", "somewhere",
    "beginning", "specified", "certainly", "primarily", "whereupon",
    "thereupon", "therefore", "necessary", "shouldnt", "immediate",
    "according", "meanwhile", "downwards", "everybody", "obviously",
    "something", "important", "resulting", "otherwise", "following",
    "similarly", "affecting", "ourselves", "invention", "hereafter",
    "elsewhere", "regarding", "everywhere", "themselves", "accordance",
    "everything", "beginnings", "yourselves", "beforehand",
    "apparently","thereafter", "containing", "specifying", "throughout",
    "importance","usefulness", "particular", "afterwards", "relatively",
    "regardless","whereafter", "previously", "especially","potentially",
    "furthermore","necessarily", "nonetheless", "accordingly",
    "immediately","significant", "information", "particularly",
    "sufficiently","successfully", "specifically", "respectively",
    "nevertheless","predominantly", "significantly", "substantially",
    "unfortunately","approximately"};
