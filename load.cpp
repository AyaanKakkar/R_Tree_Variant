#include "load.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>

using namespace std;

const string DATASET_FILE = "dataset.bin";


int32_t loadData(int32_t* &poiID, Rectangle* &MBRs, Poi_Type* &poiTypes) {
    fstream file;
    file.open(DATASET_FILE, ios::in | ios::binary);

    int POIs;


    file.read((char *) &POIs, sizeof(POIs));

    poiID = new int32_t[POIs];
    MBRs = new Rectangle[POIs];
    poiTypes = new Poi_Type[POIs];


    for (int i = 0; i < POIs; i++) {


        file.read((char *) &poiID[i], sizeof(poiID[i]));
        


        file.read((char *) &MBRs[i], sizeof(MBRs[i]));
        
        file.read((char *) &poiTypes[i], sizeof(poiTypes[i]));
        
    }

    file.close();

    return POIs;
}