#include <fstream>
#include <vector>
#include <iostream>
#include <queue>
#include "Huffman.h"


static std::vector<char> ReadAllBytes(std::ifstream& ifs)
{
    std::ifstream::pos_type pos = ifs.tellg();
    
    std::vector<char> result(pos);
    
    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);
    
    return result;
}

struct InputStream: IInputStream
{
    explicit InputStream(std::ifstream&& in_stream) {
        stream = std::move(in_stream);
//        std::string line;
        file = ReadAllBytes(stream);
//        while (std::getline(stream, line))
//        {
//            for(auto c: line)
//            {
//                file.push_back(c);
//            }
//        }
        std::reverse(std::begin(file), std::end(file));
    }
    bool Read(t_byte& value) override;
    ~InputStream() override {stream.close();}
    
    std::ifstream stream;
    std::vector<char> file;
};

bool InputStream::Read(t_byte &value)
{
    if (file.empty())
        return false;
    else
    {
        value = t_byte(file.back());
        file.pop_back();
        return true;
    }
}

struct OutputStream: IOutputStream
{
    explicit OutputStream(std::ofstream&& out_stream) {
        stream = std::move(out_stream);
    }
    void Write(t_byte value) override;
    ~OutputStream() override {stream.close();}
    
    std::ofstream stream;
};

void OutputStream::Write(t_byte value)
{
    stream << value;
}

int main()
{
    // IInputStream
    std::ifstream tmp_in_stream("input.txt", std::ios::binary|std::ios::ate);
    IInputStream* in_stream = new InputStream(std::move(tmp_in_stream));
    
    // IOutputStream
    std::ofstream tmp_out_stream;
    tmp_out_stream.open("output.txt");
    IOutputStream* out_stream = new OutputStream(std::move(tmp_out_stream));
    
    Encode(*in_stream, *out_stream);
    delete in_stream;
    delete out_stream;
    
    // IInputStream
    std::ifstream tmp_in_stream_decode("output.txt", std::ios::binary|std::ios::ate);
    IInputStream* in_stream_decode = new InputStream(std::move(tmp_in_stream_decode));
    
    // IOutputStream
    std::ofstream tmp_out_stream_decode;
    tmp_out_stream_decode.open("output2.txt");
    IOutputStream* out_stream_decode = new OutputStream(std::move(tmp_out_stream_decode));
    Decode(*in_stream_decode, *out_stream_decode);
    
    delete in_stream_decode;
    delete out_stream_decode;
    
    return 0;
}
