/****************************************************
Name: Sohail Selky
Date: 2/18/2016
Problem Number: 3
Hours spent solving the problem: 17
Instructor: Komogortsev, TSU
*****************************************************/

// This is driver's code


#ifdef linux
#endif

#ifdef __CYGWIN32__
#define LINUX false
#define WINDOWS true
#endif


#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>

#include "life.h"

const char FILE_NAME[] = "glider_gun_fight.txt";

using namespace std;

const int NUM_GENERATIONS = 10; //set to a smaller number for debugging

int main()
{
    populateWorld(FILE_NAME);

    showWorld();

    for (int iteration = 0; iteration < NUM_GENERATIONS; iteration++)
    {


        system("clear"); //Linux only
        iterateGeneration();
        showWorld();
    }
    return 0;
}
