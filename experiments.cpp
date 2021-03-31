#include <rectangle.h>
#include "rtree.h"
#include "saveGraph.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <vector>
#include <iomanip>

using namespace std;

const long double DEFAULT_WINDOW_PERCENTAGE = 6;
const int DEFAULT_QUERY_TYPES = 4;
const long double startX_MBR = 0;
const long double startY_MBR = 0;

const int EXPERIMENT_NO = 1;

const string INDEX_FILE_NAME = "Tree.bin";


Rectangle getQueryRectangle(long double windowSize) {
    return Rectangle(startX_MBR, startX_MBR + windowSize, startY_MBR, startY_MBR + windowSize);
}

int32_t* getBitmap(int numTypes) {
    int32_t* bitmap = new int32_t[BITMAP_SIZE];
    for (int i = 0; i < BITMAP_SIZE; i++) {
        if (i < numTypes) {
            bitmap[i] = 1;
        }
        else {
            bitmap[i] = 0;
        }
    }

    return bitmap;
}

long double getWindowSize(long double percentage) {
    return 8.00 * percentage / 100.00;
}

int main() {

    if (EXPERIMENT_NO == 1) {

        RTree* rtree = RTree::LoadIndex(INDEX_FILE_NAME);

        cout << "TREE LOADED" << endl;

        cout << "Varying Query Window Size" << endl;

        vector<long double> perc{3, 6, 9, 12, 15};

        vector<int> diskIO(5);

        vector<long double> times(5);

        for (int i = 0; i < perc.size(); i++) {
            Rectangle queryMBR = getQueryRectangle(getWindowSize(perc[i]));

            int32_t* bitmap = getBitmap(DEFAULT_QUERY_TYPES);

            rtree -> diskIO_ = 0;

            char* doc;
            vector<Key_Value>* pois;

            rtree -> queryMBR(queryMBR, bitmap, doc, pois);

            diskIO[i] = rtree -> diskIO_;

            times[i] = diskIO[i]*0.01;

            delete [] doc;
            delete [] pois;
        }

        cout << "Saving Results" << endl;

        saveData("Query_Window_Time_Tree.data", perc, times);
        saveData("Query_Window_IO_Tree.data", perc, diskIO);
    }

    else if (EXPERIMENT_NO == 2){
        RTree* rtree = RTree::LoadIndex(INDEX_FILE_NAME);

        cout << "TREE LOADED" << endl;
        
        cout << "Varying Queried Types" << endl;

        vector<long double> queryTypes{2, 4, 6, 8, 10};

        vector<int> diskIO(5);

        vector<long double> times(5);

        for (int i = 0; i < queryTypes.size(); i++) {
            Rectangle queryMBR = getQueryRectangle(getWindowSize(DEFAULT_WINDOW_PERCENTAGE));

            int32_t* bitmap = getBitmap(queryTypes[i]);

            rtree -> diskIO_ = 0;

            char* doc;
            vector<Key_Value>* pois;

            rtree -> queryMBR(queryMBR, bitmap, doc, pois);

            diskIO[i] = rtree -> diskIO_;

            times[i] = diskIO[i]*0.01;

            delete [] doc;
            delete [] pois;
        }

        cout << "Saving Results" << endl;

        saveData("Queried_Types_Time_Tree.data", queryTypes, times);
        saveData("Queried_Types_IO_Tree.data", queryTypes, diskIO);
    }
    else {
        cout << "Varying Tree Size" << endl;

        vector<long double> poi_count{400000, 800000, 1200000, 1600000, 2000000};
        vector<string> indexFileNames{"Tree_400k.bin", "Tree_800k.bin", "Tree_1200k.bin", "Tree_1600k.bin", "Tree_2000k.bin"};

        vector<int> diskIO(5);

        vector<long double> times(5);

        for (int i = 0; i < poi_count.size(); i++) {

            RTree* rtree = RTree::LoadIndex(indexFileNames[i]);

            cout << "Size : " << poi_count[i] << " | Tree Loaded" << endl; 

            Rectangle queryMBR = getQueryRectangle(getWindowSize(DEFAULT_WINDOW_PERCENTAGE));

            int32_t* bitmap = getBitmap(DEFAULT_QUERY_TYPES);

            rtree -> diskIO_ = 0;

            char* doc;
            vector<Key_Value>* pois;

            rtree -> queryMBR(queryMBR, bitmap, doc, pois);

            diskIO[i] = rtree -> diskIO_;

            times[i] = diskIO[i]*0.01;

            delete [] doc;
            delete [] pois;
            delete rtree;
        }

        cout << "Saving Results" << endl;

        saveData("Tree_Size_Time_Tree.data", poi_count, times);
        saveData("Tree_Size_IO_Tree.data", poi_count, diskIO);
    }

}

