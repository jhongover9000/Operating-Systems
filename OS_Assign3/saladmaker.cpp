// OS Project 3: Saladmaker (CPP)
// Joseph Hong
// Description:
// ==========================================================================================
// ==========================================================================================
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
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

// ==========================================================================================
// ==========================================================================================
// Global Variables

bool isWorking = true;

clock_t startTime;
clock_t endTime;


// ==========================================================================================
// ==========================================================================================
// Functions

// Signal Handler
void signalHandlerOne(int sig){
    printf("Saladmaker received signal.\n");
    // isWorking = false;
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



struct Kitchen{
    int totalSalads;

    float ingredients[3];

    float waitTimeOne;
    float waitTimeTwo;
    float waitTimeThree;

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
    double waitTimes[3];
    double totalTimes[3];
};

struct Timer {
    int processCount;
    bool isRunning = false;
    double t1, t2, cpu_time;
    struct tms start_tms, end_tms;
    int i,sum=0;

    clock_t startTimer;
    clock_t endTimer;
    double waitTotal = 0;

    void update(){
        
        // if processCount reaches 2 or more, start timer
        if(!isRunning && processCount >= 2){
            times(&start_tms);
            this->isRunning = true;
        }
        // if processCount drops below 2, stop timer
        else if(isRunning && processCount <= 1){
            times(&end_tms);
            double totalTime = (double)(end_tms.tms_stime - start_tms.tms_stime) / CLOCKS_PER_SEC;
            this->waitTotal += totalTime;
            this->isRunning = false;
        }
    }
};

void incrementCount(Timer* timer, sem_t* sem){

}

void decrementCount(Timer* timer, sem_t* sem){

}

void waitSem(sem_t* sem){
    
    sem_wait(sem);
}

void writeRecords(Record* record, int saladType, int saladsCount, int weights[], int times[], sem_t* sem){
    sem_wait(sem);
    int i = saladType;
    record->saladsCreated[i] = saladsCount;
    record->onions[i] = weights[0];
    record->tomatoes[i] = weights[1];
    record->peppers[i] = weights[2];
    record->waitTimes[i] = times[0];
    record->totalTimes[i] = times[1];
    sem_post(sem);
}

// ==========================================================================================
// ==========================================================================================
// Main Program
int main(int argc, char* argv[]){
    // Set Signal Handler.
    signal(SIGUSR1, signalHandlerOne);

    // Timer. This adapted from the example in last assignment.
    double t1, t2, w1, w2;
    struct tms start_tms, end_tms, startWait, endWait;
    double ticspersec;
    int i,sum=0;
    ticspersec = (double) sysconf(_SC_CLK_TCK);
    double totalWaitTime = 0;

    // Begin Clock for Overall CPU Time.
    startTime = clock();
    
    // Begin Clock for Total Runtime.
    t1 = (double) times(&start_tms);

    // Arguments
    int saladType = atoi(argv[1]);
    int id = atoi(argv[2]);
    int rid = atoi(argv[3]);
    int totalSalads = atoi(argv[4]);
    
    // Temporal Log.
    string logFile = "output.txt";

    // PID Variables
    pid_t pid;
    pid_t ppid;
    pid = getpid();
    ppid = getppid();

    // Character Buffers
    char pidChar[30];
    char reportBuff[80];

    // Salad Variables
    int saladCount = 0;                          // number of salads made
    float currentWeights[3];
    float totalWeights[3];
    float timings[2];

    // Attach Shared Memory Segments.
    Kitchen* kitchen;
    kitchen = (Kitchen*) shmat(id, NULL, 0);

    Record* record;
    record = (Record*) shmat(rid, NULL, 0);

    // Set Target Weight (for ingredient in possession).
    int targetWeight;
    if(saladType == 0){
        targetWeight = 80;
    }
    else if(targetWeight == 1){
        targetWeight = 50;
    }
    else{
        targetWeight = 30;
    }
    
    // Load Semaphores.
    sprintf(pidChar, "%d", pid);
    sem_t* ownSem = sem_open(pidChar, O_CREAT, 0666, 0);
    sprintf(pidChar, "%d", ppid);
    sem_t* chefSem = sem_open(pidChar, O_CREAT, 0666, 1);
    sem_t* weighSem = sem_open("weighing", O_CREAT, 0666, 1);
    sem_t* recordSem = sem_open("recording", O_CREAT, 0666, 1);
    sem_t* logSem = sem_open("logging", O_CREAT, 0666, 1);
    

    sprintf(reportBuff, "Saladmaker %d: Created holding ingredient %d.", pid, saladType);
    writeContent(logFile, reportBuff, logSem);
    
    while(1){
        // set timer start (for waiting for chef)
        w1 = times(&startWait);
        sem_wait(ownSem);
        w2 = times(&endWait);
        totalWaitTime += (w2 - w1)/ticspersec;
        
        if(kitchen->totalSalads == totalSalads){
            // Ending
            sprintf(reportBuff, "Saladmaker %d: Terminating...", pid);
            writeContent(logFile, reportBuff, logSem);
            endTime = clock();
            double cpuTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
            sprintf(reportBuff, "Saladmaker %d: CPU time was %.6f seconds in real time.", pid, cpuTime);
            writeContent(logFile, reportBuff, logSem);
            cout << reportBuff <<endl;


            t2 = (double) times(&end_tms);
            double totalTime = (t2 - t1) / ticspersec;
            sprintf(reportBuff, "Saladmaker %d: Runtime was %.6f seconds in real time.", pid, totalTime);
            writeContent(logFile, reportBuff, logSem);
            cout<<reportBuff<<endl;

            double waitTime = totalTime - cpuTime;
            sprintf(reportBuff, "Saladmaker %d: Total time spent waiting for ingredients was %.6f seconds in real time.", pid, totalWaitTime);
            writeContent(logFile, reportBuff, logSem);
            cout<<reportBuff<<endl;

            
            
            // close and unlink semaphores
            sem_close(ownSem);
            sem_close(chefSem);
            sem_close(weighSem);
            sem_close(recordSem);
            sem_close(logSem);
            sprintf(pidChar, "%d", pid);
            sem_unlink(pidChar);
            sprintf(pidChar, "%d", ppid);
            sem_unlink(pidChar);
            sem_unlink("weighing");
            sem_unlink("recording");
            sem_unlink("logging");

            // detach memory segments
            shmdt(kitchen);
            shmdt(record);

            sem_post(chefSem);

            sprintf(reportBuff, "Saladmaker %d: Ended with %.4fg tomatoes, %.4fg peppers, %.4fg onions used, with a total of %d salads made.",
            pid, totalWeights[0],totalWeights[1],totalWeights[2],saladCount );
            writeContent(logFile, reportBuff, logSem);
            sprintf(reportBuff, "Saladmaker %d: Terminated.", pid);
            writeContent(logFile, reportBuff, logSem);

            
            exit(0);
        }
        else{
            // set timer end add to total wait time using difftime()

            // take ingredients (save weights)
            for(int i = 0; i < 3; i++){
                if(i == saladType){
                    continue;
                }
                else{
                    currentWeights[i] = kitchen->ingredients[i];
                }
            }
            kill(ppid, SIGUSR1);
            sprintf(reportBuff, "Saladmaker %d: Received ingredients.", pid);
            writeContent(logFile, reportBuff, logSem);
            
            // since a salad will be made, increment the number of salads created
            kitchen->totalSalads++;
            saladCount++;

            // post semaphore to let chef know ingredients have been received
            sem_post(chefSem);

        }

        // weigh ingredients
        sprintf(reportBuff, "Saladmaker %d: Weighing and chopping ingredients...", pid);
        writeContent(logFile, reportBuff, logSem);
        // create random weight for ingredient already in possession, then add to total weights
        float randWeight = getWeight(targetWeight);
        currentWeights[saladType] = randWeight;
        for(int i = 0; i < 3; i++){
            totalWeights[i] += currentWeights[i];
        }
        sprintf(reportBuff, "Saladmaker %d: Made salad with %.4fg tomatoes, %.4fg peppers, %.4fg onions.", pid,
        currentWeights[0], currentWeights[1], currentWeights[2]);
        writeContent(logFile, reportBuff, logSem);

    }
    
    // If something goes wrong.
    printf("Saladmaker %d: Error occurred. Exiting irregularly.", pid);
    exit(1);
}