#ifndef _CONFIG
#define _CONFIG

#include <string>
#include <vector>

using namespace std;

// Global Information
const string DATASET_FILE = "dataset.bin";

const string INDEX_FILE_NAME = "Tree.bin";

const int BITMAP_SIZE = 10;

const int SUB_BITMAP_SIZE = 5;

const int PAGE_SIZE = 16384;
const int MAX_ENTRIES = 128;
const int MIN_ENTRIES = 64;

const long double X_START = 0;
const long double X_END = 8;

const long double Y_START = 0;
const long double Y_END = 8;


// Experiments
const int EXPERIMENT_NO = 1;

const long double DEFAULT_WINDOW_PERCENTAGE = 6;

const int DEFAULT_QUERY_TYPES = 4;

const long double QUERY_START_X = 0;
const long double QUERY_START_Y = 0;


const vector<long double> PERC{3, 6, 9, 12, 15};

const vector<long double> QUERIED_TYPES{2, 4, 6, 8, 10};

const vector<long double> POI_COUNT{400000, 800000, 1200000, 1600000, 2000000};
const vector<string> INDEX_FILE_NAMES{"Tree_400k.bin", "Tree_800k.bin", "Tree_1200k.bin", "Tree_1600k.bin", "Tree_2000k.bin"};


// Synthetic Data Generation
const int POIs = 100000;

const double SKEW = 0.7;
const double BITMAP_DISTRIBUTION_SKEW = 0.7;
const double SUB_BITMAP_DISTRIBUTION_SKEW = 0.7;

const int BUCKET_SIZE = 64;
const int ROWS = 8;
const int COLS = 8;

#endif