/****************************************************
Name: Sohail Selky
Date: 2/18/2016
Problem Number: 3
Hours spent solving the problem: 17
Instructor: Komogortsev, TSU
*****************************************************/

//This header file provides the prototypes of the function definitions
//for the project.

#ifndef life_h
#define life_h

#include <iostream>
#include <fstream>


void populateWorld(const char * file);
void showWorld();
void iterateGeneration();
int checkCell(char); //checks cell and returns interger for counting up live cells surrounding
char** findFirstLast(char ** , char ** ); //looks for the first/last row/col of live cells


#endif
