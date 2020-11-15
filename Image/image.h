/*
Модуль image.h содержит определение класса Image, хранящего данные
о BMP изображении.
*/

#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>


#pragma pack(push, 1)
/**
 * Структура для хранения данных из заголовка BMP файла.
 */
struct BMPFileHeader
{
    // Тип файла (0x4D42 для BMP)
    unsigned short file_type{ 0x4D42 };
    // Размер файла в байтах (bit_count * height * width + offset_bits)
    unsigned long file_size{ 0 };
    // Зарезервирован (всегда 0)
    unsigned short reserved1{ 0 };
    // Зарезервирован (всегда 0)
    unsigned short reserved2{ 0 };
    // Смещение данных о пикселях от начала файла в байтах
    unsigned long offset_data{ 0 };
};
#pragma pack(pop)


/**
 * Структура для хранения данных из заголовка изображения.
 */
struct BMPInfoHeader
{
    // Число байтов, необходимое для этой структуры (40)
    unsigned long size{ 0 };
    // Ширина точечного рисунка в пикселях
    unsigned long width{ 0 };
    // Высота точечного рисунка в пикселях
    unsigned long height{ 0 };
    // Число плоскостей целевого устройства (всегда 1)
    unsigned short planes{ 1 };
    // Глубина цвета, число бит на точку (0, 1, 4, 8, 16, 24, 32)
    unsigned short bit_count{ 0 };
    // Тип сжатия (0 для несжатого изображения)
    unsigned long compression{ 0 };
    // Размер изображения в байтах (bit_count * height * width)
    unsigned long size_image{ 0 };
    // Разрешающая способность по горизонтали
    unsigned long x_px_per_meter{ 0 };
    // Разрешающая способность по вертикали
    unsigned long y_px_per_meter{ 0 };
    // Число индексов используемых цветов (если все цвета, то 0)
    unsigned long colors_used{ 0 };
    // Число необходимых цветов (если требуются все цвета, то 0)
    unsigned long colors_important{ 0 };
};


/**
 * Структура для цветовых значений пикселя изображения.
 */
struct RGBQuad
{
    unsigned char blue; // синий цвет
    unsigned char green; // зеленый цвет
    unsigned char red; // красный цвет
    unsigned char reserved;
};


/**
 * Структура для цветовых значений пикселя изображения.
 */
struct RGBTriple
{
    unsigned char blue; // синий цвет
    unsigned char green; // зеленый цвет
    unsigned char red; // красный цвет
};


/**
 * Класс для работы с BMP изображениями.
 */
class Image
{
protected:
    // Параметр для хранения заголовка файла
    BMPFileHeader file_header;
    // Параметр для хранения заголовка изображения
    BMPInfoHeader bmp_info_header;
    // Одномерный массив для хранения цветов о пикселях изображения
    RGBTriple* data;

public:
    // Конструктор класса без параметров
    Image();
    // Конструктор класса, считывающий изображение из BMP файла
    Image(const char*);
    // Конструктор класса, копирующий изображение
    Image(Image&);
    // Конструктор класса, создающий новое изображение по параметрам
    Image(unsigned char, unsigned short, unsigned long,
        unsigned long);
    // Деструктор класса
    ~Image();
    // Переопределение оператора присваивания
    Image operator = (Image);
    // Метод загружает изображение из BMP файла
    int load_image(const char*);
    // Метод записывает изображение в BMP файл
    void write_image(const char*);

protected:
    // Метод определяет, является ли изображение пустым
    bool check_empty();
    // Метод копирует изображение по ссылке
    bool copy_image(Image&);
    // Метод читает массив пикселей из 24-битного BMP файла
    void read_data_24(FILE*);
    // Метод читает массив пикселей из 32-битного BMP файла
    void read_data_32(FILE*);
    // Метод записывает массив пикселей в 24-битный BMP файл
    void write_data_24(FILE*);
    // Метод записывает массив пикселей в 32-битный BMP файл
    void write_data_32(FILE*);
};


/**
 * Конструктор класса Image без параметров. Заголовки файла и
 * изображения принимают значения по умолчанию.
 */
Image::Image()
{
    // Одномерный массив с данными пикселей
    data = nullptr;
}


/**
 * Конструктор класса Image, считывающий изображение из BMP файла.
 * @param filename: имя файла с изображением.
 */
Image::Image(const char* filename)
{
    load_image(filename);
}


/**
 * Конструктор класса Image, создающий новое изображение.
 * @param mode: значение для цветов пикселя;
 * @param bit_count: глубина изображения;
 * @param width: ширина изображения;
 * @param height: высота изображения.
 */
Image::Image(unsigned char mode, unsigned short bit_count,
    unsigned long width, unsigned long height)
{
    // Заполняем заголовок изображения
    bmp_info_header.size = sizeof(BMPInfoHeader);
    bmp_info_header.bit_count = bit_count; // глубина цвета
    bmp_info_header.height = height; // высота
    bmp_info_header.width = width; // ширина
    // Заполняем поля файлового заголовка
    file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    file_header.file_size = bit_count * height * width + 
        file_header.offset_data;
    // Выделяем память для данных о пикселях
    data = new RGBTriple[width * height];
    // Записываем в массив заданный цвет
    RGBTriple rgbtriple = { mode, mode, mode };
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            data[i * width + j] = rgbtriple;
        }
    }
    std::cout << "Изображение создано.\n";
}


/**
 * Конструктор копий класса Image.
 * @param image: готовое изображение BMP, которуе будет скопировано.
 */
Image::Image(Image& image)
{
    copy_image(image);
}


/**
 * Деструктор класса Image.
 */
Image::~Image()
{
    // Удаляем массив пикселей
    delete[] data;
}


/**
 * Переопределение оператора присваивания = для класса Image.
 * @param image: изображение, которое копируется.
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
 * Метод класа Image для копирования изображения по ссылке.
 * @param image: ссылка на изображение, которое нужно скопировать.
 */
bool Image::copy_image(Image& image)
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
    return true;
}


/**
 * Метод класса Image определяет, является ли изображение-объект
 * этого класса пустым.
 * @return: true, если является, иначе false.
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
 * Метод класса Image для загрузки изображения из BMP файла.
 * @param filename: имя файла с изображением.
 * @returm: 0, если при выполнении функции произошла ошибка.
 */
int Image::load_image(const char* filename)
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
    if (bmp_info_header.compression != 0 ||
        (bmp_info_header.bit_count != 24 && bmp_info_header.bit_count != 32))
    {
        // Работаем только с несжатыми беспалитровыми изображениями с
        // грубиной цвета 24 или 32
        fclose(file);
        std::cout << "Ошибка! Изображение должно быть несжатым " <<
            "беспалитровым с глубиной цвета 24 или 32 бит.\n";
        return 0;
    }
    // Сдвигаем положение курсора на поле пикселей
    fseek(file, file_header.offset_data, SEEK_SET);
    // Выделяем память для данных о пикселях
    data = new RGBTriple[bmp_info_header.width * bmp_info_header.height];
    // Читаем массив пикселей из BMP файла
    if (bmp_info_header.bit_count == 24)
    {
        // Если изображение 24-битное
        read_data_24(file);
    }
    else
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
 * 24-битного файла BMP.
 * @param file: открытый BMP файл.
 */
void Image::read_data_24(FILE* file)
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
            // Считываем данные в структуру RGBTriple
            fread(&data[i * bmp_info_header.width + j], sizeof(RGBTriple),
                1, file);
        }
        fseek(file, padding, SEEK_CUR);
    }
}


/**
 * Метод класса Image для чтения массива пикселей изображения из
 * 32-битного файла BMP.
 * @param file: открытый BMP файл.
 */
void Image::read_data_32(FILE* file)
{
    // Если изображение 32-битное, каждая строка кратна 4
    RGBQuad px;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // Цикл для прохода по номерам строк пикселей
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // Цикл для прохода по номерам столбцов пикселей.
            // Считываем данные в структуру RGBTriple
            fread(&px, sizeof(RGBQuad), 1, file);
            data[i * bmp_info_header.width + j].blue = px.blue;
            data[i * bmp_info_header.width + j].green = px.green;
            data[i * bmp_info_header.width + j].red = px.red;
        }
    }
}


/**
 * Метод класса Image записывает изображение в BMP файл.
 * @param filename: имя файла, в который нужно сохранить изображение.
 */
void Image::write_image(const char* filename)
{
    // Открываем файл
    FILE* file;
    fopen_s(&file, filename, "wb");
    if (!file)
    {
        // Если файл не был открыт
        std::cout << "Ошибка! Не удалось открыть файл '" << 
            filename << "'.\n";
        return;
    }
    // Записываем файловый заголовок
    fwrite(&file_header, sizeof(BMPFileHeader), 1, file);
    // Записываем заголовок изображения
    fwrite(&bmp_info_header, sizeof(BMPInfoHeader), 1, file);
    // Сдвигаем положение курсора на поле пикселей
    unsigned char temp = 0;
    int count = file_header.offset_data - sizeof(BMPFileHeader) - 
        sizeof(BMPInfoHeader);
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
void Image::write_data_24(FILE* file)
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
void Image::write_data_32(FILE* file)
{
    // Если изображение 32-битное, каждая строка уже кратна 4
    RGBQuad px;
    for (unsigned int i = 0; i < bmp_info_header.height; i++)
    {
        // Цикл для прохода по номерам строк пикселей
        for (unsigned int j = 0; j < bmp_info_header.width; j++)
        {
            // Цикл для прохода по номерам столбцов пикселей.
            // Записываем структуру RGBQuad
            px.blue = data[i * bmp_info_header.width + j].blue;
            px.green = data[i * bmp_info_header.width + j].green;
            px.red = data[i * bmp_info_header.width + j].red;
            px.reserved = 0;
            fwrite(&px, sizeof(RGBQuad), 1, file);
        }
    }
}

#endif

