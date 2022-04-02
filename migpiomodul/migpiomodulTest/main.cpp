#include <cstdio>
#include <cGPIOModul.h>
#include <IOImage.h>
#include <string>
using namespace miIOImage;
using namespace miModul;

int main()
{
    miIOImage::IOImage imgin;
    miIOImage::IOImage imgout;
    miModul::cGPIOModul modul();
    std::string conf = 
    "{"
    "gpiomodul"
    "{"
    "iomaps"
        "["
            "bitsize:1,"
            "direction:in,"
            


    printf("Hallo aus %s!\n", "migpiomodulTest");
    return 0;
}