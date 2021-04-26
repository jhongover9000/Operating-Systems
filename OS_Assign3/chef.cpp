// OS Project 3: Chef (CPP)
// Joseph Hong
// Description:
// ==========================================================================================
// ==========================================================================================
// Includes
#include <cstdlib>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

#include <signal.h>         // forking and process signals
#include <unistd.h>
#include <sys/wait.h>

#include <sys/stat.h>       // mkfifo
#include <fcntl.h>

#include <iostream>         // strings, i/o
#include <fstream>
#include <sstream>
#include <string.h>

#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SEGMENTSIZE sizeof(sem_t)
#define SEGMENTPERM 0666

using namespace std;

// ==========================================================================================
// ==========================================================================================
// Global Variables

// Signal Handling
int signalOneCount = 0;

// ==========================================================================================
// ==========================================================================================
// Functions

// Signal Handler
void signalHandlerOne(int sig){
    // printf("Chef received signal.\n");
    signalOneCount++;   
}

// Get Random Weight. Gets a random value between 0.8 and 1.2 and multiplies the value given.
// Modified from example https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c.
float getWeight(float val){
    float a = 0.4;
    float multiplier = ( (float)rand()/(float)(RAND_MAX) ) * a;
    multiplier += 0.8;
    float randomVal = val * multiplier;
    return randomVal;
}

// Wait for X microseconds. I know that usleep() is deprecated but nanosleep() wasn't working.
// Modified from example https://stackoverflow.com/questions/541784/wait-statement-in-c-component
void takeBreak(int val){
    float a = 0.5;
    float multiplier = ( (float)rand()/(float)(RAND_MAX) ) * a;
    multiplier += 0.5;
    float randomVal = val * multiplier;
    double timeInMicro = randomVal * 1000000;    // 1 second = 1  million microseconds; ignore following decimals
    usleep(timeInMicro);
}

// Logging Actions. Writes into external file.
void writeContent(string file, char charBuf[], sem_t* sem){
    sem_wait(sem);
    time_t mytime;
    mytime = time(NULL);
    char *t = ctime(&mytime);
    if (t[strlen(t)-1] == '\n'){
        t[strlen(t)-1] = '\0';
    }
    ofstream writeFile;
    writeFile.open(file, ios_base::app);
    writeFile << t << " | " << charBuf << endl;
    writeFile.close();
    sem_post(sem);
}

// Kitchen. Contains the total number of salads and the ingredients placed on table.
struct Kitchen {
    int totalSalads;
    float ingredients[3];
    // clear table (set all values to 0)
    void clearTable(){
        for(int i = 0; i < 3; i++){
            this->ingredients[i] = 0;
        }
    }
};

// Records. Updated at the very end before a saladmaker ends & read by chef.
struct Record {
    float saladsCreated[3];
    float onions[3];
    float tomatoes[3];
    float peppers[3];
    float waitTimes[3];
    float totalTimes[3];
};

// ==========================================================================================
// ==========================================================================================
// Main Program
int main(int argc, char* argv[]){

    // Random Seed (For Random Ranges)
    srand(time(NULL));

    signal(SIGUSR1, signalHandlerOne);

	string mainCSV;                     // file path for CSV file (input file)
    string outputFile;                  // file path for CSV file (output file)

    int status;
    pid_t pid;
    pid_t ppid; 


    int saladmakerPIDs[3];              // stores the PIDs of the saladmakers
    sem_t* saladmakerSem[3];
    bool firstTouch[3];
    int targetWeights[3] = {80, 50, 30};
    int totalSalads = 10;

    key_t key;
    Kitchen* kitchen;
    Record* record;
    int dataArray[5];
    int id = 0;
    int rid = 0;
    int err = 0;
    void* mem;
    
    char pidChar[30];
    char identityChar[30];
    char identityCharTwo[30];
    char saladTypeChar[30];
    char totalSaladsChar[30];
    char reportBuff[80];

    int waitTimeMax = 1;                    // maximum waittime of chef

    
    ppid = getpid();
    key = ppid;

    string logFile = "output.txt";
    ofstream writeFile(logFile);
    writeFile <<"";
    writeFile.close();
    
    // Create Shared Memory.
    id = shmget(key, sizeof(Kitchen), IPC_CREAT | 0666);
    cout<< "ID: "<<id<<endl;
    kitchen = (Kitchen*) shmat(id, NULL, 0);

    rid = shmget(key+1, sizeof(Record), IPC_CREAT | 0666);
    cout<< "ID: "<<rid<<endl;
    record = (Record*) shmat(rid, NULL, 0);
    
    // Create Semaphores.
    sprintf(pidChar, "%d", ppid);
    sem_t* ownSem = sem_open(pidChar, O_CREAT, 0666, 1);
    sem_t* weighSem = sem_open("weighing", O_CREAT, 0666, 1);
    sem_t* recordSem = sem_open("recording", O_CREAT, 0666, 1);
    sem_t* logSem = sem_open("logging", O_CREAT, 0666, 1);

    // Create Saladmakers.
    for(int i=0; i<3; i++){
        // select ingredient type saladmaker will have available (0-tomatoes, 1-peppers, 2-onions)
        int saladType = i;
        // arguments for command line (saladmaker). salad type, ppid
        sprintf(saladTypeChar, "%d", saladType);
        sprintf(identityChar, "%d", id);
        sprintf(identityCharTwo, "%d", rid);
        sprintf(totalSaladsChar, "%d", totalSalads);
        // create new process. Check for errors.
        if((pid = fork()) < 0){
            cout << "coord: Error creating child process (saladmaker)." << endl;
            exit(1);
        }
        // if child, execute new program for 'saladmaker'. Check for errors.
        if(pid == 0){
            // salad type, ppid, 
            execlp("./saladmaker", "saladmaker", saladTypeChar, identityChar, identityCharTwo, totalSaladsChar, NULL);
            cout << "coord: Failed to load saladmaker." << endl;
            exit(1);
        }
        // otherwise take the PID and save it
        saladmakerPIDs[i] = pid;
        firstTouch[i] = true;
        sprintf(pidChar, "%d", pid);
        // then create a semaphore for that PID (the 'signal' the chef sends) initialized as 0
        sem_t* sem = sem_open(pidChar, O_CREAT, 0666, 0);
        // sem_wait(sem);
        saladmakerSem[i] = sem;
        sprintf(reportBuff, "Chef %d: Created saladmaker %d.", ppid, pid);
        writeContent(logFile, reportBuff, logSem);
    }
    
    // Place ingredients. This is set to a loop of totalSalads, as one ingredient placement
    // guarantees that one salad will be made.
    for(int i = 0; i < totalSalads; i++){
        // sprintf(reportBuff, "Chef %d: Waiting...", ppid);
        // writeContent(logFile, reportBuff, logSem);
        // cout<<reportBuff<<endl;

        sem_wait(ownSem);

        int randVal = rand()%3;
        sprintf(reportBuff, "Chef %d: Placing ingredients for saladmaker %d.", ppid, saladmakerPIDs[randVal]);
        writeContent(logFile, reportBuff, logSem);
        // set random-weighted ingredients on table, excluding the one that the saladmaker possesses
        for(int i = 0; i < 3; i++){
            if(i == randVal){
                continue;
            }
            float randWeight = getWeight(targetWeights[i]);
            kitchen->ingredients[i] = randWeight;
        }
        
        takeBreak(waitTimeMax);



        sem_post(saladmakerSem[randVal]);

        // randomly select two ingredients based on random number from 0-2
        // if 0 then A and B, afterwards increment semaphore for saladmaker 0
        // else if 1 then B and C, afterwards increment semaphore for saladmaker 1
        // else if 2 then C and A, afterwards increment semaphore for saladmaker 2
        
        // wait designated time before selecting again
        // randomize wait time
        
    }  

    sem_wait(ownSem);
    // afterwards, send signal to processes to end?
    for(int i = 0; i < 3; i++){
        
        sprintf(pidChar,"%d",saladmakerPIDs[i]);
        sprintf(reportBuff, "Chef %d: Terminating saladmaker %d.", ppid, saladmakerPIDs[i]);
        
        // send a signal to wrap it up?
        kill(saladmakerPIDs[i], SIGUSR1);

        sleep(1);
    
        sem_post(saladmakerSem[i]);
    }

    // Logging and Placing Ingredients. While saladmakers are working, place ingredients and
    // log events on outputFile.
    while ((pid = wait(&status)) > 0);

    // Close/Unlink Remaining Semaphores.
    sprintf(pidChar,"%d",ppid);
    sem_close(ownSem);
    sem_close(weighSem);
    sem_close(recordSem);
    sem_close(logSem);
    sem_unlink(pidChar);
    sem_unlink("weighing");
    sem_unlink("recording");
    sem_unlink("logging");
    
    // Give Report.
    sprintf(reportBuff, "Chef %d: Created %d salads.", ppid, kitchen->totalSalads);
    writeContent(logFile, reportBuff, logSem);
    cout << reportBuff <<endl;

    // Detach and Delete Shared Memory.
    shmdt(kitchen);
    shmctl(id, IPC_RMID, NULL);
    shmdt(record);
    shmctl(rid, IPC_RMID, NULL);


    sprintf(reportBuff, "Chef %d: Successfully terminated.", ppid);
    writeContent(logFile, reportBuff, logSem);
    cout << "Successfully Terminated." << endl;
    exit(0);
}




// ==========================================================================================
// ==========================================================================================
// 