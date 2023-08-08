#include <type_traits>
#include <iostream>

#include <functional>
#include <utility>
#include <memory>
#include <limits>




constexpr bool is_power_of_two(int n){
    return (n != 0) && ((n & (n - 1)) == 0);
}

//need -std=c++20
template<int N>
concept po2_t = is_power_of_two(N);

//modulo of power of 2
//N has to be power of 2
//only if N is power of 2
template<int N>
constexpr int mod_p2(int n) requires po2_t<N>
{
    static_assert(is_power_of_two(N), "N has to be power of 2");
    return n % N;       //compiler should be able to optimize this?
}

constexpr int mod_2N(int n, int N){
    //n % (2^N)
    return n & ((1<<N)-1);
}



class default_po2_growther{
    public:
        // Given a hash and the current capacity, use bit-wise trick to return the bucket index.
        static constexpr auto compute_index(std::size_t hash, std::size_t capacity) -> std::size_t
        {
            return hash & (capacity - 1); //last x bits in the hash
        }

        // Given a desired new capacity for the bucket container, pick the closest power of two. 
        static constexpr auto compute_closest_capacity(std::size_t min_capacity) -> std::size_t
        {
            constexpr auto highest_capacity = (std::size_t{1} << (std::numeric_limits<std::size_t>::digits - 1));

            if (min_capacity > highest_capacity) {
                std::cout << "Maximum capacity for the dense_hash_map reached." << std::endl;
                return highest_capacity;
            }

            --min_capacity;

            for (auto i = 1; i < std::numeric_limits<std::size_t>::digits; i *= 2) {
                min_capacity |= min_capacity >> i;
            }

            return ++min_capacity;
        }

        // Returns the minimum capacity (~= initial capacity) the bucket container must have.
        static constexpr auto minimum_capacity() -> std::size_t { return 8u; }
};


// hash indice a bucket
// bucket points to a contiguous memory
// item in the memory is a struct of {key, ptr_to_data, ptr_to_next}
template<
    class Key,
    class Data
>
struct Entries{
    Key  key;
    std::unique_ptr<Data> data;          //store
    Entries* next = nullptr;

    class Iter;
    Iter begin(){
        return Iter(this);
    }
    Iter end(){
        return Iter(nullptr);
    }

    void reset(){
        data = nullptr;         //clear the data
    }

    Entries* FindLast(const Key& key){
        Entries* curr = this;
        while(curr->next != nullptr){
            //check duplicate keys
            if(curr->key == key){
                return nullptr;
            }
            curr = curr->next;
        }

        return curr;
    }

    Entries* Find(const Key& key){
        Entries* curr = this;
        while(curr != nullptr){
            if(curr->key == key){
                return curr;
            }
            curr = curr->next;
        }
        return nullptr;
    }

    class Iter{
        Entries* curr;
    public:
        Iter(Entries* curr): curr(curr){}
        Iter& operator++(){
            curr = curr->next;
            return *this;
        }
        bool operator!=(const Iter& other){
            return curr != other.curr;
        }
        auto operator*(){
            return curr->data.get();
        }
    };

};


template<
    class Key,
    class Data,
    class Hash = std::hash<Key>,
    class Equal = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, Data>>,
    class Growther  = default_po2_growther
>
class HashTable{
private:
    std::size_t _size;
    std::size_t _used_size;
    std::vector<Entries<Key, Data>> _entries;
    std::vector<int>     _buckets;
    std::vector<int>     _free_list;
public:
    HashTable(std::size_t size=Growther::minimum_capacity()):
        _size(size),
        _entries(),
        _buckets(),
        _used_size(0),
        _free_list()

    {
        _entries.reserve(size);
        _buckets.resize(size, -1);
    }

    //iterator
    auto begin() -> decltype(_entries.begin()){
        return _entries.begin();
    }
    auto end() -> decltype(_entries.end()){
        return _entries.end();
    }

    void Grow(){
        //at least 50% more then the current used
        std::size_t new_size = Growther::compute_closest_capacity(_used_size + (_used_size>>1));
        std::cout << "new size: " << new_size << std::endl;
        std::vector<int> new_buckets(new_size, -1);
        std::vector<Entries<Key, Data>> new_entries;
        new_entries.reserve(new_size);
        //rehash all the entries
        //TODO:


        _size = new_size;
        _buckets = std::move(new_buckets);
        _entries = std::move(new_entries);
    }

    void tryGrow(){
        if(_used_size > _size>>1){
            std::cout << "trigger grow, current used used size / size: " << _used_size << "/" << _size << std::endl;
            Grow();
        }
    }

    //data will be moved if insert success
    //forward the data reference type to std::make_unique
    template<class DataT>
    bool Insert(const Key& key, const DataT&& data) requires std::is_same_v<Data, std::remove_reference_t<DataT>>
    {
        std::size_t hash = Hash{}(key);
        std::size_t index = Growther::compute_index(hash, _size);
        std::cout << "hash: " << hash << " index: " << index << std::endl;

        if(_buckets[index] == -1){
            //empty bucket
            _entries.push_back(Entries<Key, Data>{key, std::make_unique<Data>(data)});
            _buckets[index] = _entries.size()-1;
            return true;
        } else {
            //collision
            //check if the key already exists
            int curr = _buckets[index];
            auto last = _entries[curr].FindLast(key);

            //key exist, do not need to resize if failed
            if(last == nullptr) return false;

            using Data_noref = std::remove_reference_t<Data>;

            //key does not exist, insert
            _entries.push_back(Entries<Key, Data>{key, std::make_unique<Data_noref>(std::forward<Data>(data))});
            last->next = &_entries.back();
        }
        _used_size++;

        tryGrow();
        return true;
    }

    bool remove(const Key& key){
        std::size_t hash = Hash{}(key);
        std::size_t index = Growther::compute_index(hash, _size);
        std::cout << "hash: " << hash << " index: " << index << std::endl;

        if(_buckets[index] == -1){
            //empty bucket
            return false;
        } else {
            auto curr = _entries[_buckets[index]];
            decltype(curr) prev = nullptr;
            while(curr != nullptr){
                if(curr->key == key){
                    //found
                    if(prev == nullptr){
                        //first entry
                        _buckets[index] = curr->next;
                        curr->reset();
                    } else {
                        prev->next = curr->next;
                        curr->reset();
                    }
                }
                prev = curr;
                curr = curr->next;
            }
        }
    }

    auto begin() const -> decltype(_entries.begin()){
        return _entries.begin();
    }
    auto end() const -> decltype(_entries.end()){
        return _entries.end();
    }
};

