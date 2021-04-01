#include <generate.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <vector>
#include <iomanip>
#include <fstream>
#include <poiType.hpp>
#include <config.hpp>

using namespace std;

int32_t* poiID = nullptr;

Rectangle* MBRs = nullptr;

Poi_Type* poiTypes = nullptr;




int main() {

    int poi_count = generateData(POIs, SKEW, poiID, MBRs, poiTypes);

    fstream file;
    file.open(DATASET_FILE, ios::binary | ios::out);

    file.write((char *) &POIs, sizeof(POIs));

    for (int i = 0; i < POIs; i++) {

        file.write((char *) &poiID[i], sizeof(poiID[i]));
        
        file.write((char *) &MBRs[i], sizeof(MBRs[i]));
        
        file.write((char *) &poiTypes[i], sizeof(poiTypes[i]));
    }

    file.close();

    return 0;
}



