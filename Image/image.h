/*
������ image.h �������� ����������� ������ Image, ��������� ������
� BMP �����������.
*/

#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>


#pragma pack(push, 1)
/**
 * ��������� ��� �������� ������ �� ��������� BMP �����.
 */
struct BMPFileHeader
{
    // ��� ����� (0x4D42 ��� BMP)
    unsigned short file_type{ 0x4D42 };
    // ������ ����� � ������ (bit_count * height * width + offset_bits)
    unsigned long file_size{ 0 };
    // �������������� (������ 0)
    unsigned short reserved1{ 0 };
    // �������������� (������ 0)
    unsigned short reserved2{ 0 };
    // �������� ������ � �������� �� ������ ����� � ������
    unsigned long offset_data{ 0 };
};
#pragma pack(pop)


/**
 * ��������� ��� �������� ������ �� ��������� �����������.
 */
struct BMPInfoHeader
{
    // ����� ������, ����������� ��� ���� ��������� (40)
    unsigned long size{ 0 };
    // ������ ��������� ������� � ��������
    unsigned long width{ 0 };
    // ������ ��������� ������� � ��������
    unsigned long height{ 0 };
    // ����� ���������� �������� ���������� (������ 1)
    unsigned short planes{ 1 };
    // ������� �����, ����� ��� �� ����� (0, 1, 4, 8, 16, 24, 32)
    unsigned short bit_count{ 0 };
    // ��� ������ (0 ��� ��������� �����������)
    unsigned long compression{ 0 };
    // ������ ����������� � ������ (bit_count * height * width)
    unsigned long size_image{ 0 };
    // ����������� ����������� �� �����������
    unsigned long x_px_per_meter{ 0 };
    // ����������� ����������� �� ���������
    unsigned long y_px_per_meter{ 0 };
    // ����� �������� ������������ ������ (���� ��� �����, �� 0)
    unsigned long colors_used{ 0 };
    // ����� ����������� ������ (���� ��������� ��� �����, �� 0)
    unsigned long colors_important{ 0 };
};


/**
 * ��������� ��� �������� �������� ������� �����������.
 */
struct RGBQuad
{
    unsigned char blue; // ����� ����
    unsigned char green; // ������� ����
    unsigned char red; // ������� ����
    unsigned char reserved;
};


/**
 * ��������� ��� �������� �������� ������� �����������.
 */
struct RGBTriple
{
    unsigned char blue; // ����� ����
    unsigned char green; // ������� ����
    unsigned char red; // ������� ����
};


/**
 * ����� ��� ������ � BMP �������������.
 */
class Image
{
protected:
    // �������� ��� �������� ��������� �����
    BMPFileHeader file_header;
    // �������� ��� �������� ��������� �����������
    BMPInfoHeader bmp_info_header;
    // ���������� ������ ��� �������� ������ � �������� �����������
    RGBTriple* data;

public:
    // ����������� ������ ��� ����������
    Image();
    // ����������� ������, ����������� ����������� �� BMP �����
    Image(const char*);
    // ����������� ������, ���������� �����������
    Image(Image&);
    // ����������� ������, ��������� ����� ����������� �� ����������
    Image(unsigned char, unsigned short, unsigned long,
        unsigned long);
    // ���������� ������
    ~Image();
    // ��������������� ��������� ������������
    Image operator = (Image);
    // ����� ��������� ����������� �� BMP �����
    int load_image(const char*);
    // ����� ���������� ����������� � BMP ����
    void write_image(const char*);

protected:
    // ����� ����������, �������� �� ����������� ������
    bool check_empty();
    // ����� �������� ����������� �� ������
    bool copy_image(Image&);
    // ����� ������ ������ �������� �� 24-������� BMP �����
    void read_data_24(FILE*);
    // ����� ������ ������ �������� �� 32-������� BMP �����
    void read_data_32(FILE*);
    // ����� ���������� ������ �������� � 24-������ BMP ����
    void write_data_24(FILE*);
    // ����� ���������� ������ �������� � 32-������ BMP ����
    void write_data_32(FILE*);
};


/**
 * ����������� ������ Image ��� ����������. ��������� ����� �
 * ����������� ��������� �������� �� ���������.
 */
Image::Image()
{
    // ���������� ������ � ������� ��������
    data = nullptr;
}


/**
 * ����������� ������ Image, ����������� ����������� �� BMP �����.
 * @param filename: ��� ����� � ������������.
 */
Image::Image(const char* filename)
{
    load_image(filename);
}


/**
 * ����������� ������ Image, ��������� ����� �����������.
 * @param mode: �������� ��� ������ �������;
 * @param bit_count: ������� �����������;
 * @param width: ������ �����������;
 * @param height: ������ �����������.
 */
Image::Image(unsigned char mode, unsigned short bit_count,
    unsigned long width, unsigned long height)
{
    // ��������� ��������� �����������
    bmp_info_header.size = sizeof(BMPInfoHeader);
    bmp_info_header.bit_count = bit_count; // ������� �����
    bmp_info_header.height = height; // ������
    bmp_info_header.width = width; // ������
    // ��������� ���� ��������� ���������
    file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    file_header.file_size = bit_count * height * width + 
        file_header.offset_data;
    // �������� ������ ��� ������ � ��������
    data = new RGBTriple[width * height];
    // ���������� � ������ �������� ����
    RGBTriple rgbtriple = { mode, mode, mode };
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            data[i * width + j] = rgbtriple;
        }
    }
    std::cout << "����������� �������.\n";
}


/**
 * ����������� ����� ������ Image.
 * @param image: ������� ����������� BMP, ������� ����� �����������.
 */
Image::Image(Image& image)
{
    copy_image(image);
}


/**
 * ���������� ������ Image.
 */
Image::~Image()
{
    // ������� ������ ��������
    delete[] data;
}


/**
 * ��������������� ��������� ������������ = ��� ������ Image.
 * @param image: �����������, ������� ����������.
 */
Image Image::operator = (Image image)
{
    if (check_empty())
    {
        copy_image(image);
    }
    return *this;
}


/**
 * ����� ����� Image ��� ����������� ����������� �� ������.
 * @param image: ������ �� �����������, ������� ����� �����������.
 */
bool Image::copy_image(Image& image)
{
    // �������� ��������� �����
    file_header = image.file_header;
    // �������� ��������� �����������
    bmp_info_header = image.bmp_info_header;
    // ������ � ������ ����������� �����������
    int height = image.bmp_info_header.height;
    int width = image.bmp_info_header.width;
    // �������� ������ �������� �����������
    data = new RGBTriple[width * height];
    for (unsigned int i = 0; i < image.bmp_info_header.height; i++)
    {
        for (unsigned int j = 0; j < image.bmp_info_header.width; j++)
        {
            data[i * width + j] = image.data[i * width + j];
        }
    }
    return true;
}


/**
 * ����� ������ Image ����������, �������� �� �����������-������
 * ����� ������ ������.
 * @return: true, ���� ��������, ����� false.
 */
bool Image::check_empty()
{
    if (bmp_info_header.size == 0)
    {
        return true;
    }
    return false;
}


/**
 * ����� ������ Image ��� �������� ����������� �� BMP �����.
 * @param filename: ��� ����� � ������������.
 * @returm: 0, ���� ��� ���������� ������� ��������� ������.
 */
int Image::load_image(const char* filename)
{
    // ��������� BMP ����
    FILE* file;
    fopen_s(&file, filename, "rb");
    if (!file)
    {
        // ���� ���� �� ��� ������
        std::cout << "������! �� ������� ������� ���� '" << 
            filename << "'.\n";
        return 0;
    }
    // ��������� �������� ���������
    fread(&file_header, sizeof(BMPFileHeader), 1, file);
    if (file_header.file_type != 0x4D42)
    {
        // �������� ������ � BMP �������
        fclose(file);
        std::cout << "������! ���� �� ����� ��������� BMP ������.\n";
        return 0;
    }
    // ��������� ��������� �����������
    fread(&bmp_info_header, sizeof(BMPInfoHeader), 1, file);
    if (bmp_info_header.compression != 0 ||
        (bmp_info_header.bit_count != 24 && bmp_info_header.bit_count != 32))
    {
        // �������� ������ � ��������� �������������� ������������� �
        // �������� ����� 24 ��� 32
        fclose(file);
        std::cout << "������! ����������� ������ ���� �������� " <<
            "������������� � �������� ����� 24 ��� 32 ���.\n";
        return 0;
    }
    // �������� ��������� ������� �� ���� ��������
    fseek(file, file_header.offset_data, SEEK_SET);
    // �������� ������ ��� ������ � ��������
    data = new RGBTriple[bmp_info_header.width * bmp_info_header.height];
    // ������ ������ �������� �� BMP �����
    if (bmp_info_header.bit_count == 24)
    {
        // ���� ����������� 24-������
        read_data_24(file);
    }
    else
    {
        // ���� ����������� 32-������
        read_data_32(file);
    }
    std::cout << "����������� �� BMP ����� '" << filename <<
        "' ���������.\n";
    // ������ ���������, ���� ����� �������
    fclose(file);
    return 1;
}


/**
 * ����� ������ Image ��� ������ ������� �������� ����������� ��
 * 24-������� ����� BMP.
 * @param file: �������� BMP ����.
 */
void Image::read_data_24(FILE* file)
{
    // ���� ����������� 24-������, ������ ������ ����������� �������,
    // �� ��������� 4
    int padding = bmp_info_header.width % 4;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // ���� ��� ������� �� ������� ����� ��������
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // ���� ��� ������� �� ������� �������� ��������.
            // ��������� ������ � ��������� RGBTriple
            fread(&data[i * bmp_info_header.width + j], sizeof(RGBTriple),
                1, file);
        }
        fseek(file, padding, SEEK_CUR);
    }
}


/**
 * ����� ������ Image ��� ������ ������� �������� ����������� ��
 * 32-������� ����� BMP.
 * @param file: �������� BMP ����.
 */
void Image::read_data_32(FILE* file)
{
    // ���� ����������� 32-������, ������ ������ ������ 4
    RGBQuad px;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // ���� ��� ������� �� ������� ����� ��������
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // ���� ��� ������� �� ������� �������� ��������.
            // ��������� ������ � ��������� RGBTriple
            fread(&px, sizeof(RGBQuad), 1, file);
            data[i * bmp_info_header.width + j].blue = px.blue;
            data[i * bmp_info_header.width + j].green = px.green;
            data[i * bmp_info_header.width + j].red = px.red;
        }
    }
}


/**
 * ����� ������ Image ���������� ����������� � BMP ����.
 * @param filename: ��� �����, � ������� ����� ��������� �����������.
 */
void Image::write_image(const char* filename)
{
    // ��������� ����
    FILE* file;
    fopen_s(&file, filename, "wb");
    if (!file)
    {
        // ���� ���� �� ��� ������
        std::cout << "������! �� ������� ������� ���� '" << 
            filename << "'.\n";
        return;
    }
    // ���������� �������� ���������
    fwrite(&file_header, sizeof(BMPFileHeader), 1, file);
    // ���������� ��������� �����������
    fwrite(&bmp_info_header, sizeof(BMPInfoHeader), 1, file);
    // �������� ��������� ������� �� ���� ��������
    unsigned char temp = 0;
    int count = file_header.offset_data - sizeof(BMPFileHeader) - 
        sizeof(BMPInfoHeader);
    fwrite(&temp, sizeof(unsigned char), count, file);
    // ���������� ������ �������� � BMP �����
    if (bmp_info_header.bit_count == 24)
    {
        // ���� ����������� 24-������
        write_data_24(file);
    }
    else
    {
        // ���� ����������� 32-������
        write_data_32(file);
    }
}


/**
 * ����� ������ Image ���������� ������ ��������� � 24-������ BMP ����.
 * @param file: ���� ��� ������.
 */
void Image::write_data_24(FILE* file)
{
    // ���� ����������� 24-������, ������ ������ ����������� �������,
    // �� ��������� 4
    int padding = bmp_info_header.width % 4;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // ���� ��� ������� �� ������� ����� ��������
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // ���� ��� ������� �� ������� �������� ��������.
            // ���������� ��������� RGBTriple
            fwrite(&data[i * bmp_info_header.width + j], sizeof(RGBTriple),
                1, file);
        }
        unsigned char temp = 0;
        fwrite(&temp, sizeof(unsigned char), padding, file);
    }
}


/**
 * ����� ������ Image ���������� ������ ��������� � 32-������ BMP ����.
 * @param file: ���� ��� ������.
 */
void Image::write_data_32(FILE* file)
{
    // ���� ����������� 32-������, ������ ������ ��� ������ 4
    RGBQuad px;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // ���� ��� ������� �� ������� ����� ��������
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // ���� ��� ������� �� ������� �������� ��������.
            // ���������� ��������� RGBQuad
            px.blue = data[i * bmp_info_header.width + j].blue;
            px.green = data[i * bmp_info_header.width + j].green;
            px.red = data[i * bmp_info_header.width + j].red;
            px.reserved = 0;
            fwrite(&px, sizeof(RGBQuad), 1, file);
        }
    }
}

#endif

