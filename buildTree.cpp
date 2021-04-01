#include "rtree.h"
#include "generate.h"
#include "load.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <vector>
#include <iomanip>
#include <poiType.hpp>
#include <config.hpp>

using namespace std;

int32_t poi_count;
int32_t* poiID = nullptr;

Rectangle* MBRs = nullptr;

Poi_Type* poiTypes = nullptr;

RTree* rtree = nullptr;


int main() {

    poi_count = loadData(poiID, MBRs, poiTypes);
   
    rtree = RTree::CreateIndex(INDEX_FILE_NAME, PAGE_SIZE, MAX_ENTRIES, MIN_ENTRIES);
    
    for (int32_t i = 0; i < poi_count; i++) {
        rtree -> insert(MBRs[i], poiID[i], poiTypes[i]);
    }

    cout << "Tree Succesfully Built" << endl;
}

