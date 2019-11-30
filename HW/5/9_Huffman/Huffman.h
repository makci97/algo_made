#ifndef ALGO_MADE_HUFFMAN_H
#define ALGO_MADE_HUFFMAN_H

typedef unsigned char t_byte;

struct IInputStream
{
    // Возвращает false, если поток закончился
    virtual bool Read(t_byte& value) = 0;
    
    virtual ~IInputStream() = default;
};

struct IOutputStream
{
    virtual void Write(t_byte value) = 0;
    
    virtual ~IOutputStream() = default;
};

// Метод архивирует данные из потока original
void Encode(IInputStream& original, IOutputStream& compressed);
// Метод восстанавливает оригинальные данные
void Decode(IInputStream& compressed, IOutputStream& original);

#endif //ALGO_MADE_HUFFMAN_H
