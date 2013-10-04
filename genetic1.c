/*
This program uses a genetic algorithm to create a string that matches a string of ASCII values of max length 100 characters.
1. Generate 500 random strings
2. Score them against the key
	a. One point for every matched char
	b. Note the max score
3. Pick the 100 best
	a. Go thru and check scores
	b. For each score, create a struct containing the score and all the strings that have that score, also note the number of strings in each struct
	c. Sort the structs based on score
	d. Take the first 100 strings in the sorted structs
3. Decide how many offspring each should have
4. Generate the offspring
	a. Outer loop loops through the array of strings
	b. Inner loop loops through an individual string, making a copy of it and making an error at random avg 1 in 10 times as it copies
5. Check if match has been found
6. Repeat from step 2 until a string matches the key

*/



#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXPOP = 500;
#define CULLEDPOP = 100;

int RandGen(int min, int max);
void GenerateInitStrings(void);


int main(int argc, char* argv[])
{
	char* key = argv[1];
	printf("%s\n", key);

	char* initStrings[MAXPOP] = GenerateInitStrings(key);

	//ScoreStrings(GenerateInitStrings(key));

	for(int i = 0; i < MAXPOP; i++)
	{
		srandom(time(NULL) + i);
		printf("%s\n", RandGen(0, 100));
	}

	

	return(0);
}



char* GenerateInitStrings(char* key)
{
	char (*initStrings)[MAXPOP] = malloc(MAXPOP * strlen(key) * sizeof(char));



	return(initStrings);
}






int RandGen(int min, int max)
{


	int randomNum = random();

	int range = max - min;

	randomNum = randomNum % (range + 1); //gives us random numbers between 0 and the difference between max and min

	return(randomNum + min);  //adds a bottom to the range and returns

}