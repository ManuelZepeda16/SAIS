#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>


//Libraries for measuring execution time
using namespace std;
typedef chrono::milliseconds MSEC;
typedef chrono::high_resolution_clock HRC;


map<int, pair<int, int>> getBuckets(vector<int> T) {
    map<int, int> count; // Count the times a string apear in the "word vector"
    map<int, pair<int, int>> buckets; // Save the value key with it's start and end positons of the buket

    for (int c : T) {// iterates through the elements of "word vector" 
    
        count[c]++; // increments the count associated with a character in the count map
    }

    int start = 0;
    vector<int> keys; // store the unique characters found in the "word vector"
    for (auto countPair : count) {// Traverse the "count" map
    
        keys.push_back(countPair.first); // Add the keys to the key vector
    }
    sort(keys.begin(), keys.end()); // Sord the key vaulues (less to greater)

    for (auto c : keys) {// iterate through the keys
    
        buckets.insert({c, {start, start + count[c]}}); // Insert the keys with it's count (start-end) inside the bucket vector (build the buckets)
        start += count[c];
    }

    return buckets;
}

bool equalSubarrays(vector<int> &T, int start1, int end1, int start2, int end2) { //check whether two subarrays in a given integer vector T are equal
    //T is a reference to the integer vector in which the subarrays are contained
    //start1 and end1 specify the range of the first subarray, where start1 is the starting index (inclusive), and end1 is the ending index (exclusive).
    //Similarly, start2 and end2 specify the range of the second subarray.
    
    if (end1 - start1 != end2 - start2) { //checks whether the lengths of the two subarrays are the same
    
        return false;
    }

    for (int i = start1, j = start2; i < end1 && j < end2; i++, j++) { //iterates through the elements of both subarrays simultaneously
    
        if (T[i] != T[j]) { //checks whether the elements at corresponding positions in the two subarrays are equal
        
            return false;
        }
    }

    return true; //If the loop completes without finding any unequal elements, the function returns true
}

vector<int> sais(vector<int> T) {
    
    vector<string> t(T.size(), "_"); // empty "-" S/L Vector 
    t[T.size() - 1] = "S"; // last S/L vector positon set to "S"
    for (int i = T.size() - 1; i > 0; i--) {// Left Traverse word vector
    
        if (T[i - 1] == T[i]) {// Word vector current possition and next position comparation
        
            t[i - 1] = t[i]; // Set S/L vector next position value equal to current value if both vaue are the same on "word vector"
        }
        else {
        
            t[i - 1] = T[i - 1] < T[i] ? "S" : "L"; // Asign either a "S" or "L" on S/L vector next value. If next position in "word vector" is lower than current positon Assaign it an "S" value otherwise set "L"
        }
    }

    map<int, pair<int, int>> buckets = getBuckets(T);

    map<int, int> count; //Save the count of a character
    vector<int> SA(T.size(), -1); // Save the sufix array
    map<int, int> LMS; // Save begining and ending of the LMS character
    
    int end = -1;
    for (int i = T.size() - 1; i > 0; i--){ // Iterate through the size of the "word" vector in reverse order
    
        if (t[i] == "S" and t[i - 1] == "L") {// Check if a character is LMS
        
            count[T[i]]++; // Increments the count for the character T[i]
            int revoffset = count[T[i]]; // calculates a reverse offset based on the count of the current character
            //This offset helps in determining the position where the current character should be placed in the Suffix Array
            SA[buckets[T[i]].second - revoffset] = i; // It assigns a position in the Suffix Array SA for the current character T[i]
            //The position is determined using information from the buckets data structure, which specifies the 
            //range of positions associated with each character. The revoffset helps adjust the position based 
            //on the count of characters that precede the current character.

            if (end != -1) {// If 'end' is not equal to -1 (indicating that it has a valid value), it means that there was a previous LMS substring
    
                LMS[i] = end; // updates the LMS array to link the current LMS substring at position i
            }
            end = i;
        }
    }

    LMS[T.size() - 1] = T.size() - 1; //Assigns the last character (position T.size() - 1) to be an LMS character
    count.clear(); //clear the 'count' vector

    for (int i = 0; i < T.size(); i++) { //Iterates through the elements of the Suffix Array (SA) to complete the Suffix Array and adjust the positions of L-type characters
    
        if (SA[i] > 0) { //Check if the current position is not the first position in the Suffix Array
        
            if (t[SA[i] - 1] == "L") { //Checks whether the character that precedes the current position in the Suffix Array is an L-type character
            
                int symbol = T[SA[i] - 1]; //Retrieves the character symbol that precedes the current position in the Suffix Array (used to determine the appropriate bucket for the character)
                int offset = count[symbol]; //It retrieves the count of occurrences for the symbol T[SA[i] - 1] from the count map
                //This offset helps determine the final position for the current L-type character in the Suffix Array

                count[symbol] = count[symbol] + 1; //increments the count for the symbol
                SA[buckets[symbol].first + offset] = SA[i] - 1; // calculates the final position for the current L-type character in the Suffix Array
            }
        }
        else if (SA[i] == 0) { //handles the special case when SA[i] is 0. 
        
        //This corresponds to the first position in the Suffix Array
        //it is treated similarly to an L-type character
            if (t[t.size() - 1] == "L") {
            
                int symbol = T[T.size() - 1];
                int offset = count[symbol];
                count[symbol] = count[symbol] + 1;
                SA[buckets[symbol].first + offset] = SA[i] - 1;
            }
        }
    }

    count.clear(); //clear the 'count' vector

    for (int i = T.size() - 1; i > 0; i--) { // iterates through the elements of the string in reverse order, starting from the second-to-last character 
    //It excludes the last character because it has already been processed in a previous part of the algorithm.
    
        if (SA[i] > 0) {  //check if the current position is not the first position in the Suffix Array
        
            if (t[SA[i] - 1] == "S") { //checks whether the character that precedes the current position in the Suffix Array is an S-type character
            
                int symbol = T[SA[i] - 1]; //retrieves the character symbol that precedes the current position in the Suffix Array
                //This symbol is used to determine the appropriate bucket for the character
                
                count[symbol] = count[symbol] + 1; //increments the count of occurrences for the symbol
                int revoffset = count[symbol];  //calculates a reverse offset based on the count of the current symbol
                SA[buckets[symbol].second - revoffset] = SA[i] - 1; //assigns a position in the Suffix Array for the current S-type character
            }
        }
    } 
    vector<int> namesp(T.size(), -1); //store names associated with substrings
    int name = 0;
    int prev = -1;

    for (int i = 0; i < SA.size(); i++) { //iterates through the elements of the Suffix Array
    
        string a = t[SA[i]]; //assigns the character type (S|L) of the current position
        string b;
        if (SA[i] > 0) { //check that the current position is not the first position in the Suffix Array
        
            b = t[SA[i] - 1]; //assigns the character type of the position that precedes the current value
        }
        else if (SA[i] == 0) { //if the current position is the first position in the Suffix Array
        
            b = t[t.size() - 1]; //assigns the character type of the last character
        }

        if (a == "S" && b == "L") { //checks whether the current substring is of the form "SaLb"
        
            if (prev != -1) { //checks if there was a previous substring
            
                if (!equalSubarrays(T, SA[prev], LMS[SA[prev]], SA[i], LMS[SA[i]])) { //checks whether the current substring, 
                //represented by the positions SA[prev] to SA[i], is not equal to the previous one, represented by the positions SA[prev] to SA[i]
                
                    name += 1; //if the substrings are not equal, it increments the name variable by 1.
                }
            }
            prev = i; //keep track of the previous substring.
            namesp[SA[i]] = name; //The name associated with the current substring is assigned to the namesp vector, indicating that this substring has been named.
        }
    }
    vector<int> names; //store the names associated with substrings
    vector<int> SApIdx; //store the indices of the corresponding substrings in the original string T

    for (int i = 0; i < T.size(); i++) {
    //iterates through the namesp vector, which likely contains the names assigned to substrings in a previous step
        
        if (namesp[i] != -1) { //checks if the namesp vector has a non-negative value at the current position
        //If a name has been assigned (namesp[i] is not -1), the code adds the name to the names vector and the index i to the SApIdx vector
        
        //prepare for the final sorting and construction of the suffix array
            names.push_back(namesp[i]);
            SApIdx.push_back(i);
        }
    }

    if (name < names.size() - 1) { //check if all substrings have been sorted 
    
        names = sais(names);
    }

    reverse(names.begin(), names.end()); //After the sorting step, the names vector is reversed

    SA.assign(T.size(), -1); //The SA vector is initialized with a size of T, and all elements are initially set to -1
    count.clear(); //clear the count vector
    
    for (int i = 0; i < names.size(); i++) { //iterate throgh the name vector
    
        int pos = SApIdx[names[i]]; //retrieves the index of the substring from the SApIdx vector based on the current name
        count[T[pos]] = count[T[pos]] + 1; //increments the count of occurrences
        int revoffset = count[T[pos]]; //calculates a reverse offset based on the character count
        SA[buckets[T[pos]].second - revoffset] = pos; //assigns the position in the suffix array for the substring
    }

    count.clear(); //clear the count map

    for (int i = 0; i < T.size(); i++) { //iterates through the elements of the original string 
    
        if (SA[i] > 0) { //checks if the current position  it's not the first position in the Suffix Array
        
            if (t[SA[i] - 1] == "L") { //checks whether the character that precedes the current position in the Suffix Array is an L-type character
            //if true, it indicates that the current character is an S-type character.
                
                int symbol = T[SA[i] - 1]; //retrieves the character symbol that precedes the current position in the Suffix Array
                int offset = count[symbol]; //retrieves the count of occurrences for the symbol
                SA[buckets[symbol].first + offset] = SA[i] - 1; //assigns the position for the current S-type character in the Suffix Array
                count[symbol] = offset + 1; //count for this symbol is then incremented
            }
        }
    }
    count.clear(); //clear the count map
    for (int i = T.size() - 1; i > 0; i--) { //iterates in reverse order through the elements of the original string
    
        if (SA[i] > 0) { //checks if the current position  it's not the first position in the Suffix Array
        
            if (t[SA[i] - 1] == "S") { //checks whether the character that precedes the current position in the Suffix Array is an S-type character
            
                int symbol = T[SA[i] - 1]; //retrieves the character symbol that precedes the current position in the Suffix Array
                count[symbol] = count[symbol] + 1; //increments the count of occurrences for the symbol
                int revoffset = count[symbol]; //calculates a reverse offset based on the count of the current symbol
                SA[buckets[symbol].second - revoffset] = SA[i] - 1; //assigns the position for the current L-type character in the Suffix Array
            }
        }
    }

    return SA; //the constructed Suffix Array SA is returned
}

//Search position of a string
int search(const string& text, const string& pattern, const vector<int>& SA) {
    int left = 0;
    int right = text.length() - 1;
    int first_occurrence = -1;

    //The loop continues until it finds the first occurrence of the pattern or exhausts the search space
    while (left <= right) { // continues as long as the left boundary is less than or equal to the right boundary
        // calculates the middle index
        int mid = (left + right) / 2;
        string suffix = text.substr(SA[mid]); // extracts a suffix from the text based on the index in the suffix array 
        if (suffix.compare(0, pattern.length(), pattern) == 0) { //compares the extracted suffix with the pattern
        // updates first_occurrence to mid to store the current index and narrows the search range to the left side
            first_occurrence = mid;
            right = mid - 1;  
        } else if (pattern < suffix) { //If the pattern is lexicographically less than the suffix, it updates the right boundary
            right = mid - 1;
        } else { //If the pattern is lexicographically greater than the suffix, it updates the left boundary
            left = mid + 1;
        }
    }

    return first_occurrence; //contains the index of the first occurrence of the pattern in the suffix array, or -1 if no occurrence is found.
}

vector<int> allOccurrences(const string& text, const string& pattern, const vector<int>& SA) {
    int firstOccurrence = search(text, pattern, SA);
    if (firstOccurrence == -1) {
        return vector<int>(1, -1);
    }
    int i = firstOccurrence;
    vector<int> occurrences;
    while (text.substr(SA[i]).compare(0, pattern.length(), pattern) == 0) {
        occurrences.push_back(SA[i]);
        i++;
    }
    sort(occurrences.begin(), occurrences.end());
    return occurrences;
}

int main(int num, char* args[]) {

    auto start = HRC::now();

    if (num != 2) {
        std::cout << "<Binary> <text>" << std::endl;
        return 1;
    }

    const char* filename = args[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error... '" << filename << "'" << std::endl;
        return 1;
    }

    std::string text;
    std::string line;

    //Read and format .txt file
   while (getline(file, line)) { 
        for (int i = 0; i < line.length(); i++) { //Traverse the book array
        char c = line.at(i);         
    
        // Check if it is an alphabetical character
        if (( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ) ) {
            text += c;
            }
        }
    }
    file.close();

    std::vector<int> T(text.begin(), text.end());
    T.push_back(0);

    
    vector<int> SA = sais(T);

    for (int i = 0; i < SA.size(); i++) {
      cout << SA[i] << " ";
    }

    cout << endl;

    auto end = HRC::now();
    auto exec_time = chrono::duration_cast<MSEC>(end-start);

    //Print execution time in milliseconds
    cout<< "\nExecution time: " << exec_time.count() << " msec\n\n";

    //Print position of the string to find
    std::string findString = "Homer";
    vector<int> occurrences = allOccurrences(text, findString, SA);

    cout << "The first string occurrence for " << "'" <<findString << "'" << " is on position " << occurrences[0] <<"\n";

    cout << "Total Ocurrences: " <<occurrences.size() << endl;

    for (int i = 0; i < occurrences.size(); i++) {
        cout << "Occurrence " << i + 1 << " at Position: " << occurrences[i] << endl;
    }

    
  return 0;
}
