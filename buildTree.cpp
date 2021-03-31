#include "rtree.h"
#include "generate.h"
#include "load.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <vector>
#include <iomanip>
#include <poiType.h>

using namespace std;

const string indexFileName = "Tree_100k.bin";

const int32_t pageSize = 16384;
const int32_t maxEntries = 128;
const int32_t minEntries = 64;

int32_t poi_count;
int32_t* poiID = nullptr;

Rectangle* MBRs = nullptr;

Poi_Type* poiTypes = nullptr;

RTree* rtree = nullptr;


int main() {

    poi_count = loadData(poiID, MBRs, poiTypes);

    
    rtree = RTree::CreateIndex(indexFileName, pageSize, maxEntries, minEntries);
    
    for (int32_t i = 0; i < poi_count; i++) {
        
        if (i % 10000 == 0) {
            cout << "i = " << i << endl;
        }
        
        rtree -> insert(MBRs[i], poiID[i], poiTypes[i]);
    }
}

