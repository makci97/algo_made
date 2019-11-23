#include <fstream>
#include <vector>
#include <cassert>


class HashTable
{
public:
    HashTable(): _size(0), _capacity(8), _mul_hash_const(7),
                 _table(_capacity), _is_removed(_capacity, false) { }
    
    bool has(const std::string& key) const;
    bool add(const std::string& key);
    bool remove(const std::string& key);

private:
    int _hash(const std::string& key) const;
    int _next_i(int cur_prob, int next_prob_i) const;
    void _realloc();
    
    int _size;
    int _capacity;
    int _mul_hash_const;
    std::vector<std::string> _table;
    std::vector<bool> _is_removed;
};


bool HashTable::has(const std::string& key) const
{
    int cur_prob = _hash(key);
    for (int i = 0; i < _capacity; ++i)
    {
        if (_table[cur_prob] == key)
            return true;
        else if (_table[cur_prob].empty() && !_is_removed[cur_prob])
            return false;
        else
            cur_prob = _next_i(cur_prob, i + 1);
    }
    return false;
}

bool HashTable::add(const std::string& key)
{
    if (has(key))
        return false;
    
    // load factor
    if (_size > 0.75 * _capacity)
        _realloc();
    
    int cur_prob = _hash(key);
    for (int i = 0; i < _capacity; ++i)
    {
        if (_table[cur_prob].empty())
        {
            _table[cur_prob] = key;
            ++_size;
            return true;
        }
        else
            cur_prob = _next_i(cur_prob, i + 1);
    }
    // must find place for key
    assert(false);
}

bool HashTable::remove(const std::string& key)
{
    if (!has(key))
        return false;
    
    int cur_prob = _hash(key);
    for (int i = 0; i < _capacity; ++i)
    {
        if (_table[cur_prob] == key)
        {
            _table[cur_prob].clear();
            _is_removed[cur_prob] = true;
            --_size;
            return true;
        }
        else
            cur_prob = _next_i(cur_prob, i + 1);
    }
    // must remove key if it exist
    assert(false);
}

int HashTable::_hash(const std::string& key) const
{
    int result = 0;
    for(auto const& s: key)
    {
        result = (result * _mul_hash_const + s) % _capacity;
    }
    return result;
}

int HashTable::_next_i(int cur_prob, int next_prob_i) const
{
    return (cur_prob + next_prob_i) % _capacity;
}

void HashTable::_realloc()
{
    _capacity *= 2;
    _mul_hash_const = _capacity - 1;
    
    auto old_table = std::move(_table);
    _table = std::move(std::vector<std::string>(_capacity));
    _is_removed = std::move(std::vector<bool>(_capacity));
    
    for (auto const& key: old_table)
    {
        if (key.empty())
            continue;
        add(key);
    }
}

int main() {
    std::ifstream in_stream;
    std::ofstream out_stream;
    in_stream.open("input.txt");
    out_stream.open("output.txt");
    
    std::string str;
    HashTable table;
    bool is_ok = false;
    while (std::getline(in_stream, str))
    {
        auto key = str.substr(2);
        switch (str[0])
        {
            case '+':
                is_ok = table.add(key);
                break;
            case '?':
                is_ok = table.has(key);
                break;
            case '-':
                is_ok = table.remove(key);
                break;
            default:
                assert(false);
                break;
        }
        if (is_ok)
            out_stream << "OK" << std::endl;
        else
            out_stream << "FAIL" << std::endl;
    }
    
    // clean
    in_stream.close();
    out_stream.close();
    
    return 0;
}