#include <cstdlib>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string>

#include <iostream>
#include <fstream>
#include <sstream>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>       // mkfifo
#include <sys/times.h>  

using namespace std;


int main(int argc, char* argv[]){

    // Timer. This adapted from the example in the prompt.
    double t1, t2, cpu_time;
    struct tms tb1, tb2; 
    double ticspersec;
    int i,sum=0;
    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);

    // Initialization Arguments
    int totalSize = atoi(argv[1]);                 // total number of lines
    int numWorkers = atoi(argv[2]);                    // number of sorters
    int attributeNum = atoi(argv[3]);              // selection of sorting type (index)  
    bool displayDescending = true;              // display in descending order (default is true)
    string argument = argv[4];
    if(argument == "ascending"){
        displayDescending = false;
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
    // Print Sorters (Confirmation)
    // for(int i = 0; i < numWorkers; i++){
    //     cout << sorters[i] << endl;
    // }
    close(mergerfd);
    // unlink the pipe to delete from filesystem
    unlink(currentPIDChar);
    // cout << "merger: Closed merger pipe." <<endl;

    // Read From Sorter Pipes.
    for(int i = 0; i < numWorkers; i++){
        // select sorter based on PID
        pid = sorters[i];
        sprintf(currentPIDChar, "%d", pid);
        // cout<< "merger: Opening pipe "<<pid<<"..."<<endl;
        fd = open(currentPIDChar, O_RDONLY);
        char readBuff[60*totalSize/2];                
        // cout<< readBuff <<endl;
        // save starting index (used for merging)
        sorterStartIndex[i] = currentIndex;
        // while there is something to read
        // cout<<pid<<endl;
        // int numLines = 0;
        // int bytesRead;
        // for(int i = 0; i < 100; i++){
        //     read(fd, &readBuff, sizeof(readBuff));
        //         // cout << numLines << ", ";
        //         string substring;
        //         substring = readBuff;
        //         // cout<<substring<<endl;
        //         mainArray[currentIndex] = substring;
        //         numLines++;
        //         currentIndex++;
        //         // cout<<substring<<endl;
        // }
        // sorterNumLines[i] = numLines;

        // use the string that was read in order to fill the mainArray
        read(fd, &readBuff, sizeof(readBuff));
        string readString;
        readString = readBuff;
        stringstream readline(readString);
        // cout<<"read string"<<endl;
        int numLines = 0;
        // split the line received by ',', then add to the mainArray at index currentIndex
        while(readline.good()){
            string substring;
            getline(readline, substring, ',');
            mainArray[currentIndex] = substring;
            numLines++;
            currentIndex++;
            // cout<<substring<<endl;
        }
        sorterNumLines[i] = numLines;

        // close pipe after reading and writing into mainArray
        close(fd);
        // unlink the pipe to delete from filesystem
        unlink(currentPIDChar);
        // cout<< "merger: Pipe " <<pid<<" closed."<<endl;
    }


    // Merge. Happens once all sorters have finished. Create a new array and sort into it.
    // string newArray[totalSize];
    // bool isFinished = false;
    // int newArrayIndex = 0;
    // int smallestWorkerIndex;
    // // set the current index of all sorters to their respective starting indicies
    // for(int i = 0; i < numWorkers; i++){
    //     sorterCurrentIndex[i] = sorterStartIndex[i];
    // }
    // // while at least one of the sorters 
    // while(!isFinished){
    //     bool firstTouch = true;
    //     // check for remaining numbers
    //     for(int i = 0; i < numWorkers; i++){
    //         // if the current index is smaller than the initial starting index + number of lines
    //         if(sorterCurrentIndex[i] < (sorterStartIndex[i] + sorterNumLines[i]) ){
    //             isFinished = false;
    //             // first unfinished array is set as the default
    //             if(firstTouch){
    //                 smallestWorkerIndex = i;
    //                 firstTouch = false;
    //             }
    //         }
    //         else{
    //             isFinished = true;
    //         }
    //     }
    //     newArray[newArrayIndex] = sorters[smallestWorkerIndex];
    //     sorterCurrentIndex[smallestWorkerIndex]++;
    // }

    // Write into Output File.
    ofstream writeFile(outputFile);
    for(int i = 0; i < totalSize; i++){
        writeFile << mainArray[i] << endl;
    }

    

    // Send SIGUSR2 and time report to root before closing.  
    t2 = (double) times(&tb2);
    char timeReport[80];
    // cout<<"merger: creating report"<<endl;
    kill(rootPID, SIGUSR2);
    sprintf(timeReport, "merger: Run time was %lf seconds in real time.", (t2 - t1) / ticspersec);
    // cout<<timeReport<<endl;
    int rootfd = open(argv[6], O_WRONLY);
    write(rootfd, timeReport, sizeof(timeReport)+1);
    close(rootfd);

    cout<< "merger: Successfully Terminated." <<endl;
    exit(0);
}