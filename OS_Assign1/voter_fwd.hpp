// Voter (Header)
// Description: Header file. Includes classes for Voter, VoterNode, VoterNodeList.
//========================================================================================================
// Header Guard
#ifndef VOTER_FWD
#define VOTER_FWD

//========================================================================================================
// Includes
#include <string>

using namespace std;

//========================================================================================================
// Voter: stores the basic information of the voter and can return each of those values
class Voter;

//========================================================================================================
// VoterNode: stores a pointer to the voter (entity of hash chain) in doubly-linked list
class VoterNode;

//========================================================================================================
// VoterNodeList: doubly-linked list for voters (hash map chaining & zipcode linking)
class VoterNodeList;

//========================================================================================================
//Header Guard End
#endif  // VOTER_H
