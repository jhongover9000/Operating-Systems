// =========================================================================================================
// =========================================================================================================
// OS Project 2: Coord (CPP)
// Joseph Hong
// Description: This is the coord node for the overall program. It receives the data from the root
// and uses this in order to create multiple sorters and a merger to merge the result from the sorters.
// =========================================================================================================
// =========================================================================================================
// Includes
#include <cstdlib>
#include <time.h>
#include <sys/times.h>
#include <signal.h>         // forking and process signals
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>       // mkfifo
#include <fcntl.h>
#include <iostream>         // strings, i/o
#include <fstream>
#include <sstream>
#include <string.h>
using namespace std;

// =======================================================================================================================================
// =======================================================================================================================================
// Main Program
int main(int argc, char* argv[]){

    // Random Seed (For Random Ranges)
    srand(time(NULL));

    // Initialization Arguments
	string mainCSV;                    // file path for CSV file (input file)
    string outputFile;                 // file path for CSV file (output file)
	int numWorkers;                    // number of sorters
	bool randomRange = false;          // random range selection (default is false)
    int attributeNum = 0;              // selection of sorting type (index)            
    bool displayDescending = true;     // display in descending order (default is true)
    int totalSize = 0;                 // total number of lines
    int sizeCounter;                   // used for the random ranges

    // Character Arrays of Variables
    char inputFileChar[40];
    char startIndexChar[40];
    char numLinesChar[40];
    char sortTypeChar[40];
    char rootPIDChar[40];

    char totalSizeChar[40];
    char numWorkersChar[40];
    char attributeNumChar[40];
    char displayDescendingChar[40];
    char outputFileChar[40];

    char currentPIDChar[40];

    // Fork Variables
	int status;
    int childNum = 0;               // child number
    int startIndex = 0;             // starting index
    int numLines;                   // number of lines to read
    int currentIndex;               // current index (for merging)
    pid_t pid;
    pid_t ppid = getppid();         // ppid to pass to sorters so that they can send a signal directly to root
    pid_t mergerPID;                // merger PID
    
    // Parse Command Line Arguments. Also checks if arguments are valid.
    for(int i = 1; i < argc; i++){
        // Check arguments
        if(argv[i][0] == '-'){
            // If '-i', check to make sure that the file exists.
            if(argv[i][1] == 'i'){
                mainCSV = argv[i+1];
                strcpy(inputFileChar, argv[i+1]);
            }
            // If '-k', create an array to store the PIDs of the children.
            else if(argv[i][1] == 'k'){
                numWorkers = atoi(argv[i+1]);
                sprintf(numWorkersChar, "%d", numWorkers);
            }
            // If '-r', set randomRange to true.
            else if(argv[i][1] == 'r'){
                randomRange = true;
            }
            // If '-a', set attribute index number.
            else if(argv[i][1] == 'a'){
                attributeNum = atoi(argv[i+1]);
                sprintf(attributeNumChar, "%d", attributeNum);

            }
            // If '-o', check if descending or ascending
            else if(argv[i][1] == 'o'){
                string argument = argv[i+1];
                if(argument == "d"){
                    displayDescending = true;
                }
                else if(argument == "a"){
                    displayDescending = false;
                }
                strcpy(displayDescendingChar,argv[i+1]);
            }
            // If '-s', designate the outputFile path
            else if(argv[i][1] == 's'){
                outputFile = argv[i+1];
                strcpy(outputFileChar, argv[i+1]);
            }
        }
    }

    // Check Size of Input File (Count Lines)
    ifstream readFile(mainCSV);
    string line;
	// If the file is readable, then get size by going line by line.
	while(getline(readFile, line)){
		totalSize++;
	}
	readFile.close();

    // Update sizeCounter, create array for sorter inputs
    sizeCounter = totalSize;                    // total number of lines in the file
    if(totalSize < numWorkers){
        cout<<"error: Number of workers exceeds number of lines."<<endl;
        exit(1);
    }
    int children[numWorkers];                   // array to store PIDs of children
    int childNumLines[numWorkers];              // number of lines per child
    sprintf(totalSizeChar, "%d", totalSize);
    string mainArray[totalSize];

    // Create Sorters. Loop based on total number of workers.
    for(childNum = 0; childNum < numWorkers; childNum++){
        // if random ranges, the number of lines is decided here for each sorter. max is half of the remaining lines, min is 1 line.
        if(randomRange){
            // if the sorter is not the last, do random
            numLines = rand()%(sizeCounter/2) + 1;
            // if the sorter is the last, the rest of the lines are assigned
            if(childNum == numWorkers-1){
                numLines = sizeCounter;
            }
        }
        // the standard size is totalSize/numWorkers
        else{
            numLines = totalSize/numWorkers;
            // for the last sorter, the remaining lines are used as numLines
            if(childNum == (numWorkers-1)){
                numLines = sizeCounter;
            }
        }
        // update the remaining number of lines
        sizeCounter -= numLines;

        // assign values to arguments for sorter. SortType indicates which method will be used.
        sprintf(startIndexChar, "%d", startIndex);
        sprintf(numLinesChar, "%d", numLines);
        sprintf(rootPIDChar, "%d", ppid);
        if(childNum%2 == 0){
            sprintf(sortTypeChar, "%d", 0);
        }
        else{
            sprintf(sortTypeChar, "%d", 1);
        }
        // create new process. Check for errors.
        if((pid = fork()) < 0){
            cout << "coord: Error creating child process (sorter)." << endl;
            exit(1);
        }
        // if child, execute new program for 'sorter' node. Check for errors.
        if(pid == 0){
            execlp("./sorter", "sorter", inputFileChar, startIndexChar, numLinesChar, sortTypeChar, rootPIDChar, attributeNumChar, displayDescendingChar, NULL);
            cout << "coord: Failed to load sorter." << endl;
            exit(1);
        }
        // otherwise, update the worker array and create a named pipe.
        children[childNum] = pid;
        childNumLines[childNum] = numLines;
        sprintf(currentPIDChar, "%d", pid);
        mkfifo(currentPIDChar, 0777);
        startIndex += numLines;
    }

    // Create Merger. Save PID to mergerPID. Create pipe and send sorter PIDs.
    if((pid = fork()) < 0){
        cout << "coord: Error creating child process (merger)." <<endl;
    }
    if(pid == 0){
        execlp("./merger", "merger", totalSizeChar, numWorkersChar, attributeNumChar, displayDescendingChar, outputFileChar, rootPIDChar, NULL);
        cout << "coord: Failed to load merger." << endl;
        exit(1);
    }
    mergerPID = pid;
    sprintf(currentPIDChar, "%d", mergerPID);
    mkfifo(currentPIDChar, 0777);
    // create pipe to send sorter PIDs
    int mergerfd = open(currentPIDChar, O_WRONLY);
    for(int i = 0; i < numWorkers; i++){
        int bytesWritten = write(mergerfd, &children[i], sizeof(children[i]));
    }
    // send sorter numLines
    for(int i = 0; i < numWorkers; i++){
        int bytesWritten = write(mergerfd, &childNumLines[i], sizeof(childNumLines[i]));
    }
    close(mergerfd);

    // Wait Code Segment. Makes sure that the coord waits for all the sorters/merger to finish and exit.
    while ((pid = wait(&status)) > 0);

    exit(0);
}