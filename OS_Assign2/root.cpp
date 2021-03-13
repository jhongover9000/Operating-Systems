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

int signalOneCount = 0;
int signalTwoCount = 0;

// Open Pipe for Time Reports.
pid_t rootpid = getpid();          // to keep track of own process ID
char rootPIDChar[40];
bool signaled = false;
int numWorkers;                    // number of sorters

string timeReportSubstring = "";
string timeReportString = "";


void signalHandlerOne(int sig){
    // printf("signal 1\n");
    signalOneCount++;   
}

void signalHandlerTwo(int sig){
    signal(SIGUSR2, signalHandlerTwo);
    // printf("signal 2\n");
    // wait for merger to finish and send information
    cout<<"root: opening"<<endl;
    int rootfd = open(rootPIDChar, O_RDONLY);
    char timeReports[60*(numWorkers+1)];
    read(rootfd, &timeReports, sizeof(timeReports));
    fcntl(rootfd,O_NONBLOCK);
    timeReportString += timeReports;
    close(rootfd);
    cout<<"root: closed"<<endl;
    unlink(rootPIDChar);
    signalTwoCount++;
}

// Checker for Digits. Ensures that the command line arguments (where applicable) are digits only.
bool isDigits(char* argument) {
  int length = strlen(argument);
  for(int i = 0; i < length; ++i) {
    if(!isdigit(argument[i])){
        return false;
    }
  }
  return true;
}

// ./myhie -i 1batch-1000.csv -k 10 -a 0 -o descending -s output.csv
// execlp("./coord", "coord", "-i", fileArg, "-k", (char)numWorkers, "-r", randomRange, "-a", sortAttribute, "-o", displayDescending, (char *)0);

int main(int argc, char* argv[]){
    signal(SIGUSR1, signalHandlerOne);
    signal(SIGUSR2, signalHandlerTwo);

    // Timer. This adapted from the example in the prompt.
    double t1, t2, cpu_time;
    struct tms tb1, tb2; 
    double ticspersec;
    int i,sum=0;
    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);

	// Fork Variables
	int status;
    pid_t pid;                      // keep track of coord PID

    // Pipe
    int fd[2];
    sprintf(rootPIDChar, "%d", rootpid);
    mkfifo(rootPIDChar, 0777);

    // Initialization Arguments
	string mainCSV;                    // file path for CSV file (input file)
    string outputFile;                 // file path for CSV file (output file)

	bool randomRange = false;          // random range selection (default is false)
    int attributeNum = 0;              // selection of sorting type (index)            
    bool displayDescending = true;     // display in descending order (default is true)
    int totalSize;

    char pidChar[40];
    string reports[numWorkers+1];        // stores the times of processes (sorters + merger)
    

    // Parse Command Line Arguments. Also checks if arguments are valid.
    if(argc < 9){
        cout << "root: Usage is ./myhie -i <inputFile> -k <numWorkers> -a <attributeNum> -o <ascending/descending> -s <outputFile>."
        << " If you want a random range for each sorter, include '-r' as well. Note that alphabetic arguments are case sensitive." << endl;
    }
    else{
        for(int i = 1; i < argc-1; i++){
            // Check arguments
            if(argv[i][0] == '-'){
                // If '-i', check to make sure that the file exists.
                if(argv[i][1] == 'i'){
                    mainCSV = argv[i+1];
                    ifstream readFile(mainCSV);
                    if(readFile.good()){
                        // cout << "file is ok" <<endl;
                    }
                    else{
                        cout<< "root: File " << mainCSV << " cannot be read. Make sure to type the entire file with its extension"<<
                        "and directory, if applicable."<<endl;
                        exit(1);
                    }
                }
                // If '-k', create an array to store the PIDs of the children.
                else if(argv[i][1] == 'k'){
                    if(isDigits(argv[i+1])){
                        numWorkers = atoi(argv[i+1]);
                    }
                    else{
                        cout<< "root: Please give a valid number for the number of workers."<<endl;
                        exit(1);
                    }
                }
                // If '-r', set randomRange to true.
                else if(argv[i][1] == 'r'){
                    randomRange = true;
                }
                // If '-a', set attribute index number.
                else if(argv[i][1] == 'a'){
                    if(isDigits(argv[i+1])){
                        attributeNum = atoi(argv[i+1]);
                    }
                    else{
                        cout<< "root: Please give a valid number for the attribute to be sorted. The"
                        <<" valid attribute numbers are 0, 3, 4, and 5." <<endl;
                        exit(1);
                    }
                    
                }
                // If '-o', check if is "descending" or "ascending"
                else if(argv[i][1] == 'o'){
                    string argument = argv[i+1];
                    if(argument == "descending"){
                        displayDescending = true;
                    }
                    else if(argument == "ascending"){
                        displayDescending = false;
                    }
                    else{ 
                        cout << "root: Please specify 'ascending' or 'descending' in '-o' argument. Case sensitive." <<endl;
                        exit(1);
                    }
                }
                // If '-s', this is the output file path
                else if(argv[i][1] == 's'){
                    outputFile = argv[i+1];
                }
            }
        }

        // Print Arguments
        // cout << "CSV: " << mainCSV << endl;
        // cout << "Workers: " << numWorkers << endl;
        // cout << "Random Range (1 is true): " << randomRange << endl;
        // cout << "Attribute Index: " << attributeNum << endl;
        // cout << "Descending (1 is true): " << displayDescending << endl;
        // cout << "Output File: " << outputFile << endl;
    }

    // Forking. Check for error first.
    if((pid = fork()) < 0){
        cout << "root: Error creating child process (coord)." << endl;
        exit(1);
    }
    // If child, execute new program for 'coord' node.
    if(pid == 0){
        // Pass variables in array (with edited first index) and execute coord.
        strcpy(argv[0],"./coord");
        execvp(argv[0], &argv[0]);
    }



    
    // wait for the coord node to finish its task.
    while(wait(&status) > 0){

    }

    // print times
    printf("\e[1;1H\e[2J");
    stringstream readline(timeReportString);
    while(readline.good()){
            string substring;
            getline(readline, substring, ',');
            if(substring != ""){
                cout<<substring<<endl;
            }
        }
    t2 = (double) times(&tb2);
    printf("root: Total turnaround time was %lf seconds in real time.\n",(t2 - t1) / ticspersec);
    // print signal counts
    cout<< "Times SIGUSR1 was sent: " << signalOneCount <<endl;
    cout<< "Times SIGUSR2 was sent: " << signalTwoCount <<endl;

    cout << "root: Successfully terminated program." <<endl;
    return EXIT_SUCCESS;
}