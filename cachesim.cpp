// cachesim.cpp
#include <bits/stdc++.h>
#include <iostream>

using namespace std;

struct CacheLine {
    bool valid = false;
    bool dirty = false;
    uint32_t tag = 0;
    uint64_t lru_time = 0;
};

class Cache {
    public:
        Cache(int csize, int bsize, int ways, bool wb)
          : cache_size(csize), block_size(bsize), write_back(wb)
        {
            int num_blocks = cache_size / block_size;
            assoc = (ways == -1 ? num_blocks : ways);
            num_sets = num_blocks / assoc;
            sets.resize(num_sets, vector<CacheLine>(assoc));
            reset_stats();
        }
    
        void reset_stats() {
            total_requests = hits = misses = 0;
            bytes_mem_to_cache = bytes_cache_to_mem = 0;
            global_time = 0;
            for (auto &S : sets)
                for (auto &L : S)
                    L = CacheLine();
        }
    
        void access(bool is_write, uint32_t addr) {
            ++total_requests;
            ++global_time;
            uint32_t block_addr = addr / block_size;
            uint32_t set_idx    = block_addr % num_sets;
            uint32_t tag        = block_addr / num_sets;
    
            auto &S = sets[set_idx];
            // 1) Hit?
            for (auto &L : S) {
                if (L.valid && L.tag == tag) {
                    ++hits;
                    L.lru_time = global_time;
                    if (is_write) {
                        if (write_back) {
                            L.dirty = true;
                        } else {
                            // write-through
                            bytes_cache_to_mem += block_size;
                        }
                    }
                    return;
                }
            }
    
            // 2) Miss
            ++misses;
            bytes_mem_to_cache += block_size;  // fetch new block
    
            // find an invalid line first
            CacheLine *victim = nullptr;
            for (auto &L : S) {
                if (!L.valid) { victim = &L; break; }
            }
            // if none invalid, evict LRU
            if (!victim) {
                victim = &*min_element(S.begin(), S.end(),
                    [](auto &a, auto &b){ return a.lru_time < b.lru_time; });
                if (victim->dirty && write_back) {
                    // write-back dirty block
                    bytes_cache_to_mem += block_size;
                }
            }
    
            // install new block
            victim->valid    = true;
            victim->tag      = tag;
            victim->lru_time = global_time;
            victim->dirty    = (is_write && write_back);
            if (is_write && !write_back) {
                // write-through on write miss
                bytes_cache_to_mem += block_size;
            }
        }
    
        // public stats
        uint64_t total_requests, hits, misses;
        uint64_t bytes_mem_to_cache, bytes_cache_to_mem;
    
    private:
        int       cache_size, block_size, assoc, num_sets;
        bool      write_back;
        uint64_t  global_time;
        vector<vector<CacheLine>> sets;
    };
    
    int main() {
        ifstream fin("test.trace");
        if (!fin) {
            cerr << "Unable to open test.trace\n";
            return 1;
        }
        ofstream fout("test.result");
        if (!fout) {
            cerr << "Unable to open test.result\n";
            return 1;
        }
    
        // configurations
        vector<int>    cache_sizes  = {1024, 2048, 8192, 65536};
        vector<int>    block_sizes  = {4, 8, 32, 256};
        vector<int>    assoc_vals   = {1, 2, 4, -1};  // -1 => fully-assoc
        vector<string> assoc_str    = {"DM","2W","4W","FA"};
        vector<bool>   wb_policy    = {true, false};
        vector<string> wb_str       = {"WB","WT"};
    
        // simulate each config
        for (int cs : cache_sizes) {
          for (int bs : block_sizes) {
            for (int ai = 0; ai < (int)assoc_vals.size(); ++ai) {
              int ways = assoc_vals[ai];
              for (int wbi = 0; wbi < 2; ++wbi) {
                Cache c(cs, bs, ways, wb_policy[wbi]);
                // reset and replay trace
                fin.clear();
                fin.seekg(0, ios::beg);
                string op, addr_hex;
                while (fin >> op >> addr_hex) {
                    uint32_t addr = stoul(addr_hex, nullptr, 16);
                    c.access(op == "write", addr);
                }
    
                double hit_rate = double(c.hits) / c.total_requests;
                int blocks_per_set = (ways == -1 ? (cs/bs) : ways);
    
                // output line
                fout << cs << " "
                     << bs << " "
                     << assoc_str[ai] << " "
                     << blocks_per_set << " "
                     << wb_str[wbi] << " "
                     << c.total_requests << " "
                     << c.hits << " "
                     << fixed << setprecision(2) << hit_rate << " "
                     << c.bytes_mem_to_cache << " "
                     << c.bytes_cache_to_mem
                     << "\n";
              }
            }
          }
        }
    
        std::cout << "Success" << std::endl;
        return 0;
    }
    