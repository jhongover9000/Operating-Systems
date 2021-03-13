// OS Project 2: Merger (CPP)
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
// Signal Handlers
void signalHandler(int sig, siginfo_t *info, void *ucontext){
    pid_t sender_pid = info->si_pid;
    cout<<sender_pid<<endl;
}

// =======================================================================================================================================
// =======================================================================================================================================
// Support Functions (for merging)



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

// =========================================================================================================
// =========================================================================================================
// Main Program
int main(int argc, char* argv[]){

    // Timer. This adapted from the example in the prompt.
    double t1, t2;
    struct tms tb1, tb2; 
    double ticspersec;
    int i,sum=0;
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
    if(argument == "ascending"){
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

    // Open pipe for reading to get sorter PIDs
    sprintf(currentPIDChar, "%d", pid);
    // cout << "merger: Opening merger pipe "<< pid <<endl;
    int mergerfd = open(currentPIDChar, O_RDONLY);
    for(int i = 0; i < numWorkers; i++){
        read(mergerfd, &sorters[i], sizeof(sorters[i]));
    }

    for(int i = 0; i < numWorkers; i++){
        read(mergerfd, &sorterNumLines[i], sizeof(sorterNumLines[i]));
    }
    close(mergerfd);

    // unlink the pipe to delete from filesystem
    unlink(currentPIDChar);
    // cout << "merger: Closed merger pipe." <<endl;

    // Read From Sorter Pipes. For each pipe, it opens twice: once to read the lines, then again to
    // read the time report
    for(int i = 0; i < numWorkers; i++){
        // select sorter based on PID
        pid = sorters[i];
        sprintf(currentPIDChar, "%d", pid);
        char readBuff[60];
        cout<< "merger: Opening pipe "<<pid<<"..."<<endl;
        fd = open(currentPIDChar, O_RDONLY);

        // save starting index (used for merging)
        sorterStartIndex[i] = currentIndex;
        string readString;

        // split the line received by ',', then add to the mainArray at index currentIndex
        for(int j = 0; j < sorterNumLines[i]; j++){
            read(fd, &readBuff, sizeof(readBuff));
            // cout<<readBuff<<endl;
            readString = readBuff;
            mainArray[currentIndex] = readString;
            currentIndex++;
        }
        cout<< "merger: " << currentIndex <<endl;
        // close(fd);

        // // receive time report
        // fd = open(currentPIDChar, O_RDONLY);
        char timeReport[60];
        read(fd, &timeReport, sizeof(timeReport));
        timeReportSubstring = timeReport;
        cout << timeReport << endl;
        timeReportString += timeReportSubstring + ",";
        close(fd);

        // // unlink the pipe to delete from filesystem
        unlink(currentPIDChar);
        // cout<< "merger: Pipe " <<pid<<" closed."<<endl;
    }

    // // Merge. Happens once all sorters have finished. Create a new array and sort into it.
    // // string newArray[totalSize];
    // bool isFinished = false;
    // string newArray[totalSize];
    // int newArrayIndex = 0;
    // int minMaxSorterNumber;
    // // set the current index of all sorters to their respective starting indicies
    // for(int i = 0; i < numWorkers; i++){
    //     sorterCurrentIndex[i] = sorterStartIndex[i];
    // }
    // // merge the sorted arrays
    // for(int i = 0; i < totalSize; i++){
    //     bool firstTouch = true;
    //     // select unfinished sorter section
    //     for(int j = 0; j < numWorkers; j++){
    //         if(firstTouch && sorterCurrentIndex[i] < (sorterStartIndex[i] + sorterNumLines[i])){
    //             // first unfinished array is set as the default
    //             minMaxSorterNumber = i;
    //             firstTouch = false;
    //         }
    //     }
    //     for(int k = 0; k < numWorkers; k++){
    //         // compare minMaxSorter's current index string to that of each other sorter
    //         if(minMaxSorterNumber != k){
    //             bool compared = compare(mainArray[sorterCurrentIndex[minMaxSorterNumber]], mainArray[sorterCurrentIndex[k]], attributeNum, isDescending);
    //             // if there is another that is bigger/smaller, change to that
    //             if(compared){
    //                 minMaxSorterNumber = k;
    //                 // cout<<(minMaxSorterIndex)<<endl;
    //             }
    //         }
    //     }
    //     // update new array
    //     newArray[i] = mainArray[sorterCurrentIndex[minMaxSorterNumber]];
    //     cout<<(newArray[i])<<endl;
    // }
    // cout<<"sorted"<<endl;

    // Write into Output File.
    ofstream writeFile(outputFile);
    for(int i = 0; i < totalSize; i++){
        writeFile << mainArray[i] << endl;
    }

    // Send SIGUSR2 and time report to root before closing.  
    t2 = (double) times(&tb2);
    char timeReport[60];
    // cout<<"merger: creating report"<<endl;
    sprintf(timeReport, "merger %d: Run time was %lf seconds in real time.", getpid(), (t2 - t1) / ticspersec);
    timeReportSubstring = timeReport;
    timeReportString += timeReport;
    // cout<<timeReportString<<endl;

    char datastreamChar[60*(numWorkers+1)];                      
    strcpy(datastreamChar, timeReportString.c_str());

    // cout<<timeReport<<endl;
    // cout<<"merger: send sig"<<endl;
    kill(rootPID, SIGUSR2);
    int rootfd = open(argv[6], O_WRONLY);
    // cout<<"merger: opening"<<endl;
    write(rootfd, datastreamChar, sizeof(datastreamChar)+1);
    close(rootfd);
    // cout<<"merger: closed"<<endl;

    cout<< "merger: Successfully Terminated." <<endl;
    exit(0);
}