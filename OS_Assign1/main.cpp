// OS Assignment 1
// Description: Includes class definitions for Voter, VoterNode, VoterNodeList
// Note: A lot of this code here (the doubly-linked lists) is based off of the
// Data Structures textbook, Data Structures and Algorithms in C++.
//========================================================================================================
// Includes
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include "structures.hpp"

using namespace std;

//========================================================================================================
// Functions

// Lookup Voter
void findVoter(HashMap* hashMap, int inputRIN){
	hashMap->findAndPrint(inputRIN);
}

// Voter Insertion
void addVoter(HashMap* hashMap, int inputRIN, string inputLastName, string inputFirstName, int inputZIP, int status){
	Voter* temp = new Voter(inputRIN, inputLastName, inputFirstName, inputZIP);
	int returnStatus = hashMap->addVoter(temp, status);
	// if the voter already exists and an error occurs, delete the tempVoter to free space.
	if(returnStatus == 3){
		// free space and dereference pointer
		delete temp;
		temp = NULL;
	}
}

// Voter deletion
void deleteVoter(HashMap* hashMap, ZipCodeList* zipCodeList, int inputRIN){
	// dereference the pointer to the voter (and handle errors)
	int returnStatus = hashMap->removeVoter(inputRIN);
	// in the case of a return status of 2, this means that the voter removed had voted and
	// needs to be dereferenced from the zipCodeList.
	if(returnStatus == 2){
		zipCodeList->removeVoter(hashMap->getCachedVoter());
		// after this, the voter object is deleted, thus freeing space.
		hashMap->deleteCachedVoter();
	}
}

// Register vote for voter
void registerVoter(HashMap* hashMap, ZipCodeList* zipCodeList, int inputRIN, int status){
	// update the vote status accordingly (and handle errors)
	int returnStatus = hashMap->registerVoter(inputRIN, status);
	// in the case of a return status of 2, this means that the voter registered
	// needs to be updated (added) on the zipCodeList.
	if(returnStatus == 2){
		zipCodeList->addVoter(hashMap->getCachedVoter(), status);
	}
}

// Register votes in bulk
void registerBulk(HashMap* hashMap, ZipCodeList* zipCodeList, string fileName, int status){
	ifstream bulkFile(fileName);
	if(bulkFile.good()){
	}
	else{
		cout<< "File " << fileName << " cannot be read. Make sure to type the entire file with its extension"<<
		"and directory, if applicable."<<endl;
	}
	string line;
	// If the file is readable, then read line by line and register voter.
	while(getline(bulkFile, line)){
		int inputRIN = stoi(line);
		// register for voter at RIN
		registerVoter(hashMap, zipCodeList, inputRIN, status);
	}
	bulkFile.close();						//Close file once finished
}

// Total Votes
void totalVoted(HashMap* hashMap){ 
	cout<< "A total of " << hashMap->getTotalVoted() << " voters have voted so far." <<endl;
}

// Percent Voted
void percentVoted(HashMap* hashMap){
	cout<< hashMap->getPercentVoted()<<"% of voters have voted so far."<<endl;
}

// Print by ZIP Code
void displayByZipCode(ZipCodeList* zipCodeList, int zipCode){
	zipCodeList->displayByZipCode(zipCode);
}

// Print All ZIP Codes
void displayAllZipCodes(ZipCodeList* zipCodeList){
	zipCodeList->displayAllZipCodes();
}

//=========================================================================================================================
//Support Functions (because writing them over and over is inefficent)

void invalidInput(){			//for errors
	cout<<"Error: Invalid Input. Please give a valid command and/or input."<<endl;
}

void help(){					//lists available commands
	cout<<"              –––––– Commands Available ––––––"<<endl;
	cout<<"Please use the 'list' command in order to view the active list,"<<endl
	<<"which can be used to find <dequeReference>s for a specific deque."<<endl
	<<"\e[1mPlease note that all commands and references are case sensitive.\e[0m"<<endl<<endl;
	cout<<"help - Display this menu. Clear terminal in process."<<endl<<endl
		<<"l <RIN>"<<endl<<"- Look up the hash-table for a voter with RIN: <RIN>."<<endl<<endl
		<<"i <RIN> <lname> <fname> <zip> "<<endl<<"- Inserts all information for a specific voter whose ID is <RIN>, last and first names are <lname> and <fname>, and resides in <zip>."<<endl<<endl
		<<"d <RIN>"<<endl<<"- Delete the voter with ID <RIN>."<<endl<<endl
		<<"r <RIN>"<<endl<<"- Register the voter with ID <RIN> as having already voted."<<endl<<endl
		<<"bv <fileofkeys>"<<endl<<"- Bulk-vote for all the keys that appear in file '<fileofkeys>.txt'."<<endl<<endl
		<<"v"<<endl<<"- Present the number of people marked as having voted so far."<<endl<<endl
		<<"perc"<<endl<<"- Display the percentage of people who have voted over the number of total poll participants who are in the system."<<endl<<endl
		<<"z <zipcode>"<<endl<<"- Print the number of all poll participants marked as having voted and list their RINs one RIN per line."<<endl<<endl
		<<"o"<<endl<<"- Produce a list of ZIP codes in decreasing order of the number of people who live in each zipcode and have voted."<<endl<<endl
		<<"clear"<<endl<<"- Clear terminal."<<endl<<endl
		<<"exit"<<endl<<"- Terminate the program."<<endl<<endl
		<<"Press ENTER to continue..."<<endl<<endl;
}

// Check if number is positive (for create command)
bool isPositive(string c){
	if(!(stoi(c.substr(c.find(" "))) > 0)){
		invalidInput();
		return false;
	}
	else{
		return true;
	}
}
// Check if RIN is good by checking if there are 6 digits
bool validRIN(int num){
	if(float(num/100000) < 1){
		cout<< "Error: A RIN must consist of 6 digits." <<endl;		
		return false;
	}
	else{
		return true;
	}
}
// Check if ZIP code is good by checking if there are 4 digits
bool validZIP(int num){
	if(float(num/1000) < 1){
		cout<< "Error: A ZIP Code must consist of 4 digits." <<endl;		
		return false;
	}
	else{
		return true;
	}
}

// Checker for one-argument commands
bool oneArgumentCheck(string c){		
	//check if the substr from the front " " and the rear " " are different
	if(c.substr(c.find(" ")) != c.substr(c.rfind(" "))){
		invalidInput();
		return false;
	} 
	else{
		return true;
	}
}

// Checker for four-argument command
bool fourArgumentCheck(string c){
	int count = 0;
	for(int i = 0; i < c.size(); i++){
		if(c.at(i) == ' '){
			count++;
		}
	}
	if(count != 4){
		invalidInput();
		return false;
	}
	else{
		return true;
	}
}


//========================================================================================================
// Main Program

int main(int argc, char* argv[]){
	// Variables for main function
    string command;
	printf("\e[1;1H\e[2J");
	bool running = true;
	help();
	string mainCSV = "";
	int tableSize = 0;

    // Receive inputs
	if(argc != 5){
		cout<<"Usage is './mvote -f <file> -m <size>', where <file> is the file (extension and directory) of"
		<<" registered voters and <size> is an integer designating the size of the hash table."<<endl;
	}
	else{
		for(int i = 1; i < argc-1; i++){
			if(argv[i][0] == '-'){
				if(argv[i][1] == 'm'){
					tableSize = atoi(argv[i+1]);
				}
				if(argv[i][1] == 'f'){
					mainCSV = argv[i+1];
				}
			}
    	}
	}

	// Use inputs to initialize
	HashMap* hashMap = new HashMap(tableSize);
	ZipCodeList* zipCodeList = new ZipCodeList();

	// Check if the input file is readable/exists
	ifstream readFile(mainCSV);
	if(readFile.good()){
	}
	else{
		cout<< "File " << mainCSV << " cannot be read. Make sure to type the entire file with its extension"<<
		"and directory, if applicable."<<endl;
		running = false;
	}
    string line, word;
	// If the file is readable, then read line by line and parse to initialize the hash table.
	while(getline(readFile, line)){
		// initialize the variables to be used in creating Voters
		string* info = new string[4];
		int rin = 0;
		string firstName = "";
		string lastName = "";
		int zipCode = 0;
		// if the first line is empty, skip it
		if(line[0] == ' '){
			continue;
		}
		
		stringstream s(line);
		int i = 0;
		while(getline(s, word, ' ')){
			info[i] = word;
			i++;
		}
		rin = stoi(info[0]);
		firstName = info[1];
		lastName = info[2];
		zipCode = stoi(info[3]);
		// create and add voter to the hash table
		addVoter(hashMap,rin,lastName,firstName,zipCode,0);
		// free space
		delete [] info;
		info = NULL;
	}
	readFile.close();						//Close file

    // Ask user for input & respond accordingly
    getline(cin, command);

	// Main Running Area (until exit is called)
	while(running){

			cout<<"> ";
			getline(cin,command);

			// Display Help
			if(command == "help"){			
				printf("\e[1;1H\e[2J");
				help();
			}

			// Clear Terminal
			else if(command == "clear"){printf("\e[1;1H\e[2J");}

			// Look Up Voter
			else if(command.substr(0,command.find(" ")) == "l"){			// if first argument is "l"
				// if there is 1 argument and it's a number, and if the RIN is valid, find and print voter.
				if(oneArgumentCheck(command) && (isPositive(command))){
					if(validRIN(stoi(command.substr(command.find(" ")+1)))){
						findVoter(hashMap, stoi(command.substr(command.find(" ")+1)));	// find voter using RIN
					}
				}
			}

			// Insert Voter
			else if(command.substr(0,command.find(" ")) == "i"){				// if first argument is "i"
				if(fourArgumentCheck(command)){ 								//check arguments
					// have the four arguments in a single string
					string argumentsOnly = command.substr(command.find(" ")+1);
					string word;
					// initialize the variables to be used in creating Voters
					string* info = new string[4];
					int rin = 0;
					string firstName = "";
					string lastName = "";
					int zipCode = 0;
					int i = 0;
					stringstream s(argumentsOnly);
					while(getline(s, word, ' ')){
						info[i] = word;
						i++;
					}
					
					// check RIN and ZIP
					if( validRIN(stoi(info[0])) ){
						if( validZIP(stoi(info[3])) ){
							rin = stoi(info[0]);
							firstName = info[1];
							lastName = info[2];
							zipCode = stoi(info[3]);
							// create and add voter to the hash table
							addVoter(hashMap,rin,lastName,firstName,zipCode,1);
						}
					}
					else{
						invalidInput();
					}
					// free space
					delete [] info;	
					info = NULL;
				}
			}

			// Delete Voter
			else if(command.substr(0,command.find(" ")) == "d"){			// if first argument is "d"
				if(command != "d"){
					// if there is 1 argument and it's a number, and if the RIN is valid, find and print voter.
					if(oneArgumentCheck(command) && (isPositive(command))){
						if(validRIN(stoi(command.substr(command.find(" "))))){
							deleteVoter(hashMap, zipCodeList, stoi(command.substr(command.find(" "))));	// find voter using RIN
						}
					}
					else{
						invalidInput();
					}
				}
			}

			// Register Voter
			else if(command.substr(0,command.find(" ")) == "r"){			// if first argument is "r"
				if(command != "r"){
					// if there is 1 argument and it's a number, and if the RIN is valid, find and print voter.
					if(oneArgumentCheck(command) && (isPositive(command))){
						if(validRIN(stoi(command.substr(command.find(" "))))){
							registerVoter(hashMap, zipCodeList, stoi(command.substr(command.find(" "))), 1);	// find voter using RIN
						}
					}
					else{
						invalidInput();
					}
				}
			}

			// Bulk Register
			else if(command.substr(0,command.find(" ")) == "bv"){			// if first argument is "bv"
				if(command != "bv"){
					// if there is 1 argument, use that argument to bulk vote.
					if(oneArgumentCheck(command)){
						registerBulk(hashMap, zipCodeList, command.substr(command.find(" ")+1), 0);
					}
					else{
						invalidInput();
					}
					cout<<"Bulk registered."<<endl;
				}
			}

			// Total Votes
			else if(command == "v"){
				totalVoted(hashMap);
			}	

			// Vote Percentage
			else if(command == "perc"){
				percentVoted(hashMap);
			}	

			// Display Zip Code
			else if(command.substr(0,command.find(" ")) == "z"){			// if first argument is "z"
				if(command != "z"){
					// if there is 1 argument and it's a number, and if the RIN is valid, find and print voter.
					if(oneArgumentCheck(command) && (isPositive(command))){
						if(validZIP(stoi(command.substr(command.find(" "))))){
							displayByZipCode(zipCodeList, stoi(command.substr(command.find(" "))));	// find voter using ZIP
						}
					}
				}
			}

			// Display All ZipCodes
			else if(command == "o"){
				displayAllZipCodes(zipCodeList);
				cout<<"Finished Displaying."<<endl;
			}			

			// Clear
			else if(command == "clear"){
				printf("\e[1;1H\e[2J");
			}
			
			// Exit
			else if(command == "exit"){
				running = false;
				delete hashMap;
				cout<<"deleted hashmap"<<endl;
				delete zipCodeList;
				cout<<"Sucessfully Terminated Program."<<endl;
			}

			else{
				cout<<"Error: Invalid Input. Please give a valid command and/or input."<<endl;
				}
	}
	return EXIT_SUCCESS;
};