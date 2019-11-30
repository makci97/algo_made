#include <vector>
#include <unordered_map>
#include <queue>
#include <cassert>
#include <algorithm>
#include "Huffman.h"

typedef unsigned char t_byte;


class BitsWriter
{
public:
    void WriteBit(bool bit);
    
    void WriteByte(t_byte byte);
    
    void WriteInt(unsigned int num);
    
    std::vector<t_byte> GetResult();

private:
    std::vector<t_byte> buffer_;
    t_byte accumulator_ = 0;
    unsigned int bits_count_ = 0;
};

void BitsWriter::WriteBit(bool bit)
{
    // Ставим бит в аккумулятор на нужное место
    accumulator_ |= static_cast<t_byte>(bit) << bits_count_;
    ++bits_count_;
    if (bits_count_ == 8)
    {
        bits_count_ = 0;
        buffer_.push_back(accumulator_);
        accumulator_ = 0;
    }
}

void BitsWriter::WriteByte(t_byte byte)
{
    if (bits_count_ == 0)
    {
        buffer_.push_back(byte);
    }
    else
    {
        accumulator_ |= byte << bits_count_;
        buffer_.push_back(accumulator_);
        accumulator_ = byte >> (8 - bits_count_);
    }
}

void BitsWriter::WriteInt(unsigned int num)
{
    std::deque<bool> bin_num;
    while(num != 0)
    {
        bin_num.push_back(bool(num % 2));
        num /= 2;
    }
    while (bin_num.size() < 32)
    {
        bin_num.push_front(false);
    }
    buffer_.insert(buffer_.end(), bin_num.begin(), bin_num.end());
}

std::vector<t_byte> BitsWriter::GetResult()
{
    if (bits_count_ != 0)
    {
        // Добавляем в буфер аккумулятор, если в нем что-то есть.
        buffer_.push_back(accumulator_);
    }
    buffer_.push_back(static_cast<t_byte>(bits_count_));
    return std::move(buffer_);
}


class Tree
{
public:
    explicit Tree(int weight=0, t_byte leaf_char='\0'):\
        _weight(weight), _leaf_char(leaf_char), _left(nullptr), _right(nullptr) {}
    Tree(std::vector<bool>& dfs_code, std::vector<t_byte>& leaf_chars);
    ~Tree(){ delete _left; delete _right; }
    
    Tree* merge(Tree* other);
    std::pair<std::vector<bool>, std::vector<t_byte> > get_coded_tree() const;
    void make_char2bits(
            std::unordered_map<t_byte, std::vector<bool> >& char2bits,
            std::vector<bool>& cur_bits
    ) const;
    int get_weight() { return _weight; }
    
    bool operator <(const Tree& other) { return _weight < other._weight; }
    
    friend Tree* decode(bool command, Tree* root, Tree* cur_node, IOutputStream& output);

private:
    int _weight;
    t_byte _leaf_char;
    Tree* _left;
    Tree* _right;
    
    static void _add_child_code(const Tree* child, std::vector<bool>& dfs_code, std::vector<t_byte>& leaf_chars);
};

Tree::Tree(std::vector<bool>& dfs_code, std::vector<t_byte>& leaf_chars):
    _weight(-1), _leaf_char('\0'), _left(nullptr), _right(nullptr)
{
    // reverse dfs
    // down == 1, up == 0
    if (dfs_code.empty())
        return;
    
    bool cur_command = dfs_code.back();
    dfs_code.pop_back();
    if (cur_command)
    {
        // has a child => don't a leaf => hasn't a _leaf_char
        _left = new Tree(dfs_code, leaf_chars);
    
        cur_command = dfs_code.back();
        dfs_code.pop_back();
        if (cur_command)
        {
            // has two children
            _right = _left;
            _left = new Tree(dfs_code, leaf_chars);
            
            // up
            if (dfs_code.empty())
                return;
            assert(!dfs_code.back());
            dfs_code.pop_back();
        }
    }
    else
    {
        // a leaf
        _leaf_char = leaf_chars.back();
        leaf_chars.pop_back();
    }
}

Tree* Tree::merge(Tree* other)
{
    Tree* merged_tree = new Tree(_weight + other->_weight);
    merged_tree->_left = this;
    merged_tree->_right = other;
    
    return merged_tree;
}

std::pair<std::vector<bool>, std::vector<t_byte> > Tree::get_coded_tree() const
{
    // down == 0, up == 1
    std::vector<bool> dfs_code;
    std::vector<t_byte> leaf_chars;
    if (_left != nullptr)
        _add_child_code(_left, dfs_code, leaf_chars);
    if (_right != nullptr)
        _add_child_code(_right, dfs_code, leaf_chars);
    if (_left == nullptr && _right == nullptr)
        leaf_chars.push_back(_leaf_char);
    
    return {dfs_code, leaf_chars};
}

void Tree::make_char2bits(
        std::unordered_map<t_byte, std::vector<bool> >& char2bits,
        std::vector<bool>& cur_bits
) const
{
    if (_left != nullptr)
    {
        cur_bits.push_back(false);
        _left->make_char2bits(char2bits, cur_bits);
        cur_bits.pop_back();
    }
    if (_right != nullptr)
    {
        cur_bits.push_back(true);
        _right->make_char2bits(char2bits, cur_bits);
        cur_bits.pop_back();
    }
    if (_left == nullptr && _right == nullptr)
        char2bits[_leaf_char] = cur_bits;
}

void Tree::_add_child_code(const Tree* child, std::vector<bool>& dfs_code, std::vector<t_byte>& leaf_chars)
{
    dfs_code.push_back(false);
    
    auto child_code_chars_pair = child->get_coded_tree();
    
    // update dfs_code
    dfs_code.reserve(dfs_code.size() + child_code_chars_pair.first.size());
    dfs_code.insert(dfs_code.end(), child_code_chars_pair.first.begin(), child_code_chars_pair.first.end());
    
    // update leaf_chars
    leaf_chars.reserve(leaf_chars.size() + child_code_chars_pair.second.size());
    leaf_chars.insert(leaf_chars.end(), child_code_chars_pair.second.begin(), child_code_chars_pair.second.end());
    
    dfs_code.push_back(true);
}

// Compares two Trees
bool compare_trees(Tree* first, Tree* second)
{
    return second->operator<(*first);
}

Tree* decode(bool command, Tree* root, Tree* cur_node, IOutputStream& output)
{
    // 0 == left, 1 == right
    if (!command)
        cur_node = cur_node -> _left;
    else
        cur_node = cur_node -> _right;
    
    if (cur_node -> _left == nullptr && cur_node -> _right == nullptr)
    {
        t_byte decoded = cur_node -> _leaf_char;
        output.Write(decoded);
        return root;
    }
    return cur_node;
}

static void copyStream(IInputStream &input, IOutputStream &output)
{
    t_byte value;
    while (input.Read(value))
    {
        output.Write(value);
    }
}

void Encode(IInputStream &original, IOutputStream &compressed)
{
    t_byte read_byte;
    std::unordered_map<t_byte, int> counter;
    std::vector<t_byte> msg;
    while (original.Read(read_byte))
    {
        msg.push_back(read_byte);
        counter[read_byte] += 1;
    }
    
    std::vector<Tree*> queue;
    queue.reserve(counter.size());
    for (auto char_count_pair: counter)
        queue.push_back(new Tree(char_count_pair.second, char_count_pair.first));
    std::sort(queue.begin(), queue.end(), compare_trees);
    
    // build Tree
    while (queue.size() > 1)
    {
        auto first = queue.back();
        queue.pop_back();
        auto second = queue.back();
        queue.pop_back();
    
        queue.push_back(first->merge(second));
        std::sort(queue.begin(), queue.end(), compare_trees);
    }
    auto tree = queue.back();
    
    // char2bits
    std::unordered_map<t_byte, std::vector<bool> > char2bits;
    std::vector<bool> cur_bits;
    char2bits.reserve(counter.size());
    tree->make_char2bits(char2bits, cur_bits);
    
    // encode n_leafs
    BitsWriter encoded;
    encoded.WriteByte(t_byte(counter.size())); // < 256
    
    // encode tree
    auto code_chars_pair = tree->get_coded_tree();
    encoded.WriteByte(t_byte((code_chars_pair.first.size() % 256u) >> 256u));    // dfs_code_len
    encoded.WriteByte(t_byte(code_chars_pair.first.size() / 256u));    // dfs_code_len
    for (auto byte: code_chars_pair.second)
    {
        encoded.WriteByte(byte);
    }
    for (auto bit: code_chars_pair.first)
    {
        encoded.WriteBit(bit);
    }
    
    // encode msg
    for (auto c: msg)
        for (bool bit: char2bits[c])
            encoded.WriteBit(bit);
        
    // write encoded
    auto result = encoded.GetResult();
    for (auto byte: result)
        compressed.Write(byte);
}

void Decode(IInputStream &compressed, IOutputStream &original)
{
    std::vector<bool> bits;
    bool bit;
    t_byte read_byte;
    unsigned int n_leafs = 0, dfs_code_len = 0;
    // read bits
    for (int i = 0; ; ++i)
    {
        if (!compressed.Read(read_byte))
            break;
        
        if (i == 0)
        {
            n_leafs = static_cast<int>(read_byte);
            continue;
        }
        if (i == 1)
        {
            dfs_code_len = static_cast<int>(read_byte);
            continue;
        }
        if (i == 2)
        {
            dfs_code_len = (dfs_code_len << 256u) | static_cast<int>(read_byte);
            continue;
        }
        for (unsigned int j = 0; j < 8; ++j)
        {
            bit = ((read_byte >> j) & 0x01u);
            bits.push_back(bit);
        }
    }
    
    // remove trash in end
    int bits_count = static_cast<int>(read_byte);
    bits.resize(bits.size() - 8 - (8 - bits_count));
    
    // to read in right order
    std::reverse(std::begin(bits), std::end(bits));
    
    // read leaf_chars
    std::vector<t_byte> leaf_chars;
    for (int i = 0; i < n_leafs; ++i)
    {
        t_byte byte = 0;
        for (unsigned int j = 0; j < 8; ++j)
        {
            byte = byte | (static_cast<t_byte>(bits.back()) << j);
            bits.pop_back();
        }
        leaf_chars.push_back(byte);
    }
    
    // read dfs_code
    std::vector<bool> dfs_code;
    for (unsigned int i = 0; i < dfs_code_len; ++i)
    {
        dfs_code.push_back(bits.back());
        bits.pop_back();
    }
    
    // build Tree
    Tree tree = Tree(dfs_code, leaf_chars);
    
    // decode msg
    Tree* cur_node = &tree;
    std::reverse(std::begin(bits), std::end(bits));
    for (auto command: bits)
    {
        cur_node = decode(command, &tree, cur_node, original);
    }
    
    bits.size();
}
