#ifndef BMPDECODER_H
#define BMPDECODER_H
#include <Arduino.h>
class BmpDecoder
{
private:
    int _bfType;      //说明文件的类型，一般为 42 4D BM
    int _bfSize;      //该位图文件的大小，用字节为单位
    int _bfReserved1; //保留，必须设置为0
    int _bfReserved2; //保留，必须设置为0
    int _bfOffBits;   //即从文件头到位图数据需偏移****字节。
    int _biSize;
    int _biWidth;          //说明像素的宽度
    int _biHeight;         //说明像素的高度 如果要想得到一个正值，则要每位取反；
    bool _isRevOrder;      // *自添加值* 如果像素的高度是个正数，说明图像时倒向的，如果是个负数，则说明是正向的位图，大多数的bmp文件都是倒向的位图
    int _biPlanes;         //为目标设备说明颜色平面数；其值总是1；
    int _biBitCount;       //说明图片的位数；1、4、8、16、24、32位
    int _biCompression;    //说明图像数据压缩的类型，一般都为0
    int _biSizeImage;      //说明图像的大小，当biCompression为0的时候，可以设置为0；
    int _biXPelsPerMeter;  //说明水平分辨率，用像素/每米表示，有符号整数；
    int _biYPelsPerMeter;  //说明垂直分辨率，用像素/每米表示，有符号整数；
    int _biColorUsed;      //位图中实际使用的颜色表中的颜色索引数（设为0的话，说明使用所有调色板选项）
    int _biColorImportant; //说明对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要
    bool _isIndexColor;    //*自添加值* 如果biColorUsed和biColorImportant都为0，则说明不使用色板没有索引颜色
    int _skipByte;         //*自添加值* 补齐位：如果水平像素数为4的倍数，则不需要补齐位
    int readByte();
    int readWord();
    long readDword();
    File _file;
public:
    uint16_t *BitMap_buffer;
    int8_t G_biWidth;
    int8_t G_biHeight;
    BmpDecoder(File file);
    ~BmpDecoder();
};

BmpDecoder::BmpDecoder(File file)
{
    _file = file;
    _bfType = readWord();
    if (_bfType != 19778)
    {
        Serial.println(F("不是bmp文件！"));
        return;
    }
    Serial.print(F("文件类型为："));
    Serial.println(_bfType); //19778 BM
    _bfSize = readDword();
    Serial.print(F("文件大小为："));
    Serial.println(_bfSize);
    _bfReserved1 = readWord();
    Serial.print(F("保留位1："));
    Serial.println(_bfReserved1);
    _bfReserved2 = readWord();
    Serial.print(F("保留位2："));
    Serial.println(_bfReserved2);
    _bfOffBits = readDword();
    Serial.print(F("偏移量："));
    Serial.println(_bfOffBits);
    _biSize = readDword();
    Serial.print(F("头结构的字节数："));
    Serial.println(_biSize);
    _biWidth = readDword();
    G_biWidth = _biWidth;
    Serial.print(F("图像宽度："));
    Serial.println(_biWidth);
    _biHeight = readDword();
    G_biHeight = _biHeight;
    Serial.print(F("图像高度："));
    Serial.println(_biHeight);
    if (_biHeight < 0)
    {
        _isRevOrder = true;
        Serial.println(F("图像是正向序列，非正常序列，但读取数据时好用"));
    }
    else
    {
        _isRevOrder = false;
        Serial.println(F("图像是倒向序列，为正常序列"));
    }
    _biPlanes = readWord();
    Serial.print(F("颜色平面数："));
    Serial.println(_biPlanes);
    _biBitCount = readWord();
    Serial.print(F("图像位数："));
    Serial.println(_biBitCount);
    _biCompression = readDword();
    Serial.print(F("图像压缩类型："));
    Serial.println(_biCompression);
    _biSizeImage = readDword();
    Serial.print(F("图像的大小为："));
    Serial.println(_biSizeImage);
    _biXPelsPerMeter = readDword();
    Serial.print(F("图像水平分辨率为（像素/米）："));
    Serial.println(_biXPelsPerMeter);
    _biYPelsPerMeter = readDword();
    Serial.print(F("图像垂直分辨率为（像素/米）："));
    Serial.println(_biYPelsPerMeter);
    _biColorUsed = readDword();
    Serial.print(F("索引颜色数为："));
    Serial.println(_biColorUsed);
    _biColorImportant = readDword();
    Serial.print(F("重要的索引颜色数为："));
    Serial.println(_biColorImportant);
    if (_biColorUsed == 0 && _biColorImportant == 0)
    {
        _isIndexColor = false;
        Serial.println(F("图像不是索引颜色，没有色板"));
    }
    else
    {
        Serial.println(F("图像是索引色"));
    }
    _skipByte = 4 - ((_biBitCount * _biHeight * 4 / 32) % 4);
    if (_skipByte == 4)
        _skipByte = 0;
    Serial.print(F("图像每行的补齐位是："));
    Serial.println(_skipByte);
    //***************************************************************************
    BitMap_buffer = new uint16_t[_biSizeImage];
    // BitMap_buffer = (uint16_t *)malloc(_biSizeImage);
    if(BitMap_buffer!=NULL) Serial.println("BitMap_buffer 申请成功");
    if(BitMap_buffer==NULL) Serial.println("BitMap_buffer 没有申请成功");

    
    if (_biBitCount == 16) //如果是16bit的565格式位图，则必须要跳过16个字节
    {
        readDword();
        readDword();
        readDword();
        readDword();
    } 
    else if (_biBitCount == 24 ){

    }
    else if (_biBitCount == 32 ){

    }
    
    //开始解析数据部分
    if (_isRevOrder)
    {
        for (uint8_t i = 0; i < _biHeight; i++)
        {
            for (uint8_t j = 0; i < _biWidth; j++)
            {
                if (_biBitCount == 16)
                    BitMap_buffer[i * _biWidth + j] = readWord();
                else if (_biBitCount == 24)
                {
                    uint16_t B = readByte();
                    uint16_t G = readByte();
                    uint16_t R = readByte();
                    BitMap_buffer[i * _biWidth + j] = ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
                }
                else if (_biBitCount == 32)
                {
                    readByte(); 
                    uint16_t B = readByte();
                    uint16_t G = readByte();
                    uint16_t R = readByte();
                    BitMap_buffer[i * _biWidth + j] = ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
                }
                
            }
            //跳过补齐位
            if (_skipByte != 0)
            {
                for (uint8_t k = 0; k < _skipByte; k++)
                {
                    readByte();
                }
            }
        }
    }
    else
    {
        for (int i = _biHeight - 1; i >= 0; i--)
        {
            for (int j = 0; j < _biWidth; j++)
            {
                if (_biBitCount == 16)
                    BitMap_buffer[i * _biWidth + j] = readWord();
                else if (_biBitCount == 24)
                {
                    uint16_t B = readByte();
                    uint16_t G = readByte();
                    uint16_t R = readByte();
                    BitMap_buffer[i * _biWidth + j] = ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
                }
                else if (_biBitCount == 32)
                {
                    uint16_t B = readByte();
                    uint16_t G = readByte();
                    uint16_t R = readByte();
                    readByte(); 
                    BitMap_buffer[i * _biWidth + j] = ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
                }
            }
            //跳过补齐位
            if (_skipByte != 0)
            {
                for (uint8_t k = 0; k < _skipByte; k++)
                {
                    readByte();
                }
            }
        }
        
    }
    _file.close();
}

int BmpDecoder::readByte()
{
    return _file.read();
}
int BmpDecoder::readWord()
{
    int b0 = readByte();
    int b1 = readByte();
    return (b1 << 8) | b0;
}
long BmpDecoder::readDword()
{
    long b0 = readByte();
    long b1 = readByte();
    long b2 = readByte();
    long b3 = readByte();
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

BmpDecoder::~BmpDecoder()
{
    //析构函数
    delete BitMap_buffer;
}
#endif