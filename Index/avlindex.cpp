#include <Index/avlindex.h>

/**
 * recalcRankingHelper(Node<Word>)
 *
 * A helper function for recalculating the ranking of each element within the
 * tree. The function is recursively called until it hits the nullptr children
 * of each node.
 *
 * @param node  - the head of the tree to calculate frankings from
 */
void AVLIndex::recalcRankingHelper(Node<Word>*& node)
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
Word& AVLIndex::insert(std::string& word)
{
    Word& ref = tree.insert(Word(word));
    addToFrequencyList(ref);
    isEmpty = false;
    return ref;
}

/**
 *
 * insert(std::string, std::string)
 *
 * Adds the document string to the Word object containing the word string
 * given by the parameter. If the word does not exist within the tree,
 * it is added to the tree.
 *
 * @param word      - the word to add to the tree and add a document to
 * @param document  - the document to add to the specified Word object
 */
void AVLIndex::insert(std::string& word, std::string& document)
{
    Word& ref = tree.insert(Word(word));
    addToFrequencyList(ref);
    isEmpty = false;
    ref.addDoc(document);
}

/**
 *
 * insert(Word)
 *
 * Inserts the word to the index and checks whether the word is one of the most
 * frequent words in the index.
 *
 * @param word  - the word to insert into the tree
 */
void AVLIndex::insert(Word word)
{
    Word& ref = tree.insert(word);
    isEmpty = false;
    addToFrequencyList(ref);
}

/**
 * empty()
 * @return  - true if the tree is empty, false if not
 */
bool AVLIndex::empty()
{
    return isEmpty;
}

/**
 *
 * Returns a reference to the Word element within the tree.
 *
 * @param word  - the Word to find within the tree
 * @exception   - out_of_bounds if the word isn't found
 * @return      - a reference to the Word object
 */
Word& AVLIndex::get(const std::string& word)
{
    return tree.find(word);
}


/**
 * getDirectoryFromFile()
 *
 * @return  - a string containing the directory from the index file if it exists
 */
std::string AVLIndex::getDirectoryFromFile()
{
    std::ifstream is{};
    is.open(getIndexFile());
    std::string directory;
    is >> directory;
    is.close();
    return directory;
}

/**
 * getDataType()
 *
 * @return  - a string containing "AVL tree"
 */
std::__cxx11::string AVLIndex::getDataType()
{
    return "AVL tree";
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
bool AVLIndex::load()
{
    isEmpty = false;
    std::ifstream is{};
    is.open(getIndexFile());

    std::string type;
    is >> indexDirectory >> type >> Parser::numFiles
            >> Parser::numPages >> Parser::numWords;

    //If the index file is in AVL tree form, parsing is much faster
    if(type == "AVL")
    {
        std::getline(is, type);
        std::getline(is, type);
        tree.head = loadTree(tree.head, is);
    }
    //Otherwise, just brute force and add all documents
    else if(type == "HASH")
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

    is.close();

    //Get table of document-word count
    std::ifstream ifs;
    ifs.open(getWordCount());

    int numWords;
    ifs >> numWords;

    int total;
    std::string doc;
    for(int i{}; i < numWords; i++)
    {
        ifs >> doc >> total;
        Parser::totalWordsInDoc[doc] = total;
    }
    ifs.close();

    //Get most frequent words
    loadFrequencyList(tree.head);
    return true;
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
Node<Word>* AVLIndex::loadTree(Node<Word>*& node, std::istream& is)
{
    std::string buf;
    //Checks if the end of the tree has been found
    while(is >> buf)
    {
        //Null pointer delimiter
        if(buf == "$$$")
            return nullptr;
        else
        {
            node = new Node<Word>(Word(buf, is));
            node->left = loadTree(node->left, is);
            node->right = loadTree(node->right, is);
            node->height = std::max(tree.getHeight(node->left),
                                    tree.getHeight(node->right)) + 1;
            return node;
        }
    }
    return node;
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
void AVLIndex::addToFrequencyList(Word& word)
{
    if(std::find(frequentWords.begin(), frequentWords.end(), &word) == frequentWords.end())
        frequentWords.push_back(&word);

    std::sort(frequentWords.begin(), frequentWords.end(), [](Word*& lhs, Word*& rhs)
    {
        return lhs->getTotalFrequency() > rhs->getTotalFrequency();
    });
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
void AVLIndex::loadFrequencyList(Node<Word>*& head)
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
 * printFrequencyList()
 *
 * Uses std::cout to output the list of 50 most frequent words
 */
void AVLIndex::printFrequencyList()
{
    for(Word* w : frequentWords)
        std::cout << w->getWord() << "\t" << w->getTotalFrequency() << " times" << std::endl;
}

/**
 * recalculateRanking()
 *
 * Recalculates the ranking of all elements within AVL tree using
 * the recalcRankingHelper function
 */
void AVLIndex::recalculateRanking()
{
    recalcRankingHelper(tree.head);
}

/**
 *
 * clear()
 *
 * Clears the index
 */
void AVLIndex::clear()
{
    isEmpty = true;
    tree.clear();
}

/**
 *
 * printToFile()
 *
 * Prints the PDF index to the index file, and prints the table of
 * document-total word combinations to the wordCount file.
 *
 */
void AVLIndex::printToFile()
{
    //output the AVL tree to index file
    std::ofstream o;
    o.open(getIndexFile());
    o << indexDirectory << "\t";
    o << "AVL" << "\t" << Parser::getNumFiles()  << "\t";
    o << Parser::getNumPages() << "\t" << Parser::getNumWords() << std::endl;
    o << tree;
    o.close();

    //output the document-word count table
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
AVLIndex::~AVLIndex() {}

/**
 * operator<< overload
 * @param o         - the ostream reference
 * @param index     - the index to take data from
 * @return          - the ostream reference with the new data
 */
std::ostream& operator<<(std::ostream& o, const AVLIndex& index)
{
    o << index.tree;
    return o;
}
