/*
 * =====================================================================================
 *
 *       Filename:  ngrams.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  01/26/2015 19:53:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bradley Golden (BG), bgolde5@uic.edu, golden.bradley@gmail.com
 *   Organization:  University of Illinois at Chicago
 *
 * =====================================================================================
 */

/*  strtok example */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_BUF 512
#define fileSelector 0 //1: very_short; 2: total_counts 3: words_that_start_with_a_q; 4: all_words

//snapshot of each line in the text file
typedef struct {
    char word[45]; //the longest word in the US dictionary is 45 characters!
    int year;
    int numOccur;
    int numTexts;
} ngramsData;

//collection of useful stats
typedef struct {
    int numLines;
    int distinctWords;
} ngramsStats;

/* function prototypes */
ngramsData *readData(int *size, char *filename);
void numDistinctWords(ngramsData *rawData, ngramsStats *usefulData, int size);
int wordsAreSame(char *str1, char *str2);
void printData(ngramsData *rawData, int n);
void printHeader();
char *wordPrompt();
char *filePrompt();
int searchByWord(char *word, ngramsData *rawData, int n);
double *averageFrequency(char *word, ngramsData *rawData, int wordIndex, int n);
int numWords(char *word, ngramsData *rawData, int wordIndex, int numWords);
int isInYearRange(int year);
int addTexts(ngramsData *rawData, int start, int range);
int addFreq(ngramsData *rawData, int start, int range, int yMin, int yMax);
void graph(double arr[], int size, char *title, char *word, double yMin, double yMax);
double findMin(double arr[], int n);
double findMax(double arr[], int n);
double yTitleStep(double min, double max);

int main (){
    
    ngramsData *rawData;
    ngramsStats *usefulData = (ngramsStats*)malloc(sizeof(ngramsStats));
    int size = 0;
    
    printHeader(); //prints required information by instructor at top of program
    char *fileName = filePrompt();
    //printf("You entered %s\n", fileName);
    rawData = readData(&size, fileName); //read data dynamically from the specified file
    //char **wordList = createWordArr(rawData, size);
    //printData(rawData, size);
    usefulData->numLines = size; //collect size into usefulData struct
    //printf("size: %i\n", size);
    numDistinctWords(rawData, usefulData, size); //gives the number of distinct words in the given file
    char *wordInput = wordPrompt(); //prompts user to input a word
    //printf("You entered: %s\n", wordInput);
    int wordIndex = searchByWord(wordInput, rawData, size);
    //printf("Word found at: %i\n", wordIndex);
    int count = numWords(wordInput, rawData, wordIndex, size);
    //printf("word count: %i\n", count);
    double *wordFreq = averageFrequency(wordInput, rawData, wordIndex, count);
    int i;
    for(i=0; i<40; i++){
        printf("%i: average word frequency: %f\n", i, wordFreq[i]);
    }
    double minVal = findMin(wordFreq, 40);
    double maxVal = findMax(wordFreq, 40);
    printf("minVal: %f\n", minVal);
    printf("maxVal: %f\n", maxVal);
    char title[] = "Word Frequency";
    graph(wordFreq, count, title, wordInput, minVal, maxVal);
    
    free(wordFreq);
    free(usefulData);
    free(rawData);
    free(fileName);
    free(wordInput);
    
    return 0;
}

/**
 * function: graph
 * description: prints graph of data to the screen
 */
void graph(double arr[], int size, char *title, char *word, double yMin, double yMax){
    int i = 0;
    int j = 0;
    double yStep;
    double yTitle;
    if(yMin > 0.5)
        yStep = yTitleStep(yMin-0.5, yMax+0.5);
    else
        yStep = yTitleStep(yMin, yMax+0.5);
    
    printf("yStep: %.2g\n", yStep);
    yTitle = yMax;//+yStep;
    printf("\t\t\t\t\t\t\t\t\t%s (%s)\t\t\n", title, word);
    printf("\t\t\t ------------------------------------------------------------------------------------------------------------------------\n");
    //display bars
    for(i=0; i<50; i++){
        printf("%f\t\t|", yTitle);
        for(j=0; j<40; j++){
            //if arr[i] + yTitle print " X "
            if(arr[j] >= yTitle){
                printf(" X ");
            }
            //else print "   "
            else{
                printf("   ");
            }
        }
        printf("|\n");
        yTitle-=yStep;
    }
    printf("\t\t\t ------------------------------------------------------------------------------------------------------------------------\n");
    printf("\t\t\t ");
    //display years
    for(i=1; i<=40; i++){
        printf(" %i ", i/10);
    }
    printf("\n\t\t\t ");
    for(i=1; i<=40; i++){
        printf(" %i ", i%10);
    }
    printf("\n\t\t\t\t\t\t\t\t\tYear\n");
}//end graph

double yTitleStep(double min, double max){
    double step;
    
    step = (max - min) / 49;
    
    return step;
}

/**
 * function: findMin
 * description: finds the min of a given array
 */
double findMin(double arr[], int n){
    double min;
    int i;
    min = arr[0];
    for(i=1; i<n; i++){
        if(arr[i] < min){
            min = arr[i];
        }
    }
    return min;
}//end findMin
/**
 * function: findMax
 * description: finds the max of a given array
 */
double findMax(double arr[], int n){
    double max;
    int i;
    max = arr[0];
    for(i=1; i<n; i++){
        if(arr[i] > max){
            max = arr[i];
        }
    }
    return max;
}//end findMax

/**
 * function: addFreq
 * description: adds word frequency in a specified year range
 */
int addFreq(ngramsData *rawData, int start, int range){
    int sum, i;
    sum = 0;
    for(i=start; i<(start+range); i++){
        sum+=rawData[i].numOccur;
        //printf("sum: %i\n", sum);
    }
    return sum;
}//end addRange

/**
 * function: addTexts
 * description: add number of texts in a specified year range
 */
int addTexts(ngramsData *rawData, int start, int range){
    int sum, i;
    sum = 0;
    for(i=start; i<(start+range); i++){
        sum+=rawData[i].numTexts;
    }
    return sum;
}//end addTexts

/** function: averageFrequency
 *  description: gives the frequency of a given word every 5 years
 */
double *averageFrequency(char *word, ngramsData *rawData, int wordIndex, int numWords){
    double *ave;
    int freq, texts;
    int i = 0;
    ave = (double*)malloc(numWords*sizeof(double));//size of ave depends on number of words in the specified range of years
    
    //average years if year is in the range of 1801 - 2000
    while(wordsAreSame(word, rawData[wordIndex].word) == 0){
        while(isInYearRange(rawData[wordIndex].year)){
            //sum 5 year range of word frequency
            //printf("year: %i\n", rawData[wordIndex].year);
            freq = addFreq(rawData, wordIndex, 5);
            //printf("freq: %i\n", freq);
            texts = addTexts(rawData, wordIndex, 5);
            //printf("texts: %i\n", texts);
            ave[i] = (double)freq / texts;
            //printf("%i: ave[%i]: %f\n", i, i, ave[i]);
            i++;
            wordIndex+=5;
        }
        wordIndex++;
    }
    return ave;
}//end averageFrequency


/** function: isInYearRange
 *  description: determines whether the given word is between 1801 and 2000
 */
int isInYearRange(int year){
    if(year > 1800 && year < 2001)
        return 1;
    return 0;
}
/** function: numWords
 *  description: counts the number of occurences of a given word within a specified timeframe
 */
int numWords(char *word, ngramsData *rawData, int wordIndex, int n){
    int count, i;
    count = 0;
    i = 0;
    while(wordsAreSame(word, rawData[i].word) == 0){
        if(isInYearRange(rawData[i].year))
            count++;
        i++;
    }
    return count;
}//end numWords


/**
 * function: searchByWord
 * description: seraches for a given word and returns the index of that word if found
 */
int searchByWord(char *word, ngramsData *rawData, int n){
    int i;
    for(i=0; i<n; i++){
        if(wordsAreSame(word, rawData[i].word) == 0){
            return i;
        }
    }
    return -1;
}//end searchByWord

/**
 * function: filePrompt
 * description: prompts the user for a file name
 */
char *filePrompt(){
    char *file = (char*)malloc(50*sizeof(char));
    printf("Enter a filename: ");
    scanf("%s", file);
    return file;
}//end filePrompt

/**
 * function: wordPrompt
 * description: prompts the user for a word of type char*
 */
char *wordPrompt(){
    char *word = (char*)malloc(50*sizeof(char));
    printf("Enter a word: ");
    scanf("%s", word);
    return word;
}

/**
 * function: printHeader
 * description: prints the required header in CS251 including:
 * author, lab and time, program # and name
 */
void printHeader(){
    printf("\n\n\nAuthor: Bradley Golden\n");
    printf("Lab: Thur 11am\n");
    printf("Program: #1, Google NGram word count\n\n\n");
}


/**
 * function: numDistinctWords
 * description: counts the number of distinct words in the given file
 */
void numDistinctWords(ngramsData *rawData, ngramsStats *usefulData, int n){
    int i;
    int count = 1;//the first word is distinct and is thus counted
    for(i=0; i<n-1; i++){
        if(wordsAreSame(rawData[i].word, rawData[i+1].word)!=0)
            //printf("word1: %s word2: %s\n", rawData[i].word, rawData[i+1].word);
            count++;
    }
    usefulData->distinctWords = count;
    
    printf("Total number of distinct words: %i\n", count);
}//end numDistinctWords

/**
 * function: wordsAreSame
 * description: returns the strcmp value of two strings
 */
int wordsAreSame(char *str1, char *str2){
    int num = 0;
    int i =0;
    int len1, len2;
    len1 = strnlen(str1, 45);
    len2 = strnlen(str2, 45);
    
    //force strings to be lowercase
    for(i=0; i<len1; i++){
        str1[i] = tolower(str1[i]);
    }
    for(i=0; i<len2; i++){
        str2[i] = tolower(str2[i]);
    }
    num = strncmp(str1, str2, 45);
    return num;
}//end wordsAreSame

/**
 * function: readData
 * description: reads ngrams data from file input in the following format:
 * word, year, number of occurences in that year, number of texts in which that word appeared
 *
 * returns a dynamically allocated array of structs of type ngramsData
 *
 * TODO: fix mem leak?
 */
ngramsData *readData(int *size, char *fileName){
    ngramsData *temp;
    *size = 0;
    int i = 0;
    int j = 0;
    int totalAllocated = 1;
    ngramsData *rawData = (ngramsData*)malloc(totalAllocated*sizeof(ngramsData));; //allocated for dynamic allocation
    
    FILE *ifp;
    
    ifp = fopen(fileName, "r");
    
    if(ifp == NULL){
        fprintf(stderr, "Cannot open input file %s", fileName);
    }
    
    //start dynamic allocation
    while(fscanf(ifp, "%s %d %d %d", rawData[i].word, &rawData[i].year, &rawData[i].numOccur, &rawData[i].numTexts) != EOF){
        if(i == totalAllocated){ //reached end of array, need to allocated more space
            temp = (ngramsData*)malloc(((totalAllocated*2)+1)*sizeof(ngramsData));
            for(j=0;j<=i;j++){
                temp[j] = rawData[j];
            }
            free(rawData);
            rawData = NULL;
            free(rawData);
            rawData = temp;
            totalAllocated = totalAllocated * 2;
        }
        
        //rawData[i].wordArr[i] = rawData[i].word;
        i++;
        //printf("size: %i\n", i);
    }
    *size = i;
    
    fclose(ifp);
    
    return rawData;
}//end readData

/**
 * function: printData
 * description: prints the current words, etc. tha are stored in struct ngramsData
 */
void printData(ngramsData *rawData, int n){
    
    int i = 0;
    
    for(i=0; i<n; i++){
        printf("%s %i %i %i\n", rawData[i].word, rawData[i].year, rawData[i].numOccur, rawData[i].numTexts);
    }
}
