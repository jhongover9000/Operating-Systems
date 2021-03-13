// Bubble Sort
#include <stdio.h>
#include<stdlib.h>
#include<string>
#include<sstream>
#include<iostream>
#include <fstream>

using namespace std;

// void bubbleSort(string array[]){
//     bool sorting = true;
//     while(sorting){
//         int iteratorIndex = 0;

//         int corrections = 0;

//         while(iteratorIndex < array->length()){
//             cout << array->length() << endl;


//             iteratorIndex++;
//         }
//         if(corrections == 0){
//             sorting = false;
//         }
//     }
// }

// Function to in-place trim all spaces in the string such that all words should contain only
// a single space between them. From GeeksforGeeks at https://www.geeksforgeeks.org/remove-extra-spaces-string/.
void removeSpaces(string &str)
{
    // n is length of the original string
    int n = str.length();
 
    // i points to next position to be filled in
    // output string/ j points to next character
    // in the original string
    int i = 0, j = -1;
 
    // flag that sets to true is space is found
    bool spaceFound = false;
 
    // Handles leading spaces
    while (++j < n && str[j] == ' ');
    // read all characters of original string
    while (j < n){
        // if current characters is non-space
        if (str[j] != ' '){
            // remove preceding spaces before dot,
            // comma & question mark
            if ((str[j] == '.' || str[j] == ',' || str[j] == '?') && i - 1 >= 0 && str[i - 1] == ' '){
                str[i - 1] = str[j++];
            }
            else{
                // copy current character at index i
                // and increment both i and j
                str[i++] = str[j++];
            }
            // set space flag to false when any
            // non-space character is found
            spaceFound = false;
        }
        // if current character is a space
        else if (str[j++] == ' '){
            // If space is encountered for the first
            // time after a word, put one space in the
            // output and set space flag to true
            if (!spaceFound){
                str[i++] = ' ';
                spaceFound = true;
            }
        }
    }
    // Remove trailing spaces
    if (i <= 1){
        str.erase(str.begin() + i, str.end());
    }
    else{
        str.erase(str.begin() + i - 1, str.end());
    }

}

// Get Attribute. Uses a string line and the index of the attribute.
double getAttributeDouble(string line, int attributeNum){
    // cout<<"Line: "<<line<<endl;
    string sarray[6];
    string substring;
    removeSpaces(line);
    double i = 0;
    stringstream ss(line);
    for(int i = 0; i < 6; i++){
        getline(ss, substring, ' ');
        sarray[i] = substring;
        // cout<<substring<<endl;
        i++;
    }
    // cout<<"Attribute String:"<<sarray[attributeNum]<<endl;
    double attribute = stoi(sarray[attributeNum]);
    return attribute;
}

// Comparison. Takes string A, the current string, and compares it to B. If true is returned,
// then a swap will take place. False means no swap need occur.
bool compare(string a, string b, int attributeNum, bool isDescending){
    cout << "Starting Compare." <<endl;
    double attributeA = getAttributeDouble(a, attributeNum);
    cout<<"Attribute A:"<<attributeA<<endl;;
    double attributeB = getAttributeDouble(b, attributeNum);
    cout<<"Attribute B:"<<attributeB<<endl;;
    // descending means that the highest goes to the front. if this is true, then this means that|
    // A and B switch places (as B is higher than A).
    if(isDescending){
        return attributeA < attributeB;
    }
    // ascending means that the lowest goes to the front. if this is true then this means that
    // A and B switch places (as B is lower than A).
    else{
        return attributeA > attributeB;
    }
}

// Swap
void swap(string &a, string &b){
    string temp;
    temp = a;  
    a = b;  
    b = temp;
}

// Bubble Sort
void bubbleSort(string array[], int length, int attributeNum, bool isDescending){
    bool sorting = true;
    while(sorting){
        int currentIndex = 0;
        int corrections = 0;
        while(currentIndex+1 != length){
            // if true, swap places
            bool compared = compare(array[currentIndex], array[currentIndex+1], attributeNum, isDescending);
            if(compared){
                // cout<< "Swapping places."<<endl;
                swap(array[currentIndex], array[currentIndex+1]);
                currentIndex++;
                corrections++;
            }
            //  if false, move to the next index
            else{
                // cout<< "Nothing changed."<<endl;
                currentIndex++;
            }
        }
        // if there are no more corrections to make, then the sorting is finished
        if(corrections == 0){
            sorting = false;
        }
    }
}

// Selection Sort. Split the array into sorted/unsorted. Send smallest/largest value in the unsorted section to end of sorted section.
void selectionSort(string array[], int length, int attributeNum, bool isDescending){
    // i is the last index of the sorted section
    for(int i = 0; i < length; i++){
        cout<<i<<endl;
        // this stores the index of the current minimum/maximum; starts as the end of the sorted section (one after)
        int minMaxValIndex = i;
        // j is the iterator that travels through the unsorted section
        for(int j = i+1; j < length; j++){
            bool compared = compare(array[minMaxValIndex], array[j], attributeNum, isDescending);
            // if a value is greater/less than the min/max, update it
            if(compared){
                cout<<"update: "<<j<<endl;
                minMaxValIndex = j;
            }
        }
        // after the iteration is complete, swap the smallest/largest value with the end
        // of the sorted section
        // cout<<"From: "<<array[i]<< " and "<< array[minMaxValIndex] <<endl;
        swap(array[i],array[minMaxValIndex]);
        // cout<<"To: "<<array[i]<< " and "<< array[minMaxValIndex] <<endl;
    }
    cout<<"sorted"<<endl;
}

// 

int main(){
    string stringA = "8006609 WILTOa       JIMENEA      3   18194.30 4007";
    string stringB = "5006780 WILTOb       JIMENEB      7   13564.30 4005";
    string stringC = "1026611 WILTOc       JIMENEC      3   28194.30 4008";
    string stringD = "4026611 WILTOd       JIMENED      5   48194.30 4001";
    string array[4] = {stringA, stringB, stringC, stringD};

    int length = sizeof(array)/sizeof(stringA);
    selectionSort(array, length, 0, true);
    for(int i = 0; i < sizeof(array)/sizeof(stringA); i++){
        cout<<array[i]<<endl;
    }

    exit(0);
}