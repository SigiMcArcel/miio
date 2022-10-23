#include <cstdio>
#include <mi/miio/IOManager.h>
#include <mi/miio/IOImage.h>
#include <mi/miio/IOModulInterface.h>
#include <mi/miio/IOModul.h>
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
   
   

    
    
    miModul::IOModul gpioModul(std::string("migpiomodul"));
        if (gpioModul.State() != miModul::IOManagerModulLoadResult::Ok)
        {
            printf("Error %d!\n", gpioModul.State());
        }
        gpioModul.Open("migpiomodul", "");
        gpioModul.Start();
        gpioModul.ReadInputs(iimage, 0, 0);
        gpioModul.WriteOutputs(oimage, 0, 0);
        gpioModul.Stop();
        gpioModul.Close();
    

    {
            miModul::IOModul PcfModul(std::string("mipcfmodul"));
        if (PcfModul.State() != miModul::IOManagerModulLoadResult::Ok)
        {
            printf("Error %d!\n", PcfModul.State());
        }
        miModul::IOModulResult result = PcfModul.Open("mipcf8574inmodul", "address=12");
        PcfModul.Start();
        PcfModul.ReadInputs(iimage, 0, 0);
        PcfModul.WriteOutputs(oimage, 0, 0);
        PcfModul.Stop();
        PcfModul.Close();
    }
    {
        miModul::IOModul PcfModu1l(std::string("mipcfmodul"));
        if (PcfModu1l.State() != miModul::IOManagerModulLoadResult::Ok)
        {
            printf("Error %d!\n", PcfModu1l.State());
        }
        miModul::IOModulResult result = PcfModu1l.Open("mipcf8574outmodul", "address=13");
        PcfModu1l.Start();
        PcfModu1l.ReadInputs(iimage, 0, 0);
        PcfModu1l.WriteOutputs(oimage, 0, 0);
        PcfModu1l.Stop();
        PcfModu1l.Close();
    }

    

    while (!exit1)
    {
           
           
            
        usleep(1000 * 500);
    }
    printf("Hallo aus %s!\n", "migpiomodulTest");
    
    return 0;
}