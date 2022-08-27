#include "ModulBase.h"

miModul::Modulresult miModul::ModulBase::AddToInputIOImage(const IOImage& image, IOImageOffset offset)
{
    _IOImageInput = image;
    _IOImageInputOffset = offset;
    return Modulresult();
}

miModul::Modulresult miModul::ModulBase::AddToOutputIOImage(const IOImage& image, IOImageOffset offset)
{
    _IOImageOutput = image;
    _IOImageOutputOffset = offset;
    return Modulresult();
}
