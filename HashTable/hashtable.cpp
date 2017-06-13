#include <HashTable/hashtable.h>

/**
 * insert(string)
 *
 * Inserts the string key into the hash table, returning a reference
 * to the word within the table
 *
 * @param key   - the string to be used for inserting into the table
 * @return      - a reference to the Word object
 */
Word& HashTable::insert(std::__cxx11::string& key)
{
    return table[hash(key)].insert(Word(key));
}

/**
 * insert(string, string)
 *
 * Inserts the string into the hash table and insert the document into
 * the reference returned by the insert function
 *
 * @param key       - the string to be used for inserting into the table
 * @param document  - the document string to add to the Word reference
 * @return          - a reference to the word created with the key
 */
Word& HashTable::insert(std::__cxx11::string& key, std::__cxx11::string& document)
{
    Word& word = table[hash(key)].insert(Word(key));
    word.addDoc(document);
    return word;
}

/**
 * insert(Word)
 *
 * Inserts the word object into the tree, returning a reference to the Word
 * object after it is inserted into the tree
 *
 * @param word
 * @return
 */
Word& HashTable::insert(Word& word)
{
    return table[hash(word.getWord())].insert(word);
}

/**
 * hash(const string) const
 *
 * Hashes a const string reference and returns an integer based on the string
 *
 * With help from: http://www.cse.yorku.ca/~oz/hash.html
 *
 * @param key   - the string key to find the place within the hash table
 * @return      - an integer containing the index where the key should go
 */
int HashTable::hash(const std::__cxx11::string& key) const
{
    unsigned long hash = 5381;
    for(int i = 0; i < (int) key.length(); i++)
    {
        hash = ((hash << 5) + hash) + key[i];
    }

    return hash % LENGTH;
}

/**
 *
 * hash(string)
 *
 * Hashes a string reference and returns an integer based on the string
 *
 * With help from: http://www.cse.yorku.ca/~oz/hash.html
 *
 * @param key   - the string key to find the place within the hash table
 * @return      - an integer containing the index where the key should go
 */
int HashTable::hash(std::__cxx11::string& key)
{
    unsigned long hash = 5381;
    for(int i = 0; i < (int) key.length(); i++)
    {
        hash = ((hash << 5) + hash) + key[i];
    }

    return hash % LENGTH;
}

/**
 * hash(rvalue string)
 *
 * Hashes a rvalue string and returns an integer based on the string
 *
 * With help from: http://www.cse.yorku.ca/~oz/hash.html
 *
 * @param key   - an rvalue string key to find the place within the hash table
 * @return      - an integer containing the index where the key should go
 */
int HashTable::hash(std::__cxx11::string&& key)
{
    unsigned long hash = 5381;
    for(int i = 0; i < (int) key.length(); i++)
    {
        hash = ((hash << 5) + hash) + key[i];
    }

    return hash % LENGTH;
}

/**
 * clear()
 * Clears the index
 */
void HashTable::clear()
{
    for(int i{}; i < LENGTH; i++)
        table[i].clear();
}

/**
 * Destructor
 */
HashTable::~HashTable() {}

/**
 * operator[] overload
 *
 * @param index - the index to find the element within the table
 * @return      - the AVL tree reference at the index of the hash table
 * @exception   - out_of_range if the index is not within bounds
 */
AvlTree<Word>& HashTable::operator[](int index)
{
    if(index < 0 || index >= LENGTH)
        throw std::out_of_range("The index is outside the bounds of the table.");

    return table[index];
}

/**
 * operator<< overload
 *
 * @param o     - an ostream reference
 * @param tab   - the hash table to get data from
 * @return      - an ostream reference containing the data from tab
 */
std::ostream& operator<<(std::ostream& o, const HashTable& tab)
{
    for(int i{}; i < LENGTH; i++)
    {
        o << tab.table[i];
    }
    return o;
}

/**
 * getWord(string)
 *
 * Returns the a Word reference from the specified key
 *
 * @param key   - the string to search the tree with
 * @return      - a reference to the Word object within the tree
 * @exception   - out_of_range if the reference does not exist
 */
Word& HashTable::getWord(const std::__cxx11::string& key)
{
    return table[hash(key)].find(key);
}

/**
 * getSize()
 * @return the length of the hash table
 */
int HashTable::getSize()
{
    return LENGTH;
}
