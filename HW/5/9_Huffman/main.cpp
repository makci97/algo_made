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
//        while (std::getline(stream, line))
//        {
//            for(auto c: line)
//            {
//                file.push_back(c);
//            }
//        }


//        file = ReadAllBytes(stream);

        byte value;
        while (stream >> value)
            file.push_back(value);
        
        std::reverse(std::begin(file), std::end(file));
    }
    bool Read(byte& value) override;
    ~InputStream() override {stream.close();}
    
    std::ifstream stream;
    std::vector<byte> file;
};

bool InputStream::Read(byte &value)
{
    if (file.empty())
        return false;
    else
    {
        value = byte(file.back());
        file.pop_back();
        return true;
    }
}

struct OutputStream: IOutputStream
{
    explicit OutputStream(std::ofstream&& out_stream) {
        stream = std::move(out_stream);
    }
    void Write(byte value) override;
    ~OutputStream() override {stream.close();}
    
    std::ofstream stream;
};

void OutputStream::Write(byte value)
{
    stream << value;
}



struct VectorInputStream: IInputStream
{
    VectorInputStream(std::vector<byte>& file): _file(file)
    {
        std::reverse(std::begin(_file), std::end(_file));
    }
    bool Read(byte& value) override
    {
        if (_file.empty())
            return false;
        else
        {
            value = byte(_file.back());
            _file.pop_back();
            return true;
        }
    }
    
    std::vector<byte> _file;
};

struct VectorOutputStream: IOutputStream
{
    void Write(byte value) override
    {
        _file.push_back(value);
    }
    
    std::vector<byte>&& get_file() { return std::move(_file);}
    
    std::vector<byte> _file;
};

int main()
{
//    // IInputStream
//    std::ifstream tmp_in_stream("input.txt", std::ios::binary|std::ios::ate);
////    std::ifstream tmp_in_stream("input.jpg", std::ios::binary|std::ios::ate);
//    IInputStream* in_stream = new InputStream(std::move(tmp_in_stream));
//
//    // IOutputStream
//    std::ofstream tmp_out_stream;
//    tmp_out_stream.open("output.txt");
//    IOutputStream* out_stream = new OutputStream(std::move(tmp_out_stream));
//
//    Encode(*in_stream, *out_stream);
//    delete in_stream;
//    delete out_stream;
//
//    // IInputStream
//    std::ifstream tmp_in_stream_decode("output.txt", std::ios::binary|std::ios::ate);
//    IInputStream* in_stream_decode = new InputStream(std::move(tmp_in_stream_decode));
//
//    // IOutputStream
//    std::ofstream tmp_out_stream_decode;
//    tmp_out_stream_decode.open("output2.txt");
////    tmp_out_stream_decode.open("output2.jpg");
//    IOutputStream* out_stream_decode = new OutputStream(std::move(tmp_out_stream_decode));
//    Decode(*in_stream_decode, *out_stream_decode);
//
//    delete in_stream_decode;
//    delete out_stream_decode;
//
    
    std::ifstream in_stream("input1.txt", std::ios::binary|std::ios::ate);
    std::vector<char> file_c = ReadAllBytes(in_stream);
    std::vector<byte> file;
    for (char c: file_c)
        file.push_back(byte(c));
    
    // encode
    IInputStream* in_stream_encode = new VectorInputStream(file);
    VectorOutputStream* out_stream_encode = new VectorOutputStream;
    Encode(*in_stream_encode, *out_stream_encode);
    auto encoded = out_stream_encode->get_file();
    
    // decode
    IInputStream* in_stream_decode = new VectorInputStream(encoded);
    VectorOutputStream* out_stream_decode = new VectorOutputStream;
    Decode(*in_stream_decode, *out_stream_decode);
    auto decoded = out_stream_decode->get_file();
    
    bool is_right = (file.size() == decoded.size());
    for (int i = 0; i < file.size(); ++i)
    {
        is_right &= (file[i] == decoded[i]);
    }
    std::cout << is_right << std::endl;
    
    
    return 0;
}
