#include <generate.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <vector>
#include <iomanip>
#include <fstream>
#include <poiType.h>

using namespace std;

const string DATASET_FILE = "dataset.bin";
const int POIs = 100000;

const double skew = 0.7;

int32_t* poiID = nullptr;

Rectangle* MBRs = nullptr;

Poi_Type* poiTypes = nullptr;




int main() {

    int poi_count = generateData(POIs, skew, poiID, MBRs, poiTypes);

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



