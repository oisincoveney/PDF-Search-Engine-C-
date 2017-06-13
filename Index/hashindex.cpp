#include <Index/hashindex.h>

HashTable HashIndex::getTable() const
{
    return table;
}

/**
 * empty()
 * @return  - returns the isEmpty variable delineating
 *              whether the table is empty or not
 */
bool HashIndex::empty()
{
    return isEmpty;
}

/**
 * recalcRankingHelper(Node<Word>)
 *
 * A helper function for recalculating the ranking of each element within the
 * trees of the hash table. The function is recursively called until it
 * hits the nullptr children of each node.
 *
 * @param node  - the head of the tree to calculate frankings from
 */
void HashIndex::recalcRankingHelper(Node<Word>*& node)
{
    if(node != nullptr)
    {
        node->data.calculateRanking();
        recalcRankingHelper(node->left);
        recalcRankingHelper(node->right);
    }
}

/**
 *
 * insert(std::string)
 *
 * Inserts the word to the index and checks whether the word is one of the most
 * frequent words in the index.
 *
 * @param word  - the word to add to the index
 * @return      - a reference to the Word object that was inserted
 */
Word& HashIndex::insert(std::__cxx11::string& word)
{
    isEmpty = false;

    Word& ref = table.insert(word);
    addToFrequencyList(ref);
    return ref;
}

/**
 *
 * insert(std::string, std::string)
 *
 * Adds the document string to the Word object containing the word string
 * given by the parameter. If the word does not exist within the table,
 * it is added to the table.
 *
 * @param word      - the word to add to the table and add a document to
 * @param document  - the document to add to the specified Word object
 */
void HashIndex::insert(std::__cxx11::string& word, std::__cxx11::string& document)
{
    isEmpty = false;

    Word& ref = table.insert(word);
    addToFrequencyList(ref);
    ref.addDoc(document);

}

/**
 *
 * insert(Word)
 *
 * Inserts the word to the index and checks whether the word is one of the most
 * frequent words in the index.
 *
 * @param word  - the word to insert into the table
 */
void HashIndex::insert(Word word)
{
    isEmpty = false;

    Word& ref = table.insert(word);
    addToFrequencyList(ref);
}

/**
 *
 * get(std::string)
 *
 * Returns a reference to the word within the tree. IF the word isn't found,
 * an out_of_bounds exception will be thrown.
 *
 * @param word      - the word to search for within the string
 * @exception       - out_of_bounds if the word does not exist
 * @return          - the reference to the Word that corresponds with the string
 */
Word& HashIndex::get(const std::__cxx11::string& word)
{
    Word& wd = table.getWord(word);
    return wd;
}

/**
 * getDirectoryFromFile()
 *
 * @return  - a string containing the directory from the index file if it exists
 */
std::string HashIndex::getDirectoryFromFile()
{
    std::ifstream is{};
    is.open(getIndexFile());
    std::string directory;
    is >> directory;
    is.close();
    return directory;
}

/**
 *
 * getDataType()
 *
 * @return - a string containing "hash table"
 */
std::__cxx11::string HashIndex::getDataType()
{
    return "hash table";
}

/**
 *
 * load()
 *
 * Loads the PDF index from the index file, and loads the table of documents
 * and the total number of words found.
 *
 * @return - false if the index file is invalid, true otherwise
 */
bool HashIndex::load()
{
    std::ifstream is{};
    is.open(getIndexFile());


    std::string type;
    is >> indexDirectory >> type >> Parser::numFiles >> Parser::numPages >> Parser::numWords;

    //If the index file has the same structure, parsing is faster
    if(type == "HASH")
    {
        int i{};
        while(is >> type && i < LENGTH)
        {
            table[i].head = loadTree(table[i].head, is);
            is >> type;
            i++;
        }
    }
    //Otherwise, brute force the parsing and just add all words linearly
    else if(type == "AVL")
    {
        while(is >> type)
        {
            if(!(type == "$$$" || type == "end---------------------------------"
                 || type == "start-------------------------------"))
            {
                insert(Word(type, is));
            }
        }
    }
    else
    {
        is.close();
        return false;
    }

    isEmpty = false;
    is.close();

    //Output the table of documents and total words
    std::ifstream ifs;
    ifs.open(getWordCount());

    int numWords;
    ifs >> numWords;

    std::string doc;
    int total;
    for(int i{}; i < numWords; i++)
    {
        ifs >> doc >> total;
        Parser::totalWordsInDoc[doc] = total;
    }
    ifs.close();

    loadFrequencyList();
    return true;
}

/**
 *
 * clear()
 *
 * Clears the index
 */
void HashIndex::clear()
{
    isEmpty = true;
    table.clear();
}

/**
 *
 * printToFile()
 *
 * Prints the PDF index to the index file, and prints the table of
 * document-total word combinations to the wordCount file.
 *
 */
void HashIndex::printToFile()
{
    //Opening the index file
    std::ofstream o;
    o.open(getIndexFile());
    o << indexDirectory << "\t";
    o << "HASH" << "\t" << Parser::getNumFiles() << "\t";
    o << Parser::getNumPages() << "\t" << Parser::getNumWords() << std::endl;
    o << table;
    o.close();

    //Opens the wordCount file
    o.open(getWordCount());
    o << Parser::totalWordsInDoc.size() << std::endl;
    for(auto it = Parser::totalWordsInDoc.begin(); it != Parser::totalWordsInDoc.end(); ++it)
    {
        o << it->first << "\t" << it->second << std::endl;
    }
    o.close();

}


/**
 * Destructor
 */
HashIndex::~HashIndex() {}

/**
 *
 * getHeight(Node<Word>*& node)
 *
 * Helper function to load each AVL tree of the hash table
 *
 * @param node  - the head of each tree in the hash table
 * @return      - the height of each node, -1 if nullptr
 */
int HashIndex::getHeight(Node<Word>*& node)
{
    return (node == nullptr) ? -1 : node->height;
}

/**
 *
 * addToFrequencyList(Word&)
 *
 * Adds the Word object to the list of frequent words if it happens to be
 * in the index frequently. Otherwise, nothing happens.
 *
 * @param word  - the Word object to check against and add to the list
 */
void HashIndex::addToFrequencyList(Word& word)
{
    //Check if the word doesn't exist, add it
    if(std::find(frequentWords.begin(), frequentWords.end(), &word) == frequentWords.end())
        frequentWords.push_back(&word);

    //Sort the list by the frequency of the word
    std::sort(frequentWords.begin(), frequentWords.end(), [](Word*& lhs, Word*& rhs)
    {
        return lhs->getTotalFrequency() > rhs->getTotalFrequency();
    });

    //If the size is greater than 50, get rid of the smallest values
    if(frequentWords.size() >= 50)
    {
        while(frequentWords.size() != 50)
        {
            frequentWords.pop_back();
        }
    }
}


/**
 *
 * loadFrequencyList(Node<Word>*)
 *
 * Recursively moves through each node within the given Node<Word> (presumably
 * an AVL tree) to find the most frequent words within the index.
 *
 * @param head  - the head of the tree within the hash table
 */
void HashIndex::loadFrequencyList(Node<Word>* head)
{
    if(head == nullptr)
        return;
    else
    {
        addToFrequencyList(head->data);
        loadFrequencyList(head->left);
        loadFrequencyList(head->right);
    }
    return;

}

/**
 * Finds all words that should be in the most frequent words using
 * the helper function loadFrequencyList(Node<Word>*)
 */
void HashIndex::loadFrequencyList()
{
    for(int i{}; i < LENGTH; i++)
    {
        loadFrequencyList(table[i].head);
    }
}

/**
 * printFrequencyList()
 *
 * Uses std::cout to output the list of 50 most frequent words
 */
void HashIndex::printFrequencyList()
{
    for(Word* w : frequentWords)
        std::cout << w->getWord() << "\t" << w->getTotalFrequency() << " times" << std::endl;
}

/**
 * recalculateRanking()
 *
 * Recalculates the ranking of all elements within the hash table using
 * the recalcRankingHelper function
 */
void HashIndex::recalculateRanking()
{
    for(int i{}; i < LENGTH; i++)
        recalcRankingHelper(table[i].head);
}

/**
 *
 * loadTree(Node<Word>* node, std::istream&)
 *
 * Helper function to load an AVL tree from an istream to the given node.
 * When the function is complete, the whole AVL tree for the istream should
 * be added.
 *
 * @param node  - the node to add to the tree
 * @param is    - the stream to take data from
 * @return      - the head node of the new tree
 */
Node<Word>* HashIndex::loadTree(Node<Word>* node, std::istream& is)
{
    std::string buf;
    //Checks if the end of the tree has been found
    while(is >> buf && buf != "end---------------------------------")
    {
        //Null pointer delimiter
        if(buf == "$$$")
            return nullptr;
        else
        {
            node = new Node<Word>(Word(buf, is));
            node->left = loadTree(node->left, is);
            node->right = loadTree(node->right, is);
            node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
            return node;
        }
    }
    return node;
}

/**
 *
 * operator<< overload
 *
 * @param o     - the ostream reference to add data to
 * @param index - the index to add data from
 * @return      - the ostream reference with the new data
 */
std::ostream& operator<<(std::ostream& o, const HashIndex& index)
{
    o << index.table;
    return o;
}
