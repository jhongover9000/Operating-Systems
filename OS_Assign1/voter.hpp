// Voter (Header)
// Description: Header file. Includes classes for Voter, VoterNode, VoterNodeList.
//========================================================================================================
// Header Guard
#ifndef VOTER_H
#define VOTER_H

//========================================================================================================
// Includes
#include <string>

using namespace std;

//========================================================================================================
// Voter: stores the basic information of the voter and can return each of those values
class Voter{
private:
	int rin;            //RIN
	string firstName;   //first name    
	string lastName;    //last name
	int zipCode;         //zip code
	bool voteStatus;      //vote status

public:
	// Constructor (Default values are "none", for head and tail of linked list)
	Voter(int rin, string lastName, string firstName, int zipCode);
	// Destructor (Default)
	~Voter();

	// Get RIN
	int getRIN();
	// Get ZIP Code
	int getZipCode();
	// Get First Name
	string getFirstName();
	// Get Last Name
	string getLastName();
	// Get Vote Status
	bool getVoteStatus();
	// Set Vote Status
	void setVoteStatus(bool status);

};

//========================================================================================================
// VoterNode: stores a pointer to the voter (entity of hash chain) in doubly-linked list
class VoterNode{
private:
	VoterNode* prev;
	VoterNode* next;
	Voter* voter;
	friend class VoterNodeList;

public:
	// Constructor
	VoterNode(Voter* voter);
	// Alternate Contructor (for head and tail nodes)
	VoterNode();
	// Destructor. Sets all the pointers to null.
	~VoterNode();

	// Get Voter
	Voter* getVoter();
};

//========================================================================================================
// VoterNodeList: doubly-linked list for voters (hash map chaining & zipcode linking)
class VoterNodeList{
private:
	int size;
	VoterNode* head;     	//pointer to front of list
	VoterNode* tail;      	//pointer to end of list

public:
	// Constructor
	VoterNodeList();
	// Destructor
	~VoterNodeList();

	// Checks if list is empty.
	bool isEmpty();

	// Add VoterNode before a node.
	void addBefore(VoterNode* node, Voter* voter);
	// Add to front of list
	void addFront(Voter* voter);
	// Add to back of list
	void addBack(Voter* voter);

	// Remove a specific node (dereferences)
	void removeVoterNode(VoterNode* node);
	// Remove VoterNode from front (for destructing class)
	void removeFront();
	// Remove VoterNode from back (if needed)
	// removeBack();

	// Get Size
	int getSize();
	// Find voter by RIN. Iterate through the list and compare the voter RIN with the input RIN.
	VoterNode* findByRIN(int inputRIN);
	// Print content (IDs) (for ZipCodeList)
	void printContents();

};

//========================================================================================================
//Header Guard End
#endif  // VOTER_H
