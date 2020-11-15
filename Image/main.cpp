#include <clocale>
#include <iostream>
#include "image.h"
#include "image_advanced.h"

int main()
{
    setlocale(LC_ALL, "Rus");
    //
    const char filename[] = "1.bmp";
    Image i1(filename);
    Image i2(2, 32, 400, 300);
    i2.write_image("2.bmp");
    Image i3(i1);
    i3.write_image("3.bmp");
    Image i4;
    i4 = i2;
    i4.write_image("4.bmp");
    return 0;
}