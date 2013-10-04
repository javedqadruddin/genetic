//
//  main.c
//  GeneticAlg
//
//  Created by Javed Qadrud-Din on 9/27/13.
//  Copyright (c) 2013 Javed Qadrud-Din. All rights reserved.
//

/*
 This program uses a genetic algorithm to create a string that matches a string of ASCII capital letters of max length 100 characters.
*/



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <strings.h>
#include <string.h>


const int MAXPOP = 200;
const int CULLEDPOP = 50;
const int MUTATIONRATE = 40; //this is the inverse of the mutation frequency, so lower number means more mutations

typedef struct
{
    int score;
    char* string;
} sequence;


int RandGen(int min, int max);
char* GenerateInitStrings(int length, sequence* offspring);
void PrintAllStrings(sequence* offspring, int numStrings, int length);
sequence ScoreOffspring(sequence* offspring, sequence* parentHeap, int MAXPOP, int CULLEDPOP, char* key);
void NewGeneration(sequence* offspring, sequence* parentHeap, int CULLEDPOP, int MAXPOP, int MUTATIONRATE);

void AddIfParent(sequence* parentHeap, sequence newElement, int* heapSize, int CULLEDPOP);
void FixHeap(sequence* parentHeap, int heapSize);
int RightPosition(sequence* parentHeap, int position, int heapSize);
void Swap (sequence* parentHeap, int position, int positionChild);
sequence LeftChild(sequence* parentHeap, int position);
sequence RightChild(sequence* parentHeap, int position, int heapSize);
int Leaf(sequence* parentHeap, int position, int heapSize);




int main(int argc, char* argv[])
{
	char* key = "FROGZZZZZZZJJDHENNEKNKLFNALKNFKLANFDSFKJAHJDAHFKJASD";//argv[1];
    
    //initializes a random seed using the current time as the seed
    int* randseed = malloc(sizeof(int));
    *randseed = time(NULL);
    srandom(*randseed);
    
    //length is the length of the input string plus the end of string marker
    int length = strlen(key) + 1;
    
    //counter to track number of generations elapsed
    int generations = 0;
    
	printf("%s\n", key);
    
    sequence parentHeap[CULLEDPOP];
    sequence offspring[MAXPOP];
    
    sequence winner;
    winner.score = 0;
    winner.string = malloc(length * sizeof(char));
    
    char* initStrings = GenerateInitStrings(length, offspring);
    
    //PrintAllStrings(offspring, MAXPOP, length);
    
    //score the initial set of offspring and put the next parents into parentHeap
    winner = ScoreOffspring(offspring, parentHeap, MAXPOP, CULLEDPOP, key);
    
    //for(int i = 0; i < MAXPOP; i++)
      //  printf("%d\n", offspring[i].score);
    
    //main loop - ScoreOffspring will return a sequence containing a score if a winner is found, else it will return a sequence with score = 0
    while(!winner.score)
    {
        generations++;
        printf("Generation %d ", generations);
        NewGeneration(offspring, parentHeap, CULLEDPOP, MAXPOP, MUTATIONRATE);
        winner = ScoreOffspring(offspring, parentHeap, MAXPOP, CULLEDPOP, key);
    }
    
    
    printf("The key is: %s\n", winner.string);
    printf("Program completed in %d generations\n", generations);
    printf("Total of %d total strings tried\n", generations * MAXPOP);
    
    unsigned long long everyCombo = 26;
    
    for(int i = 0; i < length; i++)
    {
        everyCombo *= 26;
    }
    printf("Trying every combination would require %lld tries\n", everyCombo);
    
	
    
	return 0;
}



//=====================================================================================================================================

int SumScores(sequence* parentHeap, int CULLEDPOP)
{
    int sum = 0;
    
    for(int i = 0; i < CULLEDPOP; i++)
    {
        sum += parentHeap[i].score;
    }
    
    if(sum > 0)
        return sum;
    else
        return 1;
}




//=====================================================================================================================================


void NewOffspring(sequence* offspring, sequence parent, int numOffspring, int position)
{
    int length = strlen(offspring[0].string);
    int mutation = 0;
    
    for(int i = 0; i < numOffspring; i++)
    {
        for(int j = 0; j < length; j++)
        {
            mutation = RandGen(0, MUTATIONRATE);
            if(mutation == MUTATIONRATE)
                offspring[position + i].string[j] = RandGen(65, 90);
            else
                offspring[position + i].string[j] = parent.string[j];
        }
    }
}



//=====================================================================================================================================

void NewGeneration(sequence* offspring, sequence* parentHeap, int CULLEDPOP, int MAXPOP, int MUTATIONRATE)
{
    sequence topSequence = parentHeap[0];
    
    int scoreSum = SumScores(parentHeap, CULLEDPOP);
    
    //variable to keep track of where we are in replacing the offspring array
    int newOffspringCounter = 0;
    
    //variable to store number of new offspring a given sequence is entitled to
    int newOffspringShare = 0;
    
    for (int i = 0; i < CULLEDPOP; i++)
    {
        if(parentHeap[i].score > topSequence.score)
            topSequence = parentHeap[i];
        
  /*      newOffspringShare = ((parentHeap[i].score) / scoreSum) * MAXPOP;
     
        if (newOffspringShare != 0) {
            printf("yo\n");
        }
        
        NewOffspring(offspring, parentHeap[i], newOffspringShare, newOffspringCounter);
        
        newOffspringCounter += newOffspringShare;*/
    }
    
    
    //in case, due to rounding down of the offspring share, there are less than MAXPOP offspring generated, the following loop fills in the last bit by
    //making offspring from the best sequence.
    while(newOffspringCounter < MAXPOP)
    {
        NewOffspring(offspring, topSequence, 1, newOffspringCounter);
        newOffspringCounter++;
    }
    
    printf("%s %d\n", topSequence.string, topSequence.score);
    
}








//=====================================================================================================================================
//Loops through the offspring array and scores each offspring based on number of characters matching the key, returns a sequence with
//score = 0 if winner is not found, else returns the winning sequence.  Calls AddIfParent to put top CULLEDPOP sequences into the parentHeap

sequence ScoreOffspring(sequence* offspring, sequence* parentHeap, int MAXPOP, int CULLEDPOP, char* key)
{
    //variable to keep track of the number of elements added to parentHeap so far
    int heapSize = 0;
    
    int length = strlen(offspring[0].string);
    
    for(int i = 0; i < MAXPOP; i++)
    {
        int score = 0;
        for(int j = 0; j < length; j++)
        {
            if(offspring[i].string[j] == key[j])
                score++;
        }
        
        offspring[i].score = score;
        if (score >= length)
        {
            return offspring[i];
        }
        AddIfParent(parentHeap, offspring[i], &heapSize, CULLEDPOP);
    }
    
    sequence nullSequence;
    nullSequence.score = 0;
    return nullSequence;
    
    
}






//=====================================================================================================================================
//Called by ScoreOffspring.  Adds an offspring passed to it to the parentHeap if that offspring has a score greater than the current lowest
//score in the parentHeap.  Also adds offspring to the parentHeap if the parentHeap is not yet at full size (CULLEDPOP).

void AddIfParent(sequence* parentHeap, sequence newElement, int* heapSize, int CULLEDPOP)
{
    
    if(*heapSize < CULLEDPOP)
    {
        parentHeap[*heapSize] = newElement;
        *heapSize = *heapSize + 1;
        FixHeap(parentHeap, *heapSize);
    }
    else if(newElement.score > parentHeap[0].score)
    {
        parentHeap[0] = newElement;
        FixHeap(parentHeap, *heapSize);
    }
    
}





//=====================================================================================================================================
//Restores the order of the parentHeap after element is added.

void FixHeap(sequence* parentHeap, int heapSize)
{
    int position = 0;
    
    //printf("--------%d\n", heapSize);
    //for(int i = 0; i < CULLEDPOP; i++)
     //   printf("%d ", parentHeap[i].score);
    //printf("\n");
    
    while(!RightPosition(parentHeap, position, heapSize))
    {
        
        int offset = 0;
        if((LeftChild(parentHeap, position).score < parentHeap[position].score) && (LeftChild(parentHeap, position).score <= RightChild(parentHeap, position, heapSize).score))
        {
            Swap(parentHeap, position, position * 2 + 1);
            position = (position * 2) + 1;
        }
        else
        {
            Swap(parentHeap, position, position * 2 + 2);
            position = (position * 2) + 2;
        }
        
        //for(int i = 0; i < CULLEDPOP; i++)
          //  printf("%d ", parentHeap[i].score);
        //printf("\n");
        
    }
}



//=====================================================================================================================================
//Checks if an object is in the right position in the parentHeap.  Checks if either child is smaller than the object, if so, returns 0.
//Checks if it's a leaf, if so, returns 1, and returns 1 in all other cases (which includes both children being larger than object).

int RightPosition(sequence* parentHeap, int position, int heapSize)
{
    if(Leaf(parentHeap, position, heapSize))
        return 1;
    else if(LeftChild(parentHeap, position).score < parentHeap[position].score)
        return 0;
    else if(RightChild(parentHeap, position, heapSize).score < parentHeap[position].score)
        return 0;
    else
        return 1;
}





//=====================================================================================================================================
//Swaps the position of two elements in the parentHeap

void Swap (sequence* parentHeap, int position, int positionChild)
{
    sequence temp;
    
    temp = parentHeap[position];
    
    parentHeap[position] = parentHeap[positionChild];
    
    parentHeap[positionChild] = temp;
}



//=====================================================================================================================================
//LeftChild and RightChild return the sequence that is the left or right child (respectively) of the object at position.  RightChild will
//return LeftChild if the position where the RightChild would be happens to be beyond the end of the heap.
sequence LeftChild(sequence* parentHeap, int position)
{
    return parentHeap[position * 2 + 1];
}

sequence RightChild(sequence* parentHeap, int position, int heapSize)
{
    if(position * 2 + 2 < heapSize)
        return parentHeap[position * 2 + 2];
    else
        return LeftChild(parentHeap, position);
}



//=====================================================================================================================================
//Returns 1 if the sequence at position in the heap is a leaf, returns 0 if not a leaf.
int Leaf(sequence* parentHeap, int position, int heapSize)
{
    
    if(position >= (heapSize-1) / 2)
        return 1;
    else
        return 0;
}



//=====================================================================================================================================
//GenerateInitStrings takes the length of the desired strings and a random seed and generates a series of random strings of length length
//and returns the memory address of the first character in that series of strings.

char* GenerateInitStrings(int length, sequence* offspring)
{

    //allocate enough memory for MAXPOP number of strings
	char* initStrings = malloc(MAXPOP * length * sizeof(char));
    
    char* position = initStrings;
    
    //loop through to create each string
    for (int i = 0; i < MAXPOP; i++)
    {
        
        //make the .string of this offspring element point to the beginning of the string that's about to be generated
        offspring[i].string = position;
        
        //loop to add random characters into each slot in the current string, except the last slot which will be for the NULL char
        for (int j = 0; j < (length - 1); j++)
        {
            //puts a random char into the memory block denoted by position
            *position = RandGen(65, 90);
            
            //moves position forward one byte
            position ++;
            
        }
        
        //puts a NULL at the end of the current string
        *position = 0;
        position++;
        
    }
    
    //loop to output the whole array for testing
    //for(int i = 0; i < MAXPOP * length; i++)
    //{
     //   printf("%c", *(initStrings + i));
    //}
   // printf("\n");
    
	return(initStrings);
}




//=====================================================================================================================================
//Takes the memory address of the first character in a series of strings, the length of each string in the set, the number of strings in the set
//and then prints each of those strings separated by a newline.


void PrintAllStrings(sequence* offspring, int numStrings, int length)
{
    //char* position = strings;
    
    //loop through each string
    for (int i = 0; i < numStrings; i++)
    {
        printf("%s\n", offspring[i].string);
        
        //moves position to the next spot in the "array" of strings
        //position = strings + i * length;
    }

}


//=====================================================================================================================================
//RandGen() takes a min and a max value as parameters and a pointer to a random seed and returns 1 random integer with value between min
//and max, inclusive.  For this to work properly, srandom() should be initialized before executing the RandGen() function.


int RandGen(int min, int max)
{
    
    //srandom(*randseed);

    //properly randomizes the seed, as the program executes too fast for time(NULL) to work, so must increment it for each rand num produced
    //*randseed = *randseed + 1;
    //int checkrandseed = *randseed;
    
	int randomNum = random();
    
	int range = max - min;
    
	randomNum = randomNum % (range + 1); //gives us random numbers between 0 and the difference between max and min
    
	return(randomNum + min);  //adds a bottom to the range and returns
    
}