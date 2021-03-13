// Voter (Definitions)
// Description: Includes class definitions for Voter, VoterNode, VoterNodeList
// Note: A lot of this code here (the doubly-linked lists) is based off of the
// Data Structures textbook, Data Structures and Algorithms in C++.
//========================================================================================================
// Includes
#include "hashMap.hpp"
#include "voter.hpp"
#include <iostream>

//========================================================================================================
// Hash Map

// Constructor
HashMap::HashMap(int size){
    array = new VoterNodeList*[size];
    for(int i = 0; i < size; i++){
        array[i] = new VoterNodeList();
    }
    this->size = size;
    this->totalParticipants = 0;
    this->totalVoted = 0;
}
// Destructor
HashMap::~HashMap(){
    // Delete all of the inner VoteNodeLists and then delete the array
    for(int i = 0; i < size; i++){
        delete array[i];
    }
    delete [] array;
}

// Get Cached Voter
Voter* HashMap::getCachedVoter(){
    return cachedVoter;
}
// Delete Cached Voter
void HashMap::deleteCachedVoter(){
    delete cachedVoter;
    cachedVoter = NULL;
}
// Get Total Participants
int HashMap::getTotalParticipants(){
    return totalParticipants;
}
// Get Total Voted
int HashMap::getTotalVoted(){
    return totalVoted;
}
// Get Percentage of Voted
int HashMap::getPercentVoted(){
    return (getTotalVoted())/(getTotalParticipants())*100;
}

// Hash function to get the hashcode
int HashMap::hash(int inputRIN){
    int* array = new int[6];
    array[0] = inputRIN % 10;
    array[1] = (inputRIN / 10) % 10;
    array[2] = (inputRIN / 100) % 10;
    array[3] = (inputRIN / 1000) % 10;
    array[4] = (inputRIN / 10000) % 10;
    array[5] = (inputRIN / 100000) % 10;
    int sum = 0;
    // add each digit^8
    for(int i = 0; i < 6; i++){
        int mult = 1;
        for(int j = 0; j < 8; j++){
            mult *= array[i];
        }
        sum += mult;
    }
    // free up memory and return hashcode
    delete [] array;
    cout<< (sum % this->size)<<endl;
    return (sum % this->size);
}

// Find voter by RIN (returns VoterNODE. You need to get the voter by calling .getVoter().)
VoterNode* HashMap::findByRIN(int inputRIN){
    int index = hash(inputRIN);
    VoterNode* voterNode = array[index]->findByRIN(inputRIN);
    return voterNode;
}

// Find and print voter details.
void HashMap::findAndPrint(int inputRIN){
    // If voter exists in system, voter information will be printed. Otherwise, will return error.
    if(findByRIN(inputRIN) != NULL){
        cachedVoter = findByRIN(inputRIN)->getVoter();
        cout<< "Details of Voter with RIN " << cachedVoter->getRIN() <<endl<< "Name: " << cachedVoter->getFirstName() <<" "<<
        cachedVoter->getLastName()<< "   ZIP Code: "<< cachedVoter->getZipCode() <<endl;
    }
    else{
        cout<< "Error: The voter with the RIN " << inputRIN << " does not exist." <<endl;
    }


}

//Add Voter
int HashMap::addVoter(Voter* voter){
    int inputRIN = voter->getRIN();
    int index = hash(inputRIN);
    // if voter with RIN already exists, cancel operation and return 3 (for error, need to delete tempVoter in outer program)
    if(array[index]->findByRIN(inputRIN) != NULL){
        cout<< "Error: A voter with RIN " << inputRIN << " already exists. No actions were taken." <<endl;
        return 3;
    }
    // else, create a new voter using the hash
    else{
        array[index]->addBack(voter);
        // increase total participants
        totalParticipants++;
        cout<< "New participant, "<< voter->getFirstName() << " " << voter->getLastName() << ", with RIN " << 
        voter->getRIN() << " and ZIP code " << voter->getZipCode() <<" was added to the system." <<endl;
        return 1;
    }
}

// Remove Voter (different actions taken afterward depending on output)
int HashMap::removeVoter(int inputRIN){
    // if voter exists in system, remove the voterNode by dereferencing. Reference cachedVoter to voter object.
    if(findByRIN(inputRIN) != NULL){
        this->cachedVoter = findByRIN(inputRIN)->getVoter();
        int index = hash(inputRIN);
        array[index]->removeVoterNode(findByRIN(inputRIN));     // dereference VoterNode from the hash chain
        // decrease totalParticipants
        totalParticipants--;
        cout<< "The voter with RIN "<< inputRIN << " has been removed from the system." <<endl;
        // if voter has not voted (status = no), then the cached voter is deleted. else, voter is stored for updating zipCodeList.
        if(!(cachedVoter->getVoteStatus())){
            delete cachedVoter;
            return 1;
        }
        // this signals to the outer program that the ZipCodeList needs to remove the voter as well.
        return 2;
    }
    else{
        cout<< "Error: The voter with the RIN " << inputRIN << " does not exist." <<endl;
        return 3;
    }
}

// Change Vote Status (different actions taken afterward depending on output)
int HashMap::registerVoter(int inputRIN){
    // if voter exists in system, check status of vote
    if(findByRIN(inputRIN) != NULL){
        // if already voted, notify. 
        if(findByRIN(inputRIN)->getVoter()->getVoteStatus()){
            cout<< "Voter with RIN " << inputRIN << " has already voted. No actions were taken." <<endl;
            return 1;
        }
        // else, cache voter, set vote status to true, then update in ZipCodeList (outer program).
        else{
            cachedVoter = findByRIN(inputRIN)->getVoter();
            cachedVoter->setVoteStatus(true);
            // increase total voted
            totalVoted++;
            cout<< "Voter with RIN " << inputRIN << " has now voted. Vote status set to true." <<endl;
            // this signals to the outer program that the ZipCodeList needs to update the voter as well.            
            return 2;
        }
    }
    else{
        cout<< "Error: The voter with the RIN " << inputRIN << " does not exist." <<endl;
        return 3;
    }
}

// void HashMap::printContents(){

// }

//========================================================================================================
// TEST

// int main(){
//     Voter* po = new Voter(100132,"Lastname","Firstname",4291);
//     Voter* se = new Voter(424455,"Lastname2","Firstname2",2111);
//     Voter* sr = new Voter(552164,"Lastname3","Firstname3",4221);
//     Voter* sy = new Voter(322532,"Lastname4","Firstname4",2111);

//     HashMap* hash = new HashMap(10);

//     hash->addVoter(po);
//     hash->addVoter(se);
//     hash->addVoter(sr);
//     hash->addVoter(sy);

//     hash->findAndPrint(100132);
//     hash->removeVoter(322532);
//     hash->findAndPrint(322532);

//     delete hash;

//     cout<<po->getFirstName()<<endl;

//     return EXIT_SUCCESS;
// }