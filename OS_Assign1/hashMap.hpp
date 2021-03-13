// Hash Map Header
//========================================================================================================
// Header Guard
#ifndef HASHMAP_H
#define HASHMAP_H
//========================================================================================================
//Includes
#include "voter.hpp"


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

	//Add Voter
	int addVoter(Voter* voter);
	// Remove Voter. Returns a Voter* of the voter so that the zipCodeList can also be updated if need be.
	int removeVoter(int inputRIN);

	// Change Vote Status
	int registerVoter(int inputRIN);

};

//========================================================================================================
//Header Guard End
#endif  // HASHMAP_H