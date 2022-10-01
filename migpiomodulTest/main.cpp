#include <cstdio>
#include <mi/miio/IOManager.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream

int main()
{
    std::ifstream inFile;
    inFile.open("/home/root/config.json"); //open the input file

    std::stringstream strStream;
    strStream << inFile.rdbuf(); //read the file
    std::string str = strStream.str(); //str holds the content of the file
    miIOManager::IOManager manager(2048, 2048);
    manager.AddIOModul(str);
    manager.IOModulControl("migpioModul", "uzuz", 7);
    manager.StopIOCycle();
    manager.StartIOCycle(20);
    printf("Hallo aus %s!\n", "migpiomodulTest");
    return 0;
}