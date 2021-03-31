#ifndef _GENERATE
#define _GENERATE
#include <cstdint>
#include <rectangle.h>
#include <poiType.h>

#include <vector>

using namespace std;

vector<int> getBucketFreq(int32_t size, double skew, int32_t bucketsize);

int32_t generateData(int32_t size, double skew, int32_t* &poiID, Rectangle* &MBRs, Poi_Type* &poiTypes);

#endif