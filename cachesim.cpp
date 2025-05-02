// cachesim.cpp
#include <bits/stdc++.h>
using namespace std;

struct CacheLine {
    bool valid = false;
    bool dirty = false;
    uint32_t tag = 0;
    uint64_t lru_time = 0;
};

