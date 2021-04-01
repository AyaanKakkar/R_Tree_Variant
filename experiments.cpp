#include <rectangle.h>
#include "rtree.h"
#include "saveGraph.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <vector>
#include <iomanip>
#include <config.hpp>

using namespace std;


Rectangle getQueryRectangle(long double windowSize_X, long double windowSize_Y) {
    return Rectangle(QUERY_START_X, QUERY_START_X + windowSize_X, QUERY_START_Y, QUERY_START_Y + windowSize_Y);
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

long double getWindowSize(long double percentage, bool x = true) {
    if (x)
        return (X_END - X_START) * percentage / 100.00;
    else
        return (Y_END - Y_START) * percentage / 100.00;
}

int main() {

    if (EXPERIMENT_NO == 1) {

        RTree* rtree = RTree::LoadIndex(INDEX_FILE_NAME);

        cout << "TREE LOADED" << endl;

        cout << "Varying Query Window Size" << endl;

        vector<long double> perc = PERC;

        vector<int> diskIO(5);

        vector<long double> times(5);

        for (int i = 0; i < perc.size(); i++) {
            Rectangle queryMBR = getQueryRectangle(getWindowSize(perc[i], true), getWindowSize(perc[i], false));

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

        vector<long double> queryTypes = QUERIED_TYPES;

        vector<int> diskIO(5);

        vector<long double> times(5);

        for (int i = 0; i < queryTypes.size(); i++) {
            Rectangle queryMBR = getQueryRectangle(getWindowSize(DEFAULT_WINDOW_PERCENTAGE, true), getWindowSize(DEFAULT_WINDOW_PERCENTAGE, false));

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

        vector<long double> poi_count = POI_COUNT;
        vector<string> indexFileNames = INDEX_FILE_NAMES;

        vector<int> diskIO(5);

        vector<long double> times(5);

        for (int i = 0; i < poi_count.size(); i++) {

            RTree* rtree = RTree::LoadIndex(indexFileNames[i]);

            cout << "Size : " << poi_count[i] << " | Tree Loaded" << endl; 

            Rectangle queryMBR = getQueryRectangle(getWindowSize(DEFAULT_WINDOW_PERCENTAGE, true), getWindowSize(DEFAULT_WINDOW_PERCENTAGE, false));

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

