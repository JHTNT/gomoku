#ifndef CACHE
#define CACHE
#include <deque>
#include <unordered_map>

template <typename T>
class Cache {
    int capacity = 2000000;
    std::deque<unsigned long long> cache;
    std::unordered_map<unsigned long long, T> cache_map;

   public:
    T& get(unsigned long long key) { return cache_map[key]; }

    void put(unsigned long long key, T value) {
        if (cache_map.find(key) != cache_map.end()) {
            cache_map[key] = value;
        } else {
            if (cache.size() >= capacity) {
                cache_map.erase(cache.front());
                cache.pop_front();
            }
            cache_map[key] = value;
            cache.push_back(key);
        }
    }

    bool has(unsigned long long key) { return cache_map.find(key) != cache_map.end(); }
};
#endif
