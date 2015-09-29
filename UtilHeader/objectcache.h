#ifndef __QROX_OBJECT_CACHE_H__
#define __QROX_OBJECT_CACHE_H__

#include <map>
#include <list>
#include <functional>

template <typename K, typename V>
class objectcache {
public:
    enum class replacement_rule {
        fifo,   // first in first out
        lru,    // least recently used
    };

private:
    std::map<K, V> cached;
    std::list<K> replace;
    int cache_size;
    replacement_rule const reprule;
    std::function<V (K)> const load;
    std::function<void (K, V)> const unload;

public:
    objectcache(int cache_size, replacement_rule reprule, std::function<V (K)> load, std::function<void (K, V)> unload);

    V get(K key);
    void clear();
};

template <typename K, typename V>
objectcache<K, V>::objectcache(int cache_size, replacement_rule reprule,
        std::function<V (K)> load, std::function<void (K, V)> unload)
    : cache_size(cache_size), reprule(reprule), load(load), unload(unload) {
}

template <typename K, typename V>
V objectcache<K, V>::get(K key) {
    auto it = cached.find(key);
    if (it != cached.end()) {
        if (reprule == replacement_rule::lru) {
            for (auto i = replace.begin(); i != replace.end(); ++i) {
                if (key == *i) {
                    replace.erase(i);
                    replace.push_back(key);
                    break;
                }
            }
        }
        return it->second;
    } else {
        while (cached.size() >= cache_size) {
            K remove_key = replace.front();
            replace.pop_front();
            auto remove_it = cached.find(remove_key);
            if (remove_it != cached.end()) {
                V remove_val = remove_it->second;
                cached.erase(remove_it);
                unload(remove_key, remove_val);
            }
        }
        V val = load(key);
        cached.insert(std::pair<K, V>(key, val));
        replace.push_back(key);
        return val;
    }
}

template <typename K, typename V>
void objectcache<K, V>::clear() {
    for (auto i = cached.begin(); i != cached.end(); ++i) {
        unload(i->first, i->second);
    }
    cached.clear();
    replace.clear();
}

#endif
