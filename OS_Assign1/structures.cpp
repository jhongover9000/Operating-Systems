// ZipCodeList (Definitions)
// Description: Includes class definitions for ZipCodeNode, ZipCodeList.
// Note: A lot of this code here (the doubly-linked lists) is based off of the
// Data Structures textbook, Data Structures and Algorithms in C++.
//========================================================================================================
// Includes
#include "structures.hpp"
#include "voter.cpp"

//========================================================================================================
// ZipCodeNode

// Constructor
ZipCodeNode::ZipCodeNode(int zipCode){
        this->zipCode = zipCode;
        this->prev = NULL;
        this->next = NULL;
        this->voterList = new VoterNodeList();
}
// Alternate Constructor
ZipCodeNode::ZipCodeNode(){
        this->prev = NULL;
        this->next = NULL;
}
// Destructor
ZipCodeNode::~ZipCodeNode(){
    this->getZipCode();
    this->prev = NULL;
    this->next = NULL;
    // delete and dereference pointer to voterList if it exists        
    if(voterList != NULL){
        cout<<"deleting voterlist"<<endl;
        delete voterList;
        voterList = NULL;            
    }
}

// Add Voter
void ZipCodeNode::addVoter(Voter* voter){
    this->voterList->addBack(voter);
}
// Remove Voter
void ZipCodeNode::removeVoter(VoterNode* node){
    this->voterList->removeVoterNode(node);
}

// Get ZIP Code
int ZipCodeNode::getZipCode(){
    return zipCode;
}
// Get voterList size
int ZipCodeNode::getSize(){
    return voterList->getSize();
}
// Get VoterNode
VoterNode* ZipCodeNode::findByRIN(int inputRIN){
    return voterList->findByRIN(inputRIN);
}

// Print Contents of voterList
void ZipCodeNode::printContents(){
    cout<< "Number of participants who have voted: " << this->voterList->getSize() <<endl
    << "List of IDs:" <<endl;
    this->voterList->printContents();
}

//========================================================================================================
// ZipCodeList

// Constructor
ZipCodeList::ZipCodeList(){
    this->head = new ZipCodeNode();
    this->tail = new ZipCodeNode();
    this->head->next = this->tail;
    this->tail->prev = this->head;
}
// Destructor
ZipCodeList::~ZipCodeList(){
    while(!isEmpty()){
        cout << this->head->next->getZipCode() << endl;
        removeFront();
    }
}

// Checks if list is empty.
bool ZipCodeList::isEmpty(){
    return this->head->next == this->tail;
}

// Add ZipCodeNode before the a node.
void ZipCodeList::addBefore(ZipCodeNode* node, int zipCode){
    ZipCodeNode* temp = new ZipCodeNode(zipCode);
    temp->prev = node->prev;
    temp->next = node;
    node->prev->next = temp;
    node->prev = temp;
}
// Add new ZipCodeNode to front of list
void ZipCodeList::addFront(int zipCode){
    addBefore(this->head->next,zipCode);
}
// Add new ZipCodeNode to back of list
void ZipCodeList::addBack(int zipCode){
    addBefore(this->tail,zipCode);
}
// Add a Voter to ZipCodeNode voterList (or create new one) when status is changed
void ZipCodeList::addVoter(Voter* voter, int status){
    int zipCode = voter->getZipCode();
    // if the ZipCodeNode doesn't exist, create a new one and add the voter
    if(findByZipCode(zipCode) == NULL){
        addFront(zipCode);
        this->head->next->voterList->addBack(voter);
    }
    else{
        findByZipCode(zipCode)->voterList->addBack(voter);
    }
    // notify user that operation was completed only when not registered in bulk.
    if(status == 1){
        cout<< "Voter was registered to the Zip Code List." <<endl;
    }
    // update list (sort) after addition
    update();
}

// Remove a specific node (when deleting a zip code)
void ZipCodeList::removeZipCodeNode(ZipCodeNode* node){
    node->prev->next = node->next;
    node->next->prev = node->prev;
    delete node;
}
// Remove ZipCodeNode from front (for destructing class)
void ZipCodeList::removeFront(){
// if the list is not empty, will reassign next and prev pointers then delete node at front
    if(!isEmpty()){
        ZipCodeNode* temp = this->head->next;
        this->head->next = temp->next;
        temp->next->prev = head;
        cout<<"deleting temp"<<endl;
        delete temp;
    }
}
// Remove Voter
void ZipCodeList::removeVoter(Voter* voter){
    int zipCode = voter->getZipCode();
    // if voter exists, dereference the VoterNode in the voterList
    if(findByZipCode(zipCode) != NULL){
        findByZipCode(zipCode)->removeVoter(findByZipCode(zipCode)->findByRIN(voter->getRIN()));
        // update list (sort) after removal
        update();
    }
    else{
        cout << "Error: Voter with RIN " << voter->getRIN() << " and ZIP code " << voter->getZipCode()
        << " does not exist." <<endl;
    }
}

// Find ZipCodeNode.
ZipCodeNode* ZipCodeList::findByZipCode(int zipCode){
    // start at first item in list
    ZipCodeNode* iterator = this->head->next;
    // will iterate through the list until the tail
    while(iterator->next != NULL){
        // upon finding the ZipCodeNode* will return ZipCodeNode*
        if(iterator->getZipCode() == zipCode){
            return iterator;
        }
        // advance iterator
        iterator = iterator->next;
    }
    // if nothing is found, then will return NULL to show that nobody voted yes in the zip code.
    return NULL;
}

// Print out ZipCodeNode and the number of voters in it (IDs)
void ZipCodeList::displayByZipCode(int zipCode){
    if(findByZipCode(zipCode) != NULL){
        this->findByZipCode(zipCode)->printContents();  
    }
    else{
        cout<< "There are no voters whose vote status is 'yes' in the ZIP code of "<< zipCode << "." <<endl;
    }
    
}
// Print out sorted zip codes and vote number.
void ZipCodeList::displayAllZipCodes(){
    if(!isEmpty()){
        cout<< "Listing by descending order regarding the number of participants who have voted so far by ZIP code." <<endl;
        // sort the list before printing
        bubbleSort();
        // start at last item in list
        ZipCodeNode* iterator = this->tail->prev;
        // will iterate through the list until the tail
        while(iterator->prev != NULL){ 
            cout<< iterator->getZipCode() <<": "<< iterator->getSize() << " voted"<<endl;
            
            // advance iterator
            iterator = iterator->prev;
        }
    }
}

// Sort the list in ascending order (printed from tail) by changing the pointers
void ZipCodeList::bubbleSort(){
    bool sorting = true;
    while(sorting){
        // start at first item in list, loop throughout the list
        ZipCodeNode* iterator = this->head->next;
        // if the number of corrections is more than 0, will continue sorting
        int corrections = 0;
        // need to stop comparisons before tail
        while(iterator->next != this->tail){
            //compare with adjacent node
            // if larger, swap spots but don't change iterator
            if(iterator->getSize() > iterator->next->getSize()){
                // use third node to switch
                ZipCodeNode* temp = new ZipCodeNode();
                // cout << "surrounding nodes" << endl;
                // change pointers of surrounding nodes
                iterator->prev->next = iterator->next;
                iterator->next->next->prev = iterator;
                // cout << "copy into temp" << endl;
                // copying iterator pointers into temp
                temp->next = iterator->next;
                temp->prev = iterator->prev;
                // cout << "link iterator to next" << endl;
                // linking iterator to iterator->next->next and iterator->next
                iterator->next = temp->next->next;
                iterator->prev = temp->next;
                // cout << "link next to iterator" << endl;
                // linking temp->next (iterator->next) to temp (iterator) and temp->prev (iterator-prev)
                temp->next->next = iterator;
                temp->next->prev = temp->prev;
                // dereference temp
                temp->next = NULL;
                temp->prev = NULL;
                temp->voterList = NULL;
                // increase the number of corrections
                corrections++;
            }
            // if smaller or equal, change iterator to next node
            else{
                iterator = iterator->next;
            }
        }
        // if there are no more corrections to make, the sorting is finished
        if(corrections == 0){
            sorting = false;
        }
    }
}

// Update the list. Cleans up empty nodes and sorts the new ones. Called after every removed 'yes' voter or changed vote status.
void ZipCodeList::update(){
    // start at first element, create a node to store the value to delete
    ZipCodeNode* iterator = this->head->next;
    ZipCodeNode* toDelete;
    // will iterate through the list until the tail
    while(iterator->next != NULL){
        // if the iterator voterList size is 0, advance one node and delete the node prior
        // (the one with size 0)
        if(iterator->getSize() == 0){
            iterator = iterator->next;
            removeZipCodeNode(iterator->prev);
        }
        else{
            // advance iterator
            iterator = iterator->next;
        }
    }
    // sort afterwards
    bubbleSort();
}
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
        if(array[i] != NULL){
            delete array[i];
            array[i] = NULL;
        }
    }
    delete [] array;
    array = NULL;
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
    array = NULL;
    // cout<< (sum % this->size)<<endl;
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
int HashMap::addVoter(Voter* voter, int status){
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
        // if not initializing, print out confirmation.
        if(status == 1){
            cout<< "New participant, "<< voter->getFirstName() << " " << voter->getLastName() << ", with RIN " << 
            voter->getRIN() << " and ZIP code " << voter->getZipCode() <<" was added to the system." <<endl;
        }
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
            cachedVoter = NULL;
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
int HashMap::registerVoter(int inputRIN, int status){
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
            // only if not adding in bulk print the confirmation (errors or attempted overwrites will be displayed though)
            if(status == 1){
                cout<< "Voter with RIN " << inputRIN << " has now voted. Vote status set to true." <<endl;
            }
            // this signals to the outer program that the ZipCodeList needs to update the voter as well.            
            return 2;
        }
    }
    else{
        cout<< "Error: The voter with the RIN " << inputRIN << " does not exist." <<endl;
        return 3;
    }
}