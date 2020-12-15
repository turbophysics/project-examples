/****************************************************
Name: Sohail Selky
Date: 2/18/2016
Problem Number: 3
Hours spent solving the problem: 17
Instructor: Komogortsev, TSU
*****************************************************/


//This file provides the implementation of the life.h header file.



#include "life.h"

//GLOBAL VARIABLES

//NOTE: I was getting linker errors every time I put this data into the life.h
//	file, so I put them here.


int ROWS = 1;         //stores the number of rows in the grid
	//NOTE: ROWS STARTS AT 1 TO ACCOUNT FOR LAST LINE IN FILE NOT HAVING /n
int COLUMNS = 0;      //stores the number of cols? in the grid


char ** sbArr = 0;//this dynArray is displayed and carried between iterations
char ** lifeArr = 0;//this dynarray is whats examined (life/death)

using namespace std;

//my algorithm for solving this problem went like this:
//
//step 1: populateWorld > find size of read-in array by counting 0,1s and /n
//step 2: reset curser, read file again feeding in data into tempArr
//step 3: parse all but 1 row of dead cells from tempArr into sbArr.
//	  findFirstLast() takes 2 arrays, finds first and last live cells,
//	  and returns a new parsed array, then nulls out the first
//step 4: showWorld() > cout every item in sbArr
//step 5: iterateGen() creates lifeArr and now looks at the surrounding cells of
// 	  each cell in sbArr to  determine if it lives/dies, deletes sbArr and
//	  then rebuilds it with findFirstLast
//step 5: showWorld() with new sbArr
//step infinity: cycles through steps 5 and 4 for as many generations as set by
// 	         the driver

//sidenote: this project took me 17hrs. 10 was spent writing/testing different
//	methods and an addition 6 was spent debugging (the last hour was
//	formatting and comments.) I think in the future my psuedocode will
//	be much mucchhh more refined and specific to avoid confusion.



void populateWorld(const char * file)
{

	ifstream readin;		//open file
	readin.open(file);
	char test;		//char test is an input for a char by char
	bool eol = 0;	//bool: dont count cols after the first line
			//once eol triggers, stops counting 0,1s

	while (!readin.eof())	//count until eof is reached
	{

		readin.get(test);
		if (test == '0' || test == '1')
		{
			if (!eol)
			{
				COLUMNS += 1; //add columns until first line
			}
		}
		else if (test == '\n')
		{
			ROWS += 1;  //add to ROWS for every newline
			eol = 1;	//ensure eol is triggered
		}
	}

	readin.clear();
	readin.seekg(0, ios::beg); //resets the curser to start reading in file

	lifeArr = new char *[ROWS]; //create pointers for each row

	for (int i = 0; i < ROWS; i++)
		lifeArr[i] = new char[COLUMNS]; //create array pointer
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
			readin >> lifeArr[i][j];
	}
readin.close();


sbArr = findFirstLast(lifeArr, sbArr);//parse lifeArr into sbArr


}


//This function outputs the grid for current generation
void showWorld()
{

//char alive = (char)178;

	//display our sbArr
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			if (sbArr[i][j] == '1')
				cout << '&';
			else
				cout << '.';
		}
	cout << endl; //reads every item, couts a character depnding on content
	}
/* ^^ I left this here because i think it looks better in the output. I
//thought I saw somewhere that we could choose which characters went out but I
//must be wrong

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
			cout << sbArr[i][j];

	cout << endl; //reads every item
	}
	*/
}

//This function creats new geneneration grid from the old generation grid
void iterateGeneration()
{

	lifeArr = new char *[ROWS]; //establish our results table


	for (int i = 0; i < ROWS; i++)
	{
		lifeArr[i] = new char[COLUMNS];//set up columns in tempArr

		for (int j = 0; j < COLUMNS; j++)

		{
			int cells = 0; //counter for live cells surrounding



			//this block checks every cell surrounding depending on
			//what position in the array the cell *isnt*

		if (i != 0) //look up if not top row
			cells += checkCell(sbArr[i - 1][j]);
		if (j != 0) // lookleft if not left border
			cells += checkCell(sbArr[i][j - 1]);
		if (i != (ROWS - 1)) //look down if not bottom border
			cells += checkCell(sbArr[i + 1][j]);
		if (j != (COLUMNS - 1)) //look right if not right border
			cells += checkCell(sbArr[i][j + 1]);
// look up/down/left/right ^^


		if (i != 0 && j != 0) //look diagnal up/left if neither top/lft
			cells += checkCell(sbArr[i - 1][j - 1]);
		if (i != 0 && j != (COLUMNS - 1)) //look top right
			cells += checkCell(sbArr[i - 1][j + 1]);
		if (i != (ROWS - 1) && j != (COLUMNS - 1))//look down right
			cells += checkCell(sbArr[i + 1][j + 1]);
		if (i != (ROWS - 1) && j != 0)
			cells += checkCell(sbArr[i + 1][j - 1]);
// look in diagnal variants ^^

			//now figure out if the cell lives, dies or is born
			if (sbArr[i][j] == '0')
			{
				if (cells == 3) //if dead, determin birt
					lifeArr[i][j] = '1';	//output result
				else				//into sbArr
					lifeArr[i][j] = '0';
			}

		else if (cells == 2 || cells == 3)//must be alive
			lifeArr[i][j] = '1';
			else 			   //if alive determine death
			lifeArr[i][j] = '0'; //output result to sbArr
			}
	}
	//clears out our old sbArr once we have our lifeArr

	for (int i = 0; i < ROWS; i++)
		delete[] sbArr[i];
	delete [] sbArr;
		sbArr = 0;

sbArr = findFirstLast(lifeArr, sbArr);  //parse the results so the output has
					//exactly 1 row dead cells around it,
				//output parsed data into sbArr for display

}

char** findFirstLast(char ** arr1, char ** arr2)//this function looks for first
			//and last row/col with live cells in arr1 and then
{			//outputs the data into a 'padded' arr2

//I spent a lot of time making this modular and I realize now that it
// was a little unecessary


  int    firstR = -1,
	 lastR = -1,
         firstC = -1,      //initial values when looking for first/last
         lastC = -1,
         ROWS2 = 0,
         COLUMNS2 = 0; //dimensions for resized array (when we find them)

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			if (firstC < 0 && arr1[i][j] == '1')
			{
				firstC = j;
				lastC = j; //if you haven't found first col and
			}	       //and this cell lives, this is the first

			else if (j < firstC && arr1[i][j] == '1')
				firstC = j;
			   //change firstC if you find one in an earlier column


			else if (firstC > -1  && arr1[i][j] == '1' && j > lastC)
				lastC = j;
		//if you have found the first col, this cell is live, and its in
		//further column than our current lastC then store this as lastC
		//(until no more live cells are found

			if (firstR < 0 && arr1[i][j] == '1')
			{
				firstR = i;
				lastR = i; //same as cols
			}
			else if (firstR > -1 && arr1[i][j] == '1' && i > lastR)
				lastR = i; //same as cols
		}

	}

//if no live cells are found...
  if(firstR == -1) //if firstR is still -1 then no live cell was ever found
   {
     ROWS2 = 1;
     COLUMNS2 = 1; //if no live cells are found, this will still produce a
     firstR = 0;   //1x1 array of 0 to output for the remaining iterations
     lastR = 0;    //rather than segfaulting
     firstC = 0;
     lastC = 0;
   }
  else //else if live cell WAS found
   {
ROWS2 = lastR - firstR + 3;	//THIS IS DIMENSIONS OF NEW PARSED ARR
COLUMNS2 = lastC - firstC + 3;// +2 because of extra dead cells
				  // +1 because in sbtrcting it removes a col
   }



arr2 = new char *[ROWS2]; //now make array
for (int i = 0; i < (ROWS2); i++)
	{
		arr2[i] = new char[COLUMNS2];
		for (int j = 0; j < (COLUMNS2); j++)
		{

		if (i == (ROWS2 - 1) || j == (COLUMNS2 - 1) ||i == 0 || j == 0)
	    // making sure we add a row of dead cells to new first/last row/col
	    // (-1 to compensate for array starting at 0)

			arr2[i][j] = '0';
			//if its first/last row/col make it a dead cell

			else  //else pull the data from our loaded array
			arr2[i][j] = arr1[(i + firstR-1 )][(j + firstC-1)];

		}

//NOTE: This block of code ^^ was the source of many many segfaults. Part of it
//was stupid typos and part of it was getting an accurate algorithm to pull/
//parse information correctly without trying to get info that wasn't there
}

//clear it out the old useless arr1

for (int i = 0; i < ROWS; i++)
	delete[] arr1[i];
delete [] arr1;
arr1 = 0;


ROWS = ROWS2; //set ROWS and COLS to the dimensions of our new grid
COLUMNS = COLUMNS2; //they will be called when repopulating next iteration

return arr2; //returns that POINTER back; this was the only way I could find
	     //for referencing a dynamic array that was built in a function

}

int checkCell(char a) //small function to add up surrounding cells
{
	int b = 0;
	if (a == '1')
		return 1;
	else
		return 0;
}
