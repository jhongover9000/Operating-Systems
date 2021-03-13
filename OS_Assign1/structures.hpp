// ZipCodeList (Header)
// Description: contains the declaration for the ZipCodeNode, ZipCodeList class.
// Note: A lot of this code here (the doubly-linked lists) is based off of the
// Data Structures textbook, Data Structures and Algorithms in C++.
//========================================================================================================
// Header Guard
#ifndef STRUCTURES_H
#define STRUCTURES_H

//========================================================================================================
// Includes
#include "voter.hpp"
#include <iostream>

using namespace std;

//========================================================================================================
// ZipCodeNode

class ZipCodeNode{
private:    
    int zipCode;
    ZipCodeNode* prev;
	ZipCodeNode* next;
    VoterNodeList* voterList;
    friend class ZipCodeList;

public:
    // Constructor
    ZipCodeNode(int zipCode);
    // Alternate Constructor
    ZipCodeNode();
    // Destructor
    ~ZipCodeNode();

    // Add Voter
    void addVoter(Voter* voter);
    // Remove Voter
    void removeVoter(VoterNode* node);

    // Get ZIP Code
    int getZipCode();

    // Get voterList size
    int getSize();

    // Get VoterNode
    VoterNode* findByRIN(int inputRIN);

    // Print Contents of voterList
    void printContents();


};

//========================================================================================================
// ZipCodeList

class ZipCodeList{
private:    
    ZipCodeNode* head;
    ZipCodeNode* tail;

public:
    // Constructor
    ZipCodeList();
    // Destructor
    ~ZipCodeList();

    // Checks if list is empty.
    bool isEmpty();

    // Add ZipCodeNode before the a node.
    void addBefore(ZipCodeNode* node, int zipCode);
    // Add　new ZipCodeNode to front of list
    void addFront(int zipCode);
    // Add new ZipCodeNode to back of list
    void addBack(int zipCode);
    // Add a Voter to ZipCodeNode voteList (or create new one) when status is changed
    void addVoter(Voter* voter, int status);

    // Remove a specific node (when deleting a zip code)
    void removeZipCodeNode(ZipCodeNode* node);
    // Remove ZipCodeNode from front (for destructing class)
    void removeFront();
    // Remove a Voter
    void removeVoter(Voter* voter);

    // Find ZipCodeNode.
    ZipCodeNode* findByZipCode(int zipCode);

    // Print out  ZipCodeNodes and the number of voters in it
    void displayByZipCode(int zipCode);
    // Print out sorted zip codes and vote number.
    void displayAllZipCodes();

    // Sort the list in ascending order (printed from tail) by changing the pointers
    void bubbleSort();

    // Update the list. Cleans up empty nodes and sorts the new ones. Called after every removed
    // 'yes' voter or changed vote status.
    void update();

};

//========================================================================================================
// Hash Map

class HashMap{
private:
	VoterNodeList** array;     	//array of doubly linked lists to store info
	int size;					//size of hash map (# of indicies)
	int totalParticipants;			//number of participants
	int totalVoted;				//number of people who have voted yes
	Voter* cachedVoter;			//stores the voter who has been recently accessed for status update or removal

public:
	// Constructor
	HashMap(int size);
	// Destructor
	~HashMap();

	// Get Cached Voter
	Voter* getCachedVoter();
	// Delete Cached Voter
	void deleteCachedVoter();
	// Get Total Participants
	int getTotalParticipants();
	// Get Total Voted
	int getTotalVoted();
	// Get Percentage of Voted
	int getPercentVoted();

	// Hash function to get the hashcode
	int hash(int inputRIN);
	// Find voter by RIN (returns VoterNODE. You need to get the voter by calling ->getVoter().)
	VoterNode* findByRIN(int inputRIN);
	// Find and Print Voter Details
	void findAndPrint(int inputRIN);

	//Add Voter (status differentiates initialization and actively running)
	int addVoter(Voter* voter, int status);
	// Remove Voter. Returns a Voter* of the voter so that the zipCodeList can also be updated if need be.
	int removeVoter(int inputRIN);

	// Change Vote Status
	int registerVoter(int inputRIN, int status);

};

//========================================================================================================
//Header Guard End
#endif  // STRUCTURES_H