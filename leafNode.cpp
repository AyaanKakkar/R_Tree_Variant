#include "leafNode.h"
#include "rtree.h"
#include <iostream>

using namespace std;

LeafNode::LeafNode(int32_t maxEntries, int32_t minEntries) : TreeNode(maxEntries, minEntries) {
    leafTypes_ = new Poi_Type[maxEntries + 1];
}

// Insert an entry into the subtree
// Returns -1 if entry successful, otherwise returns the ID of the new node that must be inserted into the parent
int32_t LeafNode::insert(Rectangle MBR, int32_t pointer, Poi_Type poiType, RTree* rTree) {


    vector<Poi_Type> pois;
    pois.push_back(poiType);
    addPOIs(pois);

    leafTypes_[currEntries_] = poiType;

    addLeafEntry(MBR, pointer);

    // Perform splitting if necessary
    if (currEntries_ > maxEntries_) {
        return split(rTree);
    }

    // Save changes to the disk
    rTree -> saveNode(this);

    return -1;
}

// Adds an entry to the leaf
void LeafNode::addLeafEntry(Rectangle MBR, int32_t pointer) {
    // Call add Entry
    addEntry(MBR, pointer);
}

void LeafNode::addEntryToSplitNode(int32_t entryIdx, TreeNode* splitNode, RTree* rTree) {

    vector<Poi_Type> temp;
    temp.push_back(leafTypes_[entryIdx]);
    ((LeafNode *)splitNode) -> addPOIs(temp);

    ((LeafNode *)splitNode) -> leafTypes_[((LeafNode *)splitNode) -> currEntries_] = leafTypes_[entryIdx];

    ((LeafNode *)splitNode) ->addEntry(MBR_[entryIdx], childPointers_[entryIdx]);
}

void LeafNode::copyNodeContent(TreeNode* node) {
    LeafNode* temp = (LeafNode *) node;
    currEntries_ = temp -> currEntries_;

    // Copy MBR
    currentMBR_ = temp -> currentMBR_;

    for (int32_t idx = 0; idx < currEntries_; idx++) {
        MBR_[idx] = temp -> MBR_[idx];
        childPointers_[idx] = temp -> childPointers_[idx];
        leafTypes_[idx] = temp -> leafTypes_[idx];
    }

    for (int i = 0; i < BITMAP_SIZE; i++) {
        nodeBitmap_[i] = temp -> nodeBitmap_[i];
        pois_[i] = temp -> pois_[i];
    }
    
    docPointer_ = temp -> docPointer_;
}


// Create a new split node
TreeNode* LeafNode::createSplitNode() {
    return new LeafNode(maxEntries_, minEntries_);
}


// Query
void LeafNode::query(Rectangle MBR, int32_t* bitmap, char* &doc, vector<Key_Value>* &pois, RTree* rTree) {

    Rectangle b = Rectangle::intersection(currentMBR_, MBR);

    if (b.isValid() && rTree -> bitmapsIntersect(bitmap, nodeBitmap_)) {
        rTree -> getDoc(docPointer_);
        for (int i = 0; i < BITMAP_SIZE; i++) {
            if (bitmap[i] > 0) {
                for (auto x: pois_[i]) {
                    bool flag = false;
                    for (auto &y : pois[i]) {
                        if (y.key == x.key) {
                            y.value += x.value;
                            flag = true;
                            break;
                        }
                    }
                    if (!flag) {
                        pois[i].push_back(x);
                    }
                }
            }
        }
    }
}

LeafNode::~LeafNode() {
    delete [] leafTypes_;
}