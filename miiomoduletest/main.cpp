#include <cstdio>
#include <mi/miio/IOManager.h>
#include <mi/miio/IOImage.h>
#include <mi/miio/IOModulInterface.h>
#include <mi/miio/IOModulLoader.h>
#include <mi/miio/GPIOModul.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "Test.h"

bool exit1 = false;
void signal_callback_handler(int signum) {
    
    // Terminate program
    exit1 = true;
}

int main()
{
    miIOImage::IOImage iimage(1024, miIOImage::IOImageType::Input, std::string("in"));
    miIOImage::IOImage oimage(1024, miIOImage::IOImageType::Output, std::string("out"));
   
   

    {
       // miIOManager::IOModulLoader GpioModul(std::string("libmigpiomodul"), std::string("0.0.0.2"));
        miModul::GPIOModul gpioModul;
        gpioModul.Open("");
        gpioModul.Start();
        gpioModul.ReadInputs(iimage, 0, 0);
        gpioModul.WriteOutputs(oimage, 0, 0);
        gpioModul.Stop();
        gpioModul.Close();
    }

    {
        miIOManager::IOModulLoader PcfModul(std::string("libmipcfmodul"), std::string("0.0.0.1"));
        PcfModul.Open("Address=12");
        PcfModul.Start();
        PcfModul.ReadInputs(iimage, 0, 0);
        PcfModul.WriteOutputs(oimage, 0, 0);
        PcfModul.Stop();
        PcfModul.Close();
    }

    

    while (!exit1)
    {
           
           
            
        usleep(1000 * 500);
    }
    printf("Hallo aus %s!\n", "migpiomodulTest");
    
    return 0;
}