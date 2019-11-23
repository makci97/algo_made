#include <vector>
#include "Huffman.h"

typedef unsigned char t_byte;


class BitsWriter
{
public:
    void WriteBit(bool bit);
    
    void WriteByte(t_byte byte);
    
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
    copyStream(original, compressed);
}

void Decode(IInputStream &compressed, IOutputStream &original)
{
    copyStream(compressed, original);
}
