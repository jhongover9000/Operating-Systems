// =========================================================================================================
// =========================================================================================================
// OS Project 2: Merger (CPP)
// Joseph Hong
// Description: This is the merger node for the overall program. It receives the data from the sorters
// and merges them into a single array before outputting all of this into a text file.
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
    string sarray[6];
    string substring;
    removeSpaces(line);
    stringstream ss(line);
    for(int i = 0; i < 6; i++){
        getline(ss, substring, ' ');
        sarray[i] = substring;
    }
    double attribute = stod(sarray[attributeNum]);
    return attribute;
}

// Comparison. Takes string A, the current string, and compares it to B. If true is returned,
// then a swap will take place. False means no swap need occur.
bool compare(string a, string b, int attributeNum, bool isDescending){
    double attributeA = getAttributeDouble(a, attributeNum);
    double attributeB = getAttributeDouble(b, attributeNum);
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

// =========================================================================================================
// =========================================================================================================
// Main Program
int main(int argc, char* argv[]){

    // Timer. This adapted from the example in the prompt.
    double t1, t2;
    struct tms tb1, tb2; 
    double ticspersec;
    int i,sum = 0;
    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);
    string timeReportSubstring = "";
    string timeReportString = "";   

    // Initialization Arguments
    int totalSize = atoi(argv[1]);                 // total number of lines
    int numWorkers = atoi(argv[2]);                    // number of sorters
    int attributeNum = atoi(argv[3]);              // selection of sorting type (index)  
    bool isDescending = true;              // display in descending order (default is true)
    string argument = argv[4];
    if(argument == "a"){
        isDescending = false;
    }
    string outputFile = argv[5];                 // file path for CSV file (output file)
    int rootPID = atoi(argv[6]);
    string mainArray[totalSize];
    int currentIndex = 0;

    // Piping Variables
	int status;
    int sorters[numWorkers];                 // array to store PIDs of sorters
    int sorterNumLines[numWorkers];
    int sorterStartIndex[numWorkers];
    int sorterCurrentIndex[numWorkers];
    int sorterNum = 0;
    int fd;
    
    char currentPIDChar[40];
    pid_t pid = getpid();
    pid_t ppid = getppid();       // ppid to pass to sorters so that they can send a signal directly to root

    // Pipe with Coord. Get PIDs and numLines for each sorter.
    sprintf(currentPIDChar, "%d", pid);
    int mergerfd = open(currentPIDChar, O_RDONLY);
    for(int i = 0; i < numWorkers; i++){
        read(mergerfd, &sorters[i], sizeof(sorters[i]));
    }
    // get number of lines per sorter
    for(int i = 0; i < numWorkers; i++){
        read(mergerfd, &sorterNumLines[i], sizeof(sorterNumLines[i]));
    }
    close(mergerfd);

    // unlink the pipe to delete from filesystem
    unlink(currentPIDChar);

    // Read From Sorter Pipes. For each pipe, it opens twice: once to read the lines, then again to
    // read the time report
    for(int i = 0; i < numWorkers; i++){
        // select sorter based on PID and open fifo
        pid = sorters[i];
        sprintf(currentPIDChar, "%d", pid);
        char readBuff[60];
        fd = open(currentPIDChar, O_RDONLY);

        // save starting index (used for merging)
        sorterStartIndex[i] = currentIndex;
        string readString;
        int linesWritten = 0;
        // split the line received by ',', then add to the mainArray at index currentIndex
        for(int j = 0; j < sorterNumLines[i]; j++){
            int bytesRead = read(fd, &readBuff, sizeof(readBuff));
            readString = readBuff;
            mainArray[currentIndex] = readString;
            currentIndex++;
        }

        // receive time report
        char timeReport[60];
        int bytesRead = read(fd, &timeReport, sizeof(timeReport));
        timeReportSubstring = timeReport;
        cout << timeReport << endl;
        timeReportString += timeReportSubstring + ",";
        close(fd);

        // unlink the pipe to delete from filesystem
        unlink(currentPIDChar);
    }

    // Merge. Happens once all sorters have finished. Create a new array and sort into it.
    // string newArray[totalSize];
    bool isFinished = false;
    string newArray[totalSize];
    int newArrayIndex = 0;
    int minMaxSorterNumber;
    // set the current index of all sorters to their respective starting indicies
    for(int i = 0; i < numWorkers; i++){
        sorterCurrentIndex[i] = sorterStartIndex[i];
    }
    // merge the sorted arrays
    for(int i = 0; i < totalSize; i++){
        // find unfinished sorter sections
        for(int j = 0; j < numWorkers; j++){
            if(sorterCurrentIndex[j] < (sorterStartIndex[j] + sorterNumLines[j])){
                // first unfinished array is set as the default
                minMaxSorterNumber = j;
                break;
            }
        }
        for(int k = 0; k < numWorkers; k++){
            // compare minMaxSorter's current index string to that of each other sorter
            if(minMaxSorterNumber != k){
                if(sorterCurrentIndex[k] < (sorterStartIndex[k] + sorterNumLines[k])){
                    bool compared = compare(mainArray[sorterCurrentIndex[minMaxSorterNumber]], mainArray[sorterCurrentIndex[k]], attributeNum, isDescending);
                    // if there is another that is bigger/smaller, change to that
                    if(compared){
                        minMaxSorterNumber = k;
                    }
                }
            }
        }
        // update new array
        newArray[i] = mainArray[sorterCurrentIndex[minMaxSorterNumber]];
        sorterCurrentIndex[minMaxSorterNumber]++;
    }

    // Write into Output File.
    ofstream writeFile(outputFile);
    for(int i = 0; i < totalSize; i++){
        writeFile << newArray[i] << endl;
    }

    // Time Reports. Send the time reports received from the sorters to the root.
    t2 = (double) times(&tb2);
    char timeReport[60];
    sprintf(timeReport, "merger %d: Runtime was %lf seconds in real time.", getpid(), (t2 - t1) / ticspersec);
    timeReportSubstring = timeReport;
    timeReportString += timeReport;

    char datastreamChar[60*(numWorkers+1)];                      
    strcpy(datastreamChar, timeReportString.c_str());

    // send SIGUSR2 and send time reports to root before closing. 
    kill(rootPID, SIGUSR2);
    int rootfd = open(argv[6], O_WRONLY);
    int bytesWritten = write(rootfd, datastreamChar, sizeof(datastreamChar)+1);
    close(rootfd);


    exit(0);
}