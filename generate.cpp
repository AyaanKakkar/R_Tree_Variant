#include <generate.h>
#include <cstdint>
#include <vector>
#include <rectangle.h>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <random>
#include <iostream>
#include <config.hpp>

using namespace std;

#define MIN(a, b) (a) < (b) ? a : b

int32_t generateData(int32_t size, double skew, int32_t* &poiID, Rectangle* &MBRs, Poi_Type* &poiTypes) {
    
    // Seed
    srand(time(0));
    

    int poiCount = size;

    // IDs
    poiID = new int[poiCount];

    for (int i = 0; i < poiCount; i++) {
        poiID[i] = i;
    }
    

    // MBRs
    MBRs = new Rectangle[poiCount];
    
    // Init bitmaps
    poiTypes = new Poi_Type[poiCount];

    // Bitmaps
    vector <int> bitmap_dist = getBucketFreq(poiCount, BITMAP_DISTRIBUTION_SKEW, BITMAP_SIZE);
    int32_t* bitmap_basket = new int32_t[poiCount];

    int bucketID = 0;
    for (int i = 0; i < poiCount; i++) {
        bitmap_basket[i] = bucketID;
        bitmap_dist[bucketID]--;
        if (bitmap_dist[bucketID] == 0) {
            bucketID++;
        }
    }

    shuffle(bitmap_basket, bitmap_basket + poiCount,
            default_random_engine(time(0)));

    vector <int> subBitmap_dist = getBucketFreq(poiCount, SUB_BITMAP_DISTRIBUTION_SKEW, SUB_BITMAP_SIZE);
    int32_t* subBitmap_basket = new int32_t[poiCount];

    bucketID = 0;
    for (int i = 0; i < poiCount; i++) {
        subBitmap_basket[i] = bucketID;
        subBitmap_dist[bucketID]--;
        if (subBitmap_dist[bucketID] == 0) {
            bucketID++;
        }
    }

    shuffle(subBitmap_basket, subBitmap_basket + poiCount,
            default_random_engine(time(0)));

    // Positioning
    vector<int> bucket = getBucketFreq(size, skew, BUCKET_SIZE);

    int matrix[ROWS][COLS];

    int bucketIDX = 0;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            matrix[i][j] = bucket[bucketIDX++];
        }
    }

    int currIdx = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            for (int k = 0; k < matrix[i][j]; k++) {

                int t1 = rand() % (RAND_MAX - 1);
                int t2 = t1 + (rand() % (MIN(1000, RAND_MAX - t1)));
                long double minX = (long double)j + (((long double)t1) / RAND_MAX);
                long double maxX = (long double)j + (((long double)t2) / RAND_MAX);
                int t3 = rand() % (RAND_MAX - 1);
                int t4 = t3 + (rand() % (MIN(1000, RAND_MAX - t3)));
                long double minY = (long double)i + (((long double)t3) / RAND_MAX);
                long double maxY = (long double)i + (((long double)t4) / RAND_MAX);

                Rectangle temp(minX, maxX, minY, maxY);

                poiTypes[currIdx].bitmapIdx = bitmap_basket[currIdx];
                poiTypes[currIdx].subIdx = subBitmap_basket[currIdx];
                
                MBRs[currIdx++] = temp;
            }
        }
    }

    // Return
    return poiCount;
}


vector<int> getBucketFreq(int32_t size, double skew, int32_t bucketsize) {
    // Calculate bottom
    double bottom = 0.0;
    for(int i=1;i <= bucketsize; i++) {
      bottom += (1/pow(i, skew));
    }
    
    
    // Calculate bucketFrequency
    vector<int> bucket(bucketsize);

    int remainSize = 0;
    for (int i=0;i<bucketsize;i++) {
        bucket[i] = (size * ((1.0 / pow((i+1), skew)) / bottom));
        remainSize += bucket[i];
    }
    remainSize = size - remainSize;
    for (int i=0; i<remainSize;i++){
        bucket[i]++;
    }

    return bucket;
}
