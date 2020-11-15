/*
������ image_advanced.h �������� ����������� ������ ImageAdvanced, ��������� ������
� BMP �����������. ����� �������� � ����������� ������������� � ���������
�������������� �����������.
*/

#pragma once
#ifndef IMAGE_ADVANCED_H
#define IMAGE_ADVANCED_H

#include <iostream>
#include <math.h>
#include "image.h"


/**
 * ����� ��� ������ � BMP �������������.
 */
class ImageAdvanced: public Image
{
protected:
    RGBQuad* palette; // ������� ������������ ������

public:
    // ����������� ������ ��� ����������
    ImageAdvanced();
    // ����������� ������, ����������� ����������� �� BMP �����
    ImageAdvanced(const char*);
    // ����������� ������, ���������� �����������
    ImageAdvanced(ImageAdvanced&);
    // ����������� ������, ��������� ����� ����������� �� ����������
    ImageAdvanced(unsigned char, unsigned short, unsigned long,
        unsigned long);
    // ���������� ������
    ~ImageAdvanced();
    // ��������������� ��������� ������������
    ImageAdvanced operator = (ImageAdvanced);
    // ����� ��������� ����������� �� BMP �����
    int load_image(const char*);
    // ����� ���������� ����������� � BMP ����
    void write_image(const char*);
    
private:
    // ����� ���������, �������� �� ����������� �������
    bool check_palette();
    // ����� �������� ����������� �� ������
    bool copy_image(ImageAdvanced&);
    // ����� ������ ������ �������� �� 1-������� BMP �����
    void read_data_1(FILE*);
    // ����� ������ ������ �������� �� 4-������� BMP �����
    void read_data_4(FILE*);
    // ����� ������ ������ �������� �� 8-������� BMP �����
    void read_data_8(FILE*);
    // ����� ���������� ������ �������� � 1-������ BMP ����
    void write_data_1(FILE*);
    // ����� ���������� ������ �������� � 4-������ BMP ����
    void write_data_4(FILE*);
    // ����� ���������� ������ �������� � 8-������ BMP ����
    void write_data_8(FILE*);
};


/**
 * ����������� ������ ImageAdvanced ��� ����������. ��������� ����� �
 * ����������� ��������� �������� �� ���������.
 */
ImageAdvanced::ImageAdvanced() : Image()
{
}


/**
 * ����������� ������ ImageAdvanced, ����������� ����������� ��
 * BMP �����.
 * @param filename: ��� ����� � ������������.
 */
ImageAdvanced::ImageAdvanced(const char* filename)
{
    load_image(filename);
}


/**
 * ����������� ������ ImageAdvanced, ��������� ����� �����������.
 * @param mode: �������� ��� ������ �������;
 * @param bit_count: ������� �����������;
 * @param width: ������ �����������;
 * @param height: ������ �����������.
 */
ImageAdvanced::ImageAdvanced(unsigned char mode, unsigned short bit_count,
    unsigned long width, unsigned long height) : 
    Image(mode, bit_count, width, height)
{
    if (!check_palette())
    {
        // ���� ����������� �� �������� �������, ��������
        // ����������� ���������
        return;
    }
    // ���� ����������� �������� �������.
    // ���������� ������ � �������
    unsigned char colors_num = (char)pow(2, bit_count);
    // ������ ��������� ���� � ���������� ����� � �����������
    bmp_info_header.colors_used = colors_num;
    file_header.offset_data = sizeof(BMPFileHeader) + 
        sizeof(BMPInfoHeader) + colors_num * sizeof(RGBQuad);
    file_header.file_size = bit_count * height * width + 
        file_header.offset_data;
    // ������� ������� ������
    palette = new RGBQuad[colors_num];
    palette[0] = { 0, 0, 0, 0 };
    palette[1].blue = palette[1].green = palette[1].red =
        palette[0].blue + (char)pow(2, 8 - bit_count) - 1;
    palette[1].reserved = 0;
    for (unsigned char i = 2; i < colors_num; i++)
    {
        palette[i].blue = palette[i].green = palette[i].red =
            palette[i - 1].blue + (char)pow(2, 8 - bit_count);
        palette[i].reserved = 0;
    }
}


/**
 * ����������� ����� ������ ImageAdvanced.
 * @param image: ������� ����������� BMP, ������� ����� �����������.
 */
ImageAdvanced::ImageAdvanced(ImageAdvanced& image)
{
    copy_image(image);
}


/**
 * ���������� ������ Image.
 */
ImageAdvanced::~ImageAdvanced()
{
    // ������� �������
    delete[] palette;
}


/**
 * ��������������� ��������� ������������ = ��� ������ ImageAdvanced.
 * @param image: �����������, ������� ����������.
 */
ImageAdvanced ImageAdvanced::operator = (ImageAdvanced image)
{
    if (check_empty())
    {
        copy_image(image);
    }
    return *this;
}


/**
 * ����� ������ ImageAdvanced ���������, �������� �� �����������
 * �������.
 * @return: true, ���� ��������, ����� false.
 */
bool ImageAdvanced::check_palette()
{
    if (bmp_info_header.bit_count == 1 || bmp_info_header.bit_count == 4 ||
        bmp_info_header.bit_count == 8)
    {
        // ����������� ����������
        return true;
    }
    return false;
}


/**
 * ����� ����� ImageAdvanced ��� ����������� ����������� �� ������.
 * @param image: ������ �� �����������, ������� ����� �����������.
 */
bool ImageAdvanced::copy_image(ImageAdvanced& image)
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
    // ���� ����������� ����������, �������� �������
    if (check_palette())
    {
        // �������� �������
        palette = new RGBQuad[bmp_info_header.colors_used];
        for (unsigned short i = 0; i < bmp_info_header.colors_used; i++)
        {
            palette[i] = image.palette[i];
        }
    }
    return true;
}


/**
 * ����� ������ ImageAdvanced ��� �������� ����������� �� BMP �����.
 * @param filename: ��� ����� � ������������.
 * @returm: 0, ���� ��� ���������� ������� ��������� ������.
 */
int ImageAdvanced::load_image(const char* filename)
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
    if (bmp_info_header.compression != 0)
    {
        // �������� ������ � ��������� �������������
        fclose(file);
        std::cout << "������! ����������� ������ ���� ��������.\n";
        return 0;
    }
    // ���� ����������� ����������, ��������� �������
    if (check_palette())
    {
        // ��������� �������
        fread(&palette, sizeof(RGBQuad), bmp_info_header.colors_used, 
            file);
    }
    // �������� ��������� ������� �� ���� ��������
    fseek(file, file_header.offset_data, SEEK_SET);
    // �������� ������ ��� ������ � ��������
    data = new RGBTriple[bmp_info_header.width * bmp_info_header.height];
    // ������ ������ �������� �� BMP �����
    if (bmp_info_header.bit_count == 1)
    {
        // ���� ����������� 1-������
        read_data_1(file);
    }
    else if (bmp_info_header.bit_count == 4)
    {
        // ���� ����������� 4-������
        read_data_4(file);
    }
    else if (bmp_info_header.bit_count == 8)
    {
        // ���� ����������� 8-������
        read_data_8(file);
    }
    else if (bmp_info_header.bit_count == 24)
    {
        // ���� ����������� 24-������
        read_data_24(file);
    }
    else if (bmp_info_header.bit_count == 32)
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
 * 1-������� ����� BMP.
 * @param file: �������� BMP ����.
 */
void ImageAdvanced::read_data_1(FILE* file)
{
    // ���� ����������� 1-������, ������ ������ ����������� �������,
    // �� ��������� 4
    int padding = (3 * int(ceil(bmp_info_header.width / 8.))) % 4;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // ���� ��� ������� �� ������� ����� ��������.
        // ������� ���������� ����������� �������� � ������
        unsigned int j = 0;
        while (j < bmp_info_header.width)
        {
            // ���� ��� ������� �� ������� �������� ��������.
            // ��������� ����
            unsigned char byte;
            fread(&byte, sizeof(unsigned char), 1, file);
            // ����� ��� ��������� ���� ��������� ��� � ����������� �����
            unsigned char mask = 0x80;
            for (int shift = 7; shift >= 0; shift--)
            {
                // �������� ��� ��� ������� 7-shift ����� �������
                unsigned char color_index = (mask & byte)>>shift;
                if (j < bmp_info_header.width)
                {
                    data[i * bmp_info_header.width + j].blue = 
                        palette[color_index].blue;
                    data[i * bmp_info_header.width + j].green =
                        palette[color_index].green;
                    data[i * bmp_info_header.width + j].red =
                        palette[color_index].red;
                    j++;
                }
                else
                {
                    // ������ ������ ���������� ��� � ������
                    break;
                }
                mask >>= 1; // �������� ����� �� 1 ��� ������
            }
        }
        fseek(file, padding, SEEK_CUR);
    }
}


/**
 * ����� ������ Image ��� ������ ������� �������� ����������� ��
 * 4-������� ����� BMP.
 * @param file: �������� BMP ����.
 */
void ImageAdvanced::read_data_4(FILE* file)
{
    // ���� ����������� 4-������, ������ ������ ����������� �������,
    // �� ��������� 4
    int padding = (3 * int(ceil(bmp_info_header.width / 2.))) % 4;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // ���� ��� ������� �� ������� ����� ��������.
        // ������� ���������� ����������� �������� � ������
        unsigned int j = 0;
        while (j < bmp_info_header.width)
        {
            // ���� ��� ������� �� ������� �������� ��������.
            // ��������� ����
            unsigned char byte;
            fread(&byte, sizeof(unsigned char), 1, file);
            // ����� ��� ��������� �������� ��� � ����������� �����
            unsigned char mask = 0xF0;
            for (int shift = 1; shift >= 0; shift--)
            {
                // �������� ��� ��� ������� 1-shift ����� �������
                unsigned char color_index = (mask & byte) >> (shift * 4);
                if (j < bmp_info_header.width)
                {
                    data[i * bmp_info_header.width + j].blue =
                        palette[color_index].blue;
                    data[i * bmp_info_header.width + j].green =
                        palette[color_index].green;
                    data[i * bmp_info_header.width + j].red =
                        palette[color_index].red;
                    j++;
                }
                else
                {
                    // ������ ������ ���������� ������� � ������
                    break;
                }
                mask >>= 4; // �������� ����� �� 4 ��� ������
            }
        }
        fseek(file, padding, SEEK_CUR);
    }
}


/**
 * ����� ������ Image ��� ������ ������� �������� ����������� ��
 * 8-������� ����� BMP.
 * @param file: �������� BMP ����.
 */
void ImageAdvanced::read_data_8(FILE* file)
{
    // ���� ����������� 8-������, ������ ������ �����������
    // ������� �� ��������� 4
    int padding = (3 * bmp_info_header.width) % 4;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // ���� ��� ������� �� ������� ����� ��������
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // ���� ��� ������� �� ������� �������� ��������.
            // ��������� ����, ������� �������� �������� �����
            unsigned char byte;
            fread(&byte, sizeof(unsigned char), 1, file);
            data[i * bmp_info_header.width + j].blue = palette[byte].blue;
            data[i * bmp_info_header.width + j].green = palette[byte].green;
            data[i * bmp_info_header.width + j].red = palette[byte].red;
        }
    }
}


/**
 * ����� ������ Image ���������� ����������� � BMP ����.
 * @param filename: ��� �����, � ������� ����� ��������� �����������.
 */
void ImageAdvanced::write_image(const char* filename)
{
    // ��������� ����
    FILE* file;
    fopen_s(&file, filename, "wb");
    if (!file)
    {
        // ���� ���� �� ��� ������
        std::cout << "������! �� ������� ������� ���� '" << filename << "'.\n";
        return;
    }
    // ���������� �������� ���������
    fwrite(&file_header, sizeof(BMPFileHeader), 1, file);
    // ���������� ��������� �����������
    fwrite(&bmp_info_header, sizeof(BMPInfoHeader), 1, file);
    // �������� ��������� ������� �� ���� ��������
    unsigned char temp = 0;
    int count = file_header.offset_data - sizeof(BMPFileHeader) - sizeof(BMPInfoHeader);
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
void ImageAdvanced::write_data_1(FILE* file)
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
void ImageAdvanced::write_data_4(FILE* file)
{
    // ���� ����������� 32-������, ������ ������ ��� ������ 4
    RGBQuad px_data;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // ���� ��� ������� �� ������� ����� ��������
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // ���� ��� ������� �� ������� �������� ��������.
            // ���������� ��������� RGBQuad
            px_data.blue = data[i * bmp_info_header.width + j].blue;
            px_data.green = data[i * bmp_info_header.width + j].green;
            px_data.red = data[i * bmp_info_header.width + j].red;
            px_data.reserved = 0;
            fwrite(&px_data, sizeof(RGBQuad), 1, file);
        }
    }
}

#endif

