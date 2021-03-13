
#include <cstdlib>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>       // mkfifo
#include <sys/times.h>  

using namespace std;

// =======================================================================================================================================
// =======================================================================================================================================
// Support Functions (for sorting functions)

// Trim Spaces. For comparisons. From GeeksforGeeks at https://www.geeksforgeeks.org/remove-extra-spaces-string/.
void removeSpaces(string &str)
{
    // n is length of the original string
    int n = str.length();
 
    // i points to next position to be filled in
    // output string/ j points to next character
    // in the original string
    int i = 0, j = -1;
 
    // flag that sets to true is space is found
    bool spaceFound = false;
 
    // Handles leading spaces
    while (++j < n && str[j] == ' ');
    // read all characters of original string
    while (j < n){
        // if current characters is non-space
        if (str[j] != ' '){
            // remove preceding spaces before dot,
            // comma & question mark
            if ((str[j] == '.' || str[j] == ',' || str[j] == '?') && i - 1 >= 0 && str[i - 1] == ' '){
                str[i - 1] = str[j++];
            }
            else{
                // copy current character at index i
                // and increment both i and j
                str[i++] = str[j++];
            }
            // set space flag to false when any
            // non-space character is found
            spaceFound = false;
        }
        // if current character is a space
        else if (str[j++] == ' '){
            // If space is encountered for the first
            // time after a word, put one space in the
            // output and set space flag to true
            if (!spaceFound){
                str[i++] = ' ';
                spaceFound = true;
            }
        }
    }
    // Remove trailing spaces
    if (i <= 1){
        str.erase(str.begin() + i, str.end());
    }
    else{
        str.erase(str.begin() + i - 1, str.end());
    }

}

// Get Attribute. Uses a string line and the index of the attribute.
double getAttributeDouble(string line, int attributeNum){
    // cout<<"Line: "<<line<<endl;
    string sarray[6];
    string substring;
    removeSpaces(line);
    stringstream ss(line);
    for(int i = 0; i < 6; i++){
        getline(ss, substring, ' ');
        sarray[i] = substring;
        // cout<<substring<<endl;
    }
    // cout<<"Attribute String: "<<sarray[attributeNum]<<endl;
    double attribute = stod(sarray[attributeNum]);
    return attribute;
}

// Comparison. Takes string A, the current string, and compares it to B. If true is returned,
// then a swap will take place. False means no swap need occur.
bool compare(string a, string b, int attributeNum, bool isDescending){
    // cout << "Starting Compare." <<endl;
    double attributeA = getAttributeDouble(a, attributeNum);
    // cout<<"Attribute A:"<<attributeA<<endl;;
    double attributeB = getAttributeDouble(b, attributeNum);
    // cout<<"Attribute B:"<<attributeB<<endl;;
    // descending means that the highest goes to the front. if this is true, then this means that|
    // A and B switch places (as B is higher than A).
    if(isDescending){
        return attributeA < attributeB;
    }
    // ascending means that the lowest goes to the front. if this is true then this means that
    // A and B switch places (as B is lower than A).
    else{
        return attributeA > attributeB;
    }
}

// Swap Places
void swapPlace(string &a, string &b){
    string temp;
    temp = a;  
    a = b;  
    b = temp;
}


// =======================================================================================================================================
// =======================================================================================================================================
// Sorting Functions

// Selection Sort. Split the array into sorted/unsorted. Send smallest/largest value in the unsorted section to end of sorted section.
void selectionSort(string array[], int length, int attributeNum, bool isDescending){
    // i is the last index of the sorted section
    for(int i = 0; i < length; i++){
        // this stores the index of the current minimum/maximum; starts as the end of the sorted section (one after)
        int minMaxValIndex = i;
        // j is the iterator that travels through the unsorted section
        for(int j = i+1; j < length; j++){
            bool compared = compare(array[minMaxValIndex], array[j], attributeNum, isDescending);
            // if a value is greater/less than the min/max, update it
            if(compared){
                minMaxValIndex = j;
            }
        }
        // after the iteration is complete, swap the smallest/largest value with the end
        // of the sorted section
        // cout<<"From: "<<array[i]<< " and "<< array[minMaxValIndex] <<endl;
        swapPlace(array[i],array[minMaxValIndex]);
        // cout<<"To: "<<array[i]<< " and "<< array[minMaxValIndex] <<endl;
    }
    // cout<<"sorted"<<endl;
}

// Bubble Sort
void bubbleSort(string array[], int length, int attributeNum, bool isDescending){
    bool sorting = true;
    while(sorting){
        int currentIndex = 0;
        int corrections = 0;
        while(currentIndex+1 != length){
            // if true, swap places
            bool compared = compare(array[currentIndex], array[currentIndex+1], attributeNum, isDescending);
            if(compared){
                // cout<< "Swapping places."<<endl;
                // cout<<"From: "<<array[currentIndex]<< " and "<< array[currentIndex+1] <<endl;
                swapPlace(array[currentIndex], array[currentIndex+1]);
                // cout<<"To: "<<array[currentIndex]<< " and "<< array[currentIndex+1] <<endl;
                currentIndex++;
                corrections++;
            }
            //  if false, move to the next index
            else{
                // cout<< "Nothing changed."<<endl;
                currentIndex++;
            }
        }
        // if there are no more corrections to make, then the sorting is finished
        if(corrections == 0){
            sorting = false;
        }
    }
    // cout<<"Sorted."<<endl;
}
// =======================================================================================================================================
// =======================================================================================================================================
// Main Program

int main(int argc, char* argv[]){

    // cout<< "sorter "<<getpid()<<": Starting Process..."<<endl;

    // Timer. This adapted from the example in the prompt.
    double t1, t2, cpu_time;
    struct tms tb1, tb2; 
    double ticspersec;
    int i,sum=0;
    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);

    // Variables
    string mainCSV = argv[1];
    int startIndex = atoi(argv[2]);
    int numLines = atoi(argv[3]);
    int sortType = atoi(argv[4]);
    int rootPID = atoi(argv[5]);
    int attributeNum = atoi(argv[6]);
    bool isDescending = true;              // display in descending order (default is true)
    string argument = argv[7];
    if(argument == "ascending"){
        isDescending = false;
    }

    // Create Array. Takes the number of lines from command line.
    string mainArray[numLines];

    // Pipe Variables
    pid_t pid = getpid();
    char pipeName[80];
    string datastreamString = "";
    int fd;

    // Get Lines. Populate the mainArray with strings.
    // cout << "sorter: Pipe " <<pid<<" starting read."<<endl;
    ifstream readFile(mainCSV);
    int lineCounter = 0;
    int currentIndex = 0;
    string line;
	// If the file is readable, then start reading at the startIndex for the designated numLines.
	while(lineCounter < startIndex){
        getline(readFile, line);
        lineCounter++;
    }
    for(int i = 0; i < numLines; i++){
        getline(readFile, line);
        mainArray[currentIndex] = line;
        currentIndex++;
    }
	readFile.close();

    // Sorting. If sortType is 0, bubble sort. If 1, insertion sort.
    if(sortType == 0){
        bubbleSort(mainArray, numLines, attributeNum, isDescending);
        // cout<<"Sorted."<<endl;
    }
    else{
        selectionSort(mainArray, numLines, attributeNum, isDescending);
        // cout<<"Sorted."<<endl;
    }
    
    // Piping. Copy into char array and send through pipe.
    char datastreamChar[60];                      
    sprintf(pipeName, "%d", pid);
    int linesWritten = 0;
    cout<<"sorter: Opening pipe "<<pid<<"..."<<endl;
    fd = open(pipeName, O_WRONLY);
    int indexCounter = 0;
    for(int i = 0; i < numLines; i++){
        string temp = mainArray[i];
        // cout<<pid<<" temp: "<<temp<<endl;
        strncpy(datastreamChar, temp.c_str(), sizeof(datastreamChar));
        // cout<<pid<<" data: "<<datastreamChar<<endl;
        // datastreamChar[sizeof(datastreamChar) - 1] = 0;
        write(fd, datastreamChar, sizeof(datastreamChar));
        indexCounter++;
        linesWritten++;
    }

    // Add Time Report. Send to Merger.
    char timeReport[60];
    t2 = (double) times(&tb2);
    sprintf(timeReport, "sorter %d: Run time was %lf seconds in real time.", pid, (t2 - t1) / ticspersec);
    // fd = open(pipeName, O_WRONLY);
    write(fd, timeReport, sizeof(timeReport)+1);
    close(fd);
    // cout << "sorter: Pipe " <<pid<<" closed."<<endl;

    // Send SIGUSR1 to root before closing.
    kill(rootPID, SIGUSR1);

    exit(0);
}