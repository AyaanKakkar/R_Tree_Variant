#include <typeinfo>
#include <iostream>
#include <cstring>
#include <deque>
#include <vector>
#include "rtree.h"
#include <config.hpp>

#define MIN(a, b) (a) < (b) ? a : b

using namespace std;

// Create a RTree with the given paramters
RTree* RTree::CreateIndex(string fileName, int32_t pageSize, int32_t maxEntries, int32_t minEntries) {
    // Create a new RTree
    RTree* rTree = new RTree();
    
    // Set parameters
    rTree -> rootPageOffset_ = 1;
    rTree -> pageSize_ = pageSize;
    rTree -> maxEntries_ = maxEntries;
    rTree -> minEntries_ = minEntries;
    rTree -> treeSize_ = 0;
    rTree -> rootID_ = -1;
    rTree -> rootNode_ = nullptr;
    rTree -> nodesAccessed_ = 0;
    rTree -> diskIO_ = 0;
    rTree -> nextPage_ = 1;

    // Open the file
    rTree -> filePointer_.open(fileName, ios::binary | ios::in | ios::out | ios::trunc);

    // Save the tree
    rTree -> saveTree();

    return rTree;
}

// Insert an entry into the RTree
void RTree::insert(Rectangle MBR, int32_t pointer, Poi_Type poiType) {
    if (treeSize_ == 0) {
        // If it's an empty tree then create a root node
        rootNode_ = new LeafNode(maxEntries_, minEntries_);
    }
    

    // Insert into the root
    int32_t newNodeID = this -> rootNode_ -> insert(MBR, pointer, poiType, this);


    if (newNodeID != -1) {
        // Get the new node
        TreeNode* newNode = this -> getNode(newNodeID);

        // Create a new root
        TreeNode* newRoot = new TreeNode(maxEntries_, minEntries_);

        // Add the new node
        newRoot -> addEntry(newNode -> currentMBR_, newNodeID); 

        newRoot -> addPOIs(newNode -> pois_);

        // Add the current root
        newRoot -> addEntry(rootNode_ -> currentMBR_, rootNode_ -> nodeID_);

        newRoot -> addPOIs(rootNode_ -> pois_);

        // Free the memory
        delete rootNode_;
        delete newNode;

        // Change the root
        rootNode_ = newRoot;

        // Save the root
        saveNode(rootNode_);
    }

    // Update root ID if necessary
    if (rootID_ != rootNode_ -> nodeID_) {
        rootID_ = rootNode_ -> nodeID_;
        saveTree();
    }
}

// Save a node
void RTree::saveNode(TreeNode* node) {

    if (node -> nodeID_ == -1) {
        treeSize_++;
        this -> saveTree();
    }

    writeNode(node);
}


// Query
void RTree::queryMBR(Rectangle MBR, int32_t* bitmap, char* &doc, vector<Key_Value>* &pois) {
    doc = new char[pageSize_];
    pois = new vector<Key_Value>[BITMAP_SIZE];
    TreeNode* topNode = getNode(rootID_);
    topNode -> query(MBR, bitmap, doc, pois, this);
}


bool RTree::bitmapsIntersect(int32_t* b1, int32_t* b2) {
    for (int i = 0; i < BITMAP_SIZE; i++) {
        if (b1[i] > 0 && b2[i] > 0) {
            return true;
        }
    }
    return false;
}

// Retrieve a node using ID
TreeNode* RTree::getNode(int32_t id) {
    TreeNode* node = this -> readNode(id);
    node -> nodeID_ = id;
    nodesAccessed_++;
    return node;
}

// Returns node data and size
int32_t RTree::getNodeData(TreeNode* node, char* &a) {
    // Calculate the size of the node
    int32_t size = 0;

    // Check if it's a leaf node
    bool flag = (typeid(*node) == typeid(LeafNode));


    size += sizeof(bool); // Node flag
    size += sizeof(node -> currEntries_);
    size += sizeof(node -> currentMBR_);
    size += sizeof(node -> currentMBR_) * node -> currEntries_;
    size += sizeof(int32_t) * node -> currEntries_;
    
    size += sizeof(node -> nodeBitmap_[0]) * BITMAP_SIZE;

    size += sizeof(node -> docPointer_);

    for (int i = 0; i < BITMAP_SIZE; i++) {
        int32_t listSize = node -> pois_[i].size();
        size += sizeof(listSize);
        size += sizeof(Key_Value) * listSize;
    }

    if (flag) {
        size += sizeof(Poi_Type) * node -> currEntries_;
    }

    a = new char[size];
    
    char* curr;
    curr = a;

    

    // Save the type of the node
    memcpy(curr, (char *) &flag, sizeof(flag));
    curr += sizeof(flag);

    // Save the number of entries
    memcpy(curr, (char *) &(node -> currEntries_), sizeof(node -> currEntries_));
    curr += sizeof(node -> currEntries_);

    // Save the MBRs
    memcpy(curr, (char *) &(node -> currentMBR_), sizeof(node -> currentMBR_));
    curr += sizeof(node -> currentMBR_);

    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy(curr, (char *) &(node -> MBR_[idx]), sizeof(node -> MBR_[idx]));
        curr += sizeof(node -> MBR_[idx]);
    }


    // Save the child Pointers
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy(curr, (char *) &(node -> childPointers_[idx]), sizeof(node -> childPointers_[idx]));
        curr += sizeof(node -> childPointers_[idx]);
    }
    
    // Save the BM
    for (int32_t idx = 0; idx < BITMAP_SIZE; idx++) {
        memcpy(curr, (char *) &(node -> nodeBitmap_[idx]), sizeof(node -> nodeBitmap_[idx]));
        curr += sizeof(node -> nodeBitmap_[idx]);
    }

    // Save the doc
    memcpy(curr, (char *) &(node -> docPointer_), sizeof(node -> docPointer_));
    curr += sizeof(node -> docPointer_);

    // Save the lists
    for (int32_t idx = 0; idx < BITMAP_SIZE; idx++) {
        int32_t listSize = node -> pois_[idx].size();
        memcpy(curr, (char *) &listSize, sizeof(listSize));
        curr += sizeof(listSize);
        for (int32_t i = 0; i < listSize; i++) {
            memcpy(curr, (char *) &(node -> pois_[idx][i]), sizeof(node -> pois_[idx][i]));
            curr += sizeof(node -> pois_[idx][i]);   
        }
    }

    if (flag) {
        LeafNode* temp = (LeafNode*) node;
        for (int32_t idx = 0; idx < temp -> currEntries_; idx++) {
            memcpy(curr, (char *) &(temp -> leafTypes_[idx]), sizeof(temp -> leafTypes_[idx]));
            curr += sizeof(temp -> leafTypes_[idx]);
        }
    }

    return size;
}


// Write a node to the file
// The initial bytes will be reserved to save the next page for the node
// Initial metadata would also contain the number of useful bytes to read in this page 
void RTree::writeNode(TreeNode* node) {

    if (node -> docPointer_ == -1) {
        node -> docPointer_ = nextPage_++;
        diskIO_++;
    }

    int32_t pageAvailable = pageSize_ - 2 * sizeof(int32_t);

    char *a; // Node data to be written
    
    int32_t size = getNodeData(node, a);

    deque <int32_t> pages;

    if (node -> nodeID_ == -1) {
        node -> nodeID_ = nextPage_;
        nextPage_++;
        int32_t bytesLeft = size - pageAvailable;
        while (bytesLeft > 0) {
            bytesLeft -= pageAvailable;
            pages.push_back(nextPage_);
            nextPage_++;
        }
    }
    else {
        filePointer_.clear();
        filePointer_.seekg(pageSize_ * node -> nodeID_);
        int32_t nextPage;
        filePointer_.read((char*) &nextPage, sizeof(nextPage));

        int32_t bytesLeft = size - pageAvailable;
        while (nextPage != -1 && bytesLeft > 0) {
            pages.push_back(nextPage);
            filePointer_.clear();
            filePointer_.seekg(nextPage * pageSize_);
            filePointer_.read((char*) &nextPage, sizeof(nextPage));
            bytesLeft -= pageAvailable;
        }

        while (bytesLeft > 0) {
            bytesLeft -= pageAvailable;
            pages.push_back(nextPage_);
            nextPage_++;
        }  
    }

    int32_t currPage = node -> nodeID_;

    int32_t bytesLeft = size;

    char* curr;
    curr = a;




    while (bytesLeft > 0) {
        filePointer_.clear();
        filePointer_.seekp(pageSize_ * currPage);
        int32_t nextPage = (pages.empty()) ? -1 : pages.front();
        if (!pages.empty()) pages.pop_front();
        int32_t bytesUsed = MIN(bytesLeft, pageAvailable);
        bytesLeft -= bytesUsed;

        filePointer_.write((char*) &nextPage, sizeof(nextPage));
        filePointer_.write((char*) &bytesUsed, sizeof(bytesUsed));
        filePointer_.write(curr, bytesUsed);

        diskIO_++;

        curr += bytesUsed;
        currPage = nextPage;
    }

    delete[] a;

    filePointer_.flush();
}


int32_t RTree::readNodeData(int32_t page, char* &a) {
    int32_t currPage = page;
    int32_t size = 0;

    char* pageData;

    deque <char*> pages;

    while (currPage != -1) {
        filePointer_.clear();
        filePointer_.seekg(currPage * pageSize_);
        pageData = new char[pageSize_];
        filePointer_.read(pageData, pageSize_);
        diskIO_++;
        int32_t bytesUsed;
        memcpy((char*) &currPage, pageData, sizeof(currPage));
        memcpy((char*) &bytesUsed, (pageData + sizeof(currPage)), sizeof(bytesUsed));
        size += bytesUsed;
        pages.push_back(pageData);
    }

    a = new char[size];

    char* curr;
    curr = a;

    currPage = page;
    
    while (currPage != -1) {
        filePointer_.clear();
        pageData = pages.front();
        int32_t bytesUsed;
        memcpy((char*) &currPage, pageData, sizeof(currPage));
        memcpy((char*) &bytesUsed, (pageData + sizeof(currPage)), sizeof(bytesUsed));
        memcpy(curr, (pageData + sizeof(currPage) + sizeof(bytesUsed)), bytesUsed);
        curr += bytesUsed;
        delete[] pageData;
        pages.pop_front();
    }

    return size;
}


// Read the node from the file
TreeNode* RTree::readNode(int32_t page) {

    char *a; // Node data
    int32_t size = readNodeData(page, a);

    char* curr;
    curr = a;

    // Get the type of the node
    bool leaf;
    memcpy((char *) &leaf, curr, sizeof(leaf));
    curr += sizeof(leaf);

    // Create the node
    TreeNode* node;
    if (leaf) {
        node = new LeafNode(maxEntries_, minEntries_);
    }
    else {
        node = new TreeNode(maxEntries_, minEntries_);
    }

    // Retrieve the relevant info
    memcpy((char *) &(node -> currEntries_), curr, sizeof(node -> currEntries_));
    curr += sizeof(node -> currEntries_);

    // Retrieve MBRs
    memcpy((char *) &(node -> currentMBR_), curr, sizeof(node -> currentMBR_));
    curr += sizeof(node -> currentMBR_);
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy((char *) &(node -> MBR_[idx]), curr, sizeof(node -> MBR_[idx]));
        curr += sizeof(node -> MBR_[idx]);
    }

    // Retrieve pointers
    for (int32_t idx = 0; idx < node -> currEntries_; idx++) {
        memcpy((char *) &(node -> childPointers_[idx]), curr, sizeof(node -> childPointers_[idx]));
        curr += sizeof(node -> childPointers_[idx]);
    }

    // Retrieve BMs
    for (int32_t idx = 0; idx < BITMAP_SIZE; idx++) {
        memcpy((char *) &(node -> nodeBitmap_[idx]), curr, sizeof(node -> nodeBitmap_[idx]));
        curr += sizeof(node -> nodeBitmap_[idx]);
    }

    // Retrieve doc
    memcpy((char *) &(node -> docPointer_), curr, sizeof(node -> docPointer_));
    curr += sizeof(node -> docPointer_);

    for (int i = 0; i < BITMAP_SIZE; i++) {
        int32_t listSize;
        memcpy((char *) &(listSize), curr, sizeof(listSize));
        curr += sizeof(listSize);
        for (int32_t idx = 0; idx < listSize; idx++) {
            Key_Value tempKV;
            memcpy((char *) &(tempKV), curr, sizeof(tempKV));
            curr += sizeof(tempKV);
            node -> pois_[i].push_back(tempKV);
        }
    }

    if (leaf) {
        // Retrieve leaf types
        LeafNode* temp = (LeafNode*) node;
        for (int32_t idx = 0; idx < temp -> currEntries_; idx++) {
            memcpy((char *) &(temp -> leafTypes_[idx]), curr, sizeof(temp -> leafTypes_[idx]));
            curr += sizeof(temp -> leafTypes_[idx]);
        }
    }

    delete[] a;

    return node;
}


void RTree::getDoc(int32_t docPointer) {
    diskIO_++;
}

// Save the tree to the file
void RTree::saveTree() {
    // Place the write pointer in the beginning
    filePointer_.clear();
    filePointer_.seekp(0);

    // Save the relevant info
    filePointer_.write((char *) &rootPageOffset_, sizeof(rootPageOffset_));
    
    filePointer_.write((char *) &treeSize_, sizeof(treeSize_));

    filePointer_.write((char *) &rootID_, sizeof(rootID_));

    filePointer_.write((char *) &pageSize_, sizeof(pageSize_));

    filePointer_.write((char *) &maxEntries_, sizeof(maxEntries_));

    filePointer_.write((char *) &minEntries_, sizeof(minEntries_));

    filePointer_.write((char *) &nextPage_, sizeof(nextPage_));    

    filePointer_.flush();

    diskIO_++;
    
}

// Load an existing RTree from the given file
RTree* RTree::LoadIndex(string fileName) {
    // Create a RTree
    RTree* rTree = new RTree();

    // Open the file
    rTree -> filePointer_.open(fileName, ios::binary | ios::in | ios::out);

    // Read the relevant info
    rTree -> filePointer_.read((char *) &(rTree -> rootPageOffset_), sizeof(rTree -> rootPageOffset_));
    
    rTree -> filePointer_.read((char *) &(rTree -> treeSize_), sizeof(rTree -> treeSize_));

    rTree -> filePointer_.read((char *) &(rTree -> rootID_), sizeof(rTree -> rootID_));

    rTree -> filePointer_.read((char *) &(rTree -> pageSize_), sizeof(rTree -> pageSize_));

    rTree -> filePointer_.read((char *) &(rTree -> maxEntries_), sizeof(rTree -> maxEntries_));

    rTree -> filePointer_.read((char *) &(rTree -> minEntries_), sizeof(rTree -> minEntries_));

    rTree -> filePointer_.read((char *) &(rTree -> nextPage_), sizeof(rTree -> nextPage_));   

    // Get the root node
    rTree -> rootNode_ = nullptr;
    if (rTree -> rootID_ != -1) {
        rTree -> rootNode_ = rTree -> getNode(rTree -> rootID_);
    }
    
    rTree -> nodesAccessed_ = 0;
    rTree -> diskIO_ = 0;

    return rTree;
}

// Destructor
RTree::~RTree() {
    delete rootNode_;
    filePointer_.close();
}