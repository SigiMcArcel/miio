#include <cstdio>
#include <mi/miio/IOManager.h>
#include <mi/miio/IOImage.h>
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

bool exit1 = false;
void signal_callback_handler(int signum) {
    
    // Terminate program
    exit1 = true;
}

int main()
{
    miIOManager::IOManagerResult mResult = miIOManager::IOManagerResult::Ok;
    miIOImage::IOImage image1;
    {
        miIOImage::IOImage image(1024, miIOImage::IOImageType::Input,"image");
        signal(SIGINT, signal_callback_handler);
        std::ifstream inFile;
        inFile.open("/home/root/mitestmodul.json"); //open the input file


        image[0] = 1;
        uint8_t u = image[0];


        std::stringstream strStream;
        strStream << inFile.rdbuf(); //read the file
        std::string str = strStream.str(); //str holds the content of the file
        miIOManager::IOManager manager(2048, 2048);
        miIOImage::IOImage inputImage = manager.InputImage();
        miIOImage::IOImage outputImage = manager.InputImage();


        mResult = manager.AddIOModul(str);
        mResult = manager.IOModulControl("migtestodul", "uzuz", 7);
        mResult = manager.StopIOCycle();
        mResult = manager.StartIOCycle(20);
        while (!exit1)
        {
            inputImage[0].
            usleep(1000 * 500);
        }
        printf("Hallo aus %s!\n", "migpiomodulTest");
    }
    return 0;
}