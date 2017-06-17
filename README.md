# PDF Search Engine in C++

This program is a tf-idf search engine made in C++ for the CSE 2341 Final Project at Southern Methodist University. The UI was made with ncurses, and PDFs are parsed using the Poppler C++ parsing library. The index can be created with both a hash table and an AVL tree, but both indices have given relatively similar parsing and search times.

# Usage

Once the program is run, usage is relatively self-explanatory. Go to the "Add Files" section to add PDFs, whose contents will be analyzed by removing stop words, stemming the words, and adding them to the specified data structure. The program also lists the most frequent words within the corpus of PDFs.

# Searching

The user can search the index with the "Search Index" menu option, where a user can enter a simple prefix boolean query to search the PDF corpus. The system will return the 10 most relevant results (based on the term frequency - inverse document frequency relevancy algorithm), and the user will be able to open the PDFs in their default PDF program.
