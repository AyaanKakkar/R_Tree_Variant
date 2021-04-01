#ifndef _TREENODE
#define _TREENODE

#include <cstdint>
#include <list>
#include <vector>
#include "rectangle.h"
#include "keyValue.hpp"
#include "poiType.hpp"

using namespace std;

class RTree;

class TreeNode {

    /*
        Used to declare a node in the R-Tree
    */

    protected: 

        // Basic Node Data
        int32_t nodeID_; // ID of the node (Used to access it on disk)
        int32_t maxEntries_; // Max Number of entries in the node
        int32_t minEntries_; // Min number of entries in the node
        int32_t currEntries_; // Number of current entries
        
        // Child Pointers
        int32_t* childPointers_; // Child Pointers (Stores the ID of each child)
        
        // MBRs
        Rectangle currentMBR_; // MBR of the current node
        Rectangle* MBR_; // MBR of each child
        
        int32_t* nodeBitmap_;
        vector<Key_Value> *pois_;

        int32_t docPointer_;

        // Add Entry to the node
        void addEntry(Rectangle MBR, int32_t pointer);
        
        // Add POIs
        void addPOIs(vector<Poi_Type> pois);
        void addPOIs(vector<Key_Value>* pois);



        // Split node
        int32_t split(RTree *rTree);

        // Split helper functions
        TreeNode* chooseSplitNode(int32_t entryIdx, TreeNode* splitNode1, TreeNode* splitNode2, int32_t remainingEntries);
        virtual TreeNode* createSplitNode();
        virtual void addEntryToSplitNode(int32_t entryIdx, TreeNode* splitNode, RTree* rTree);
        
        // Copy node
        virtual void copyNodeContent(TreeNode* node);

        // Remove event data
        void freePOI();

    public:

        // Constructor and desctructor
        TreeNode(int32_t maxEntries, int32_t minEntries);
        virtual ~TreeNode();
        
        // Insertion function
        virtual int32_t insert(Rectangle MBR, int32_t pointer, Poi_Type poiType, RTree* rTree);
        
        // Query
        virtual void query(Rectangle MBR, int32_t* bitmap, char* &doc, vector<Key_Value>* &pois, RTree* rTree);


    friend class RTree;
};

#endif