// Voter (Definitions)
// Description: Includes class definitions for Voter, VoterNode, VoterNodeList
// Note: A lot of this code here (the doubly-linked lists) is based off of the
// Data Structures textbook, Data Structures and Algorithms in C++.
//========================================================================================================
// Includes
#include "voter.hpp"
#include <iostream>

//========================================================================================================
//Voter Class

// Constructor (Default values are "none", for head and tail of linked list)
Voter::Voter(int rin, string lastName, string firstName, int zipCode){
    this->rin = rin;
    this->firstName = firstName;
    this->lastName = lastName;
    this->zipCode = zipCode;
    this->voteStatus = false;      //default vote status is false
}
// Destructor (Default)
Voter::~Voter(){}

// Get RIN
int Voter::getRIN(){
    return this->rin;
}
// Get ZIP Code
int Voter::getZipCode(){
    return this->zipCode;
}
// Get First Name
string Voter::getFirstName(){
    return this->firstName;
}
// Get Last Name
string Voter::getLastName(){
    return this->lastName;
}
// Get Vote Status
bool Voter::getVoteStatus(){
    return this->voteStatus;
}
// Set Vote Status
void Voter::setVoteStatus(bool status){
    this->voteStatus = status;
}

//========================================================================================================
// VoterNode Class

// Constructor
VoterNode::VoterNode(Voter* voter){
    this->prev = NULL;
    this->next = NULL;
    this->voter = voter;
}
// Alternate Contructor (for head and tail nodes)
VoterNode::VoterNode(){
    this->prev = NULL;
    this->next = NULL;
    this->voter = NULL;
}
// Destructor. Sets all the pointers to null.
VoterNode::~VoterNode(){
    this->prev = NULL;
    this->next = NULL;
    this->voter = NULL;
}

// Get Voter
Voter* VoterNode::getVoter(){
    return voter;
}

//========================================================================================================
// VoterNodeList Class

// Constructor
VoterNodeList::VoterNodeList(){
    this->size = 0;
    this->head = new VoterNode();
    this->tail = new VoterNode();
    this->head->next = tail;
    this->tail->prev = head;
}
// Destructor
VoterNodeList::~VoterNodeList(){
    while(!isEmpty()){
        removeFront();
        // cout<< "removed" <<endl;
        // cout<< "size:" << size <<endl;
    }
    this->head = NULL;
    this->tail = NULL;
    size = 0;
}

// Checks if list is empty.
bool VoterNodeList::isEmpty(){
    return this->size == 0;
}

// Add VoterNode before the a node.
void VoterNodeList::addBefore(VoterNode* node, Voter* voter){
    VoterNode* temp = new VoterNode(voter);
    temp->prev = node->prev;
    temp->next = node;
    node->prev->next = temp;
    node->prev = temp;
    size++;
}
// Add to front of list
void VoterNodeList::addFront(Voter* voter){
    addBefore(this->head->next,voter);
}
// Add to back of list
void VoterNodeList::addBack(Voter* voter){
    addBefore(this->tail,voter);
}

// Remove a specific node (when deleting a voter)
void VoterNodeList::removeVoterNode(VoterNode* node){
    node->prev->next = node->next;
    node->next->prev = node->prev;
    // Dereference pointers (deletion happens separately, in the main program)
    node->next = NULL;
    node->prev = NULL;
    node->voter = NULL;
    size--;
}
// Remove VoterNode from front (for destructing)
// Note: do not use for removing voter or may cause nullpointer error in zipCodeList
void VoterNodeList::removeFront(){
// if the list is not empty, will reassign next and prev pointers then dereference node
    if(!isEmpty()){
        VoterNode* temp = this->head->next;
        this->head->next = temp->next;
        temp->next->prev = this->head;
        temp->prev = NULL;
        temp->next = NULL;
        // if voter exists, delete the object (since this is called twice, once for the hash map and 
        // once for the zipcode list on exit, need to make it an if statement)
        if(temp->voter != NULL){
            delete temp->voter;
            temp->voter = NULL;
        }
        size--;
    }
}

// Get Size
int VoterNodeList::getSize(){
    return size;
}

// Find voter by RIN. Iterate through the list and compare the voter RIN with the input RIN.
VoterNode* VoterNodeList::findByRIN(int inputRIN){
    // start at first item in list
    VoterNode* iterator = this->head->next;
    // will iterate through the list until the tail
    while(iterator->next != NULL){
        if(iterator->getVoter()->getRIN() == inputRIN){
            return iterator;
        }
        // advance iterator
        iterator = iterator->next;
    }
    
    // if nothing is found, then will return NULL to show that no voter exists with that RIN.
    return NULL;
}

// Print Content of all voter IDs in the list (for ZipCodeList)
void VoterNodeList::printContents(){
    // start at first item in list
    VoterNode* iterator = this->head->next;
    // will iterate through the list until the tail
    while(iterator->next != NULL){
        // print out the RIN of the voter and newline
        cout<< iterator->getVoter()->getRIN() <<endl;

        // advance iterator
        iterator = iterator->next;
    }
}
//========================================================================================================
//TEST

// int main(){
//     Voter* po = new Voter(1001,"Lastname","Firstname",4291);
//     Voter* se = new Voter(4244,"Lastname2","Firstname2",2111);
//     printf("\e[1;1H\e[2J");
//     VoterNodeList* list = new VoterNodeList();
//     cout<< list->getSize() <<endl;
//     list->addBack(po);
//     cout<< list->getSize() <<endl;
//     list->addFront(se);
//     cout<< list->getSize() <<endl;
//     cout<<list->findByRIN(1001)->getVoter()->getFirstName()<<endl;
//     list->printContents();
//     delete list;

//     return EXIT_SUCCESS;
// }
