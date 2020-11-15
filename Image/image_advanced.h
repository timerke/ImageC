/*
Модуль image_advanced.h содержит определение класса ImageAdvanced, хранящего данные
о BMP изображении. Класс работает с палитровыми изображениями и позволяет
масштабировать изображения.
*/

#pragma once
#ifndef IMAGE_ADVANCED_H
#define IMAGE_ADVANCED_H

#include <iostream>
#include <math.h>
#include "image.h"


/**
 * Класс для работы с BMP изображениями.
 */
class ImageAdvanced: public Image
{
protected:
    RGBQuad* palette; // палитра используемых цветов

public:
    // Конструктор класса без параметров
    ImageAdvanced();
    // Конструктор класса, считывающий изображение из BMP файла
    ImageAdvanced(const char*);
    // Конструктор класса, копирующий изображение
    ImageAdvanced(ImageAdvanced&);
    // Конструктор класса, создающий новое изображение по параметрам
    ImageAdvanced(unsigned char, unsigned short, unsigned long,
        unsigned long);
    // Деструктор класса
    ~ImageAdvanced();
    // Переопределение оператора присваивания
    ImageAdvanced operator = (ImageAdvanced);
    // Метод загружает изображение из BMP файла
    int load_image(const char*);
    // Метод записывает изображение в BMP файл
    void write_image(const char*);
    
private:
    // Метод проверяет, содержит ли изображение палитру
    bool check_palette();
    // Метод копирует изображение по ссылке
    bool copy_image(ImageAdvanced&);
    // Метод читает массив пикселей из 1-битного BMP файла
    void read_data_1(FILE*);
    // Метод читает массив пикселей из 4-битного BMP файла
    void read_data_4(FILE*);
    // Метод читает массив пикселей из 8-битного BMP файла
    void read_data_8(FILE*);
    // Метод записывает массив пикселей в 1-битный BMP файл
    void write_data_1(FILE*);
    // Метод записывает массив пикселей в 4-битный BMP файл
    void write_data_4(FILE*);
    // Метод записывает массив пикселей в 8-битный BMP файл
    void write_data_8(FILE*);
};


/**
 * Конструктор класса ImageAdvanced без параметров. Заголовки файла и
 * изображения принимают значения по умолчанию.
 */
ImageAdvanced::ImageAdvanced() : Image()
{
}


/**
 * Конструктор класса ImageAdvanced, считывающий изображение из
 * BMP файла.
 * @param filename: имя файла с изображением.
 */
ImageAdvanced::ImageAdvanced(const char* filename)
{
    load_image(filename);
}


/**
 * Конструктор класса ImageAdvanced, создающий новое изображение.
 * @param mode: значение для цветов пикселя;
 * @param bit_count: глубина изображения;
 * @param width: ширина изображения;
 * @param height: высота изображения.
 */
ImageAdvanced::ImageAdvanced(unsigned char mode, unsigned short bit_count,
    unsigned long width, unsigned long height) : 
    Image(mode, bit_count, width, height)
{
    if (!check_palette())
    {
        // Если изображение не содержит палитру, создание
        // изображения закончено
        return;
    }
    // Если изображение содержит палитру.
    // Количество цветов в палитре
    unsigned char colors_num = (char)pow(2, bit_count);
    // Меняем некоторые поля в заголовках файла и изображения
    bmp_info_header.colors_used = colors_num;
    file_header.offset_data = sizeof(BMPFileHeader) + 
        sizeof(BMPInfoHeader) + colors_num * sizeof(RGBQuad);
    file_header.file_size = bit_count * height * width + 
        file_header.offset_data;
    // Создаем палитру цветов
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
 * Конструктор копий класса ImageAdvanced.
 * @param image: готовое изображение BMP, которуе будет скопировано.
 */
ImageAdvanced::ImageAdvanced(ImageAdvanced& image)
{
    copy_image(image);
}


/**
 * Деструктор класса Image.
 */
ImageAdvanced::~ImageAdvanced()
{
    // Удаляем палитру
    delete[] palette;
}


/**
 * Переопределение оператора присваивания = для класса ImageAdvanced.
 * @param image: изображение, которое копируется.
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
 * Метод класса ImageAdvanced проверяет, содержит ли изображение
 * палитру.
 * @return: true, если содержит, иначе false.
 */
bool ImageAdvanced::check_palette()
{
    if (bmp_info_header.bit_count == 1 || bmp_info_header.bit_count == 4 ||
        bmp_info_header.bit_count == 8)
    {
        // Изображение палитровое
        return true;
    }
    return false;
}


/**
 * Метод класа ImageAdvanced для копирования изображения по ссылке.
 * @param image: ссылка на изображение, которое нужно скопировать.
 */
bool ImageAdvanced::copy_image(ImageAdvanced& image)
{
    // Копируем заголовок файла
    file_header = image.file_header;
    // Копируем заголовок изображения
    bmp_info_header = image.bmp_info_header;
    // Высота и ширина копируемого изображения
    int height = image.bmp_info_header.height;
    int width = image.bmp_info_header.width;
    // Копируем массив пикселей изображения
    data = new RGBTriple[width * height];
    for (unsigned int i = 0; i < image.bmp_info_header.height; i++)
    {
        for (unsigned int j = 0; j < image.bmp_info_header.width; j++)
        {
            data[i * width + j] = image.data[i * width + j];
        }
    }
    // Если изображение палитровое, копируем палитру
    if (check_palette())
    {
        // Копируем палитру
        palette = new RGBQuad[bmp_info_header.colors_used];
        for (unsigned short i = 0; i < bmp_info_header.colors_used; i++)
        {
            palette[i] = image.palette[i];
        }
    }
    return true;
}


/**
 * Метод класса ImageAdvanced для загрузки изображения из BMP файла.
 * @param filename: имя файла с изображением.
 * @returm: 0, если при выполнении функции произошла ошибка.
 */
int ImageAdvanced::load_image(const char* filename)
{
    // Открываем BMP файл
    FILE* file;
    fopen_s(&file, filename, "rb");
    if (!file)
    {
        // Если файл не был открыт
        std::cout << "Ошибка! Не удалось открыть файл '" <<
            filename << "'.\n";
        return 0;
    }
    // Считываем файловый заголовок
    fread(&file_header, sizeof(BMPFileHeader), 1, file);
    if (file_header.file_type != 0x4D42)
    {
        // Работаем только с BMP файлами
        fclose(file);
        std::cout << "Ошибка! Файл не имеет требуемый BMP формат.\n";
        return 0;
    }
    // Считываем заголовок изображения
    fread(&bmp_info_header, sizeof(BMPInfoHeader), 1, file);
    if (bmp_info_header.compression != 0)
    {
        // Работаем только с несжатыми изображениями
        fclose(file);
        std::cout << "Ошибка! Изображение должно быть несжатым.\n";
        return 0;
    }
    // Если изображение палитровое, считываем палитру
    if (check_palette())
    {
        // Считываем палитру
        fread(&palette, sizeof(RGBQuad), bmp_info_header.colors_used, 
            file);
    }
    // Сдвигаем положение курсора на поле пикселей
    fseek(file, file_header.offset_data, SEEK_SET);
    // Выделяем память для данных о пикселях
    data = new RGBTriple[bmp_info_header.width * bmp_info_header.height];
    // Читаем массив пикселей из BMP файла
    if (bmp_info_header.bit_count == 1)
    {
        // Если изображение 1-битное
        read_data_1(file);
    }
    else if (bmp_info_header.bit_count == 4)
    {
        // Если изображение 4-битное
        read_data_4(file);
    }
    else if (bmp_info_header.bit_count == 8)
    {
        // Если изображение 8-битное
        read_data_8(file);
    }
    else if (bmp_info_header.bit_count == 24)
    {
        // Если изображение 24-битное
        read_data_24(file);
    }
    else if (bmp_info_header.bit_count == 32)
    {
        // Если изображение 32-битное
        read_data_32(file);
    }
    std::cout << "Изображение из BMP файла '" << filename <<
        "' прочитано.\n";
    // Данные прочитаны, файл можно закрыть
    fclose(file);
    return 1;
}


/**
 * Метод класса Image для чтения массива пикселей изображения из
 * 1-битного файла BMP.
 * @param file: открытый BMP файл.
 */
void ImageAdvanced::read_data_1(FILE* file)
{
    // Если изображение 1-битное, каждая строка дополняется байтами,
    // до кратности 4
    int padding = (3 * int(ceil(bmp_info_header.width / 8.))) % 4;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // Цикл для прохода по номерам строк пикселей.
        // Счетчик количества прочитанных пикселей в строке
        unsigned int j = 0;
        while (j < bmp_info_header.width)
        {
            // Цикл для прохода по номерам столбцов пикселей.
            // Считываем байт
            unsigned char byte;
            fread(&byte, sizeof(unsigned char), 1, file);
            // Маска для выделения всех отдельных бит в прочитанном байте
            unsigned char mask = 0x80;
            for (int shift = 7; shift >= 0; shift--)
            {
                // Получаем бит под номером 7-shift слева направо
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
                    // Прочли нужное количество бит в строке
                    break;
                }
                mask >>= 1; // сдвигаем маску на 1 бит вправо
            }
        }
        fseek(file, padding, SEEK_CUR);
    }
}


/**
 * Метод класса Image для чтения массива пикселей изображения из
 * 4-битного файла BMP.
 * @param file: открытый BMP файл.
 */
void ImageAdvanced::read_data_4(FILE* file)
{
    // Если изображение 4-битное, каждая строка дополняется байтами,
    // до кратности 4
    int padding = (3 * int(ceil(bmp_info_header.width / 2.))) % 4;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // Цикл для прохода по номерам строк пикселей.
        // Счетчик количества прочитанных пикселей в строке
        unsigned int j = 0;
        while (j < bmp_info_header.width)
        {
            // Цикл для прохода по номерам столбцов пикселей.
            // Считываем байт
            unsigned char byte;
            fread(&byte, sizeof(unsigned char), 1, file);
            // Маска для выделения половины бит в прочитанном байте
            unsigned char mask = 0xF0;
            for (int shift = 1; shift >= 0; shift--)
            {
                // Получаем бит под номером 1-shift слева направо
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
                    // Прочли нужное количество полубит в строке
                    break;
                }
                mask >>= 4; // сдвигаем маску на 4 бит вправо
            }
        }
        fseek(file, padding, SEEK_CUR);
    }
}


/**
 * Метод класса Image для чтения массива пикселей изображения из
 * 8-битного файла BMP.
 * @param file: открытый BMP файл.
 */
void ImageAdvanced::read_data_8(FILE* file)
{
    // Если изображение 8-битное, каждая строка дополняется
    // байтами до кратности 4
    int padding = (3 * bmp_info_header.width) % 4;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // Цикл для прохода по номерам строк пикселей
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // Цикл для прохода по номерам столбцов пикселей.
            // Считываем байт, который является индексом цвета
            unsigned char byte;
            fread(&byte, sizeof(unsigned char), 1, file);
            data[i * bmp_info_header.width + j].blue = palette[byte].blue;
            data[i * bmp_info_header.width + j].green = palette[byte].green;
            data[i * bmp_info_header.width + j].red = palette[byte].red;
        }
    }
}


/**
 * Метод класса Image записывает изображение в BMP файл.
 * @param filename: имя файла, в который нужно сохранить изображение.
 */
void ImageAdvanced::write_image(const char* filename)
{
    // Открываем файл
    FILE* file;
    fopen_s(&file, filename, "wb");
    if (!file)
    {
        // Если файл не был открыт
        std::cout << "Ошибка! Не удалось открыть файл '" << filename << "'.\n";
        return;
    }
    // Записываем файловый заголовок
    fwrite(&file_header, sizeof(BMPFileHeader), 1, file);
    // Записываем заголовок изображения
    fwrite(&bmp_info_header, sizeof(BMPInfoHeader), 1, file);
    // Сдвигаем положение курсора на поле пикселей
    unsigned char temp = 0;
    int count = file_header.offset_data - sizeof(BMPFileHeader) - sizeof(BMPInfoHeader);
    fwrite(&temp, sizeof(unsigned char), count, file);
    // Записываем массив пикселей в BMP файла
    if (bmp_info_header.bit_count == 24)
    {
        // Если изображение 24-битное
        write_data_24(file);
    }
    else
    {
        // Если изображение 32-битное
        write_data_32(file);
    }
}


/**
 * Метод класса Image записывает массив пиксейлей в 24-битный BMP файл.
 * @param file: файл для записи.
 */
void ImageAdvanced::write_data_1(FILE* file)
{
    // Если изображение 24-битное, каждая строка дополняется байтами,
    // до кратности 4
    int padding = bmp_info_header.width % 4;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // Цикл для прохода по номерам строк пикселей
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // Цикл для прохода по номерам столбцов пикселей.
            // Записываем структуру RGBTriple
            fwrite(&data[i * bmp_info_header.width + j], sizeof(RGBTriple),
                1, file);
        }
        unsigned char temp = 0;
        fwrite(&temp, sizeof(unsigned char), padding, file);
    }
}


/**
 * Метод класса Image записывает массив пиксейлей в 32-битный BMP файл.
 * @param file: файл для записи.
 */
void ImageAdvanced::write_data_4(FILE* file)
{
    // Если изображение 32-битное, каждая строка уже кратна 4
    RGBQuad px_data;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // Цикл для прохода по номерам строк пикселей
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // Цикл для прохода по номерам столбцов пикселей.
            // Записываем структуру RGBQuad
            px_data.blue = data[i * bmp_info_header.width + j].blue;
            px_data.green = data[i * bmp_info_header.width + j].green;
            px_data.red = data[i * bmp_info_header.width + j].red;
            px_data.reserved = 0;
            fwrite(&px_data, sizeof(RGBQuad), 1, file);
        }
    }
}

#endif

