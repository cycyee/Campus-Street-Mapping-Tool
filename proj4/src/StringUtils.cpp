#include "StringUtils.h"
#include<string> 
namespace StringUtils{

std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept{
    if (end < 0) {
        end += str.length(); //convert negative index to string location
    }
    if (start < 0) {
        start += str.length(); //convert negative index to string location
    }
    if (end == 0) {   //if only one parameter passed in, 
        end = str.length();  //python treats it as the end and start automatically becomes 0
    }
    int len = end - start;
    if (len < 0 || len > str.length()) {
        len = 0;
        start = 0;
    } //print nothing if end is before start
    return str.substr(start, len);
    
}

std::string Capitalize(const std::string &str) noexcept{
    std::string newstr;
    if(str.length() == 0) {
        return newstr;
    }
    newstr += str[0];
    for (int i = 1; i < str.length(); i++) {
        newstr += tolower(str[i]);//make sure only first letter is capitalized like in py
    }
    newstr[0] = toupper(newstr[0]);
    return newstr;
}

std::string Upper(const std::string &str) noexcept{
    std::string newstr;
    for (int i = 0; i < str.length(); i++) {
        newstr += toupper(str[i]);
    }
    return newstr;
    
}

std::string Lower(const std::string &str) noexcept{
    std::string newstr;
    for (int i = 0; i < str.length(); i++) {
        newstr += tolower(str[i]);
    }
    return newstr;
    
}

std::string LStrip(const std::string &str) noexcept{
    std::string newstr;
    int count = 0;
    //count number of extra spaces on left side
    for (int i = 0; i < str.length(); i++){
        if (std::isspace(str[i])) {
            count++;
        }
        else{
            break; //stop counting at the first letter
        }
    }
    //print only the part of the string after the extra spaces
    for (int j = count; j < str.length(); j++){
        newstr += str[j];
    }
    return newstr;

}

std::string RStrip(const std::string &str) noexcept{
    std::string newstr;
    int count = 0;
    //count # extra spaces on right side, starting at last str character
    for (int i = str.length()-1; i > 0; i--){
        if(std::isspace(str[i])) {
            count++;
        }
        else{
            break;
        }
    }
    //only print parts of the original string up until the extra spaces
    for (int j = 0; j < str.length()-count; j++) {
        newstr += str[j];
    }
    return newstr;
    
}

std::string Strip(const std::string &str) noexcept{
    std::string newstr1 = RStrip(str);
    std::string newstr2 = LStrip(newstr1);
    return newstr2;
    
}

std::string Center(const std::string &str, int width, char fill) noexcept{
    if (str.length() > width) {
        return str;
    }
    //padding on either side is the extra spaces /2, so that both sides are the same
    int padding = (width - str.length()) / 2;
    std::string paddingstr;
    for (int i = 0; i < padding; i++) {
        paddingstr += fill;
    }
    std::string finalstr = paddingstr + str + paddingstr;
    if (finalstr.length() < width) {
        finalstr += fill; //if there is an odd number of padding characters, place the extra one on the right side
    }
    return finalstr;
}

std::string LJust(const std::string &str, int width, char fill) noexcept{
    std::string newstr; 
    newstr += str;  //start with the string on the left
    //if there is no padding needed to reach the input width return string
    if ( width < str.length()) { 
        return str;
    }
    //add the necessary padding to the right of the string
    int padding = width - str.length();
    for (int i = 0; i < padding; i++) {
        newstr += fill;
    }
    return newstr;
}

std::string RJust(const std::string &str, int width, char fill) noexcept{
    std::string newstr;
    if (width < str.length()) { //same, if no padding needed return str
        return str;
    }
    //add padding before string, so string is on the right, padding on the left
    int padding = width - str.length();
    for (int i = 0; i < padding; i++) {
        newstr += fill;
    }
    newstr += str; //add string after padding
    return newstr;
}

std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept{
    std::string newstr;
    int start = 0; //init start as 0 to begin at start of str
    int found = str.find(old); //find first instance of old
    while(str.find(old, found) != std::string::npos) { //iterate while instances of old are in str ( find returns std::string::npos when theres none of it)
        newstr += str.substr(start, found - start); //add in the parts of the str from start to where old was found
        newstr += rep; //add replacement string
        start = found + old.length(); //update start to skip the old string we replaced
        found = str.find(old, start); // update found by searching for old over the next piece of str
    }
    newstr += str.substr(start); //add the final piece of str in case the str doesnt end with old
    return newstr;
}

std::vector< std::string > Split(const std::string &str, const std::string &splt) noexcept{
    std::vector < std::string > stringVector; //create empty vector
    std::string spltparam = splt; //make modifiable splt copy

    size_t start = 0;
    if(splt.empty()) {
        spltparam = " \t\n"; 
        size_t found = str.find_first_of(spltparam); //find first splt parameter
        if (found == std::string::npos) {
            stringVector.push_back(str);
            return stringVector;
        }
    else{
        if(found == 0) {
            stringVector.push_back("");
            start = found + 1; //skip over the parameter length
            found = str.find_first_of(spltparam, start);
        }
        while (found != std::string::npos) { //while loop to keep finding the splt parameter
            while(str[found] == ' ' || str[found] == '\n' || str[found] == '\t') {
                found++;
            }
            std::string entry = Strip(str.substr(start, found - start));
            stringVector.push_back(entry); //start is 0 at first, so push back everything from the start to the first parameter found
            start = found; //skip over the parameter length
            found = str.find_first_of(spltparam, start + 1); //update found by searching over unsearched part
        }
    }
    if (start < str.length()) {
        std::string entry = Strip(str.substr(start, found - start));
        stringVector.push_back(entry);
    } //add last portion in case str doesn't end with the param
    }
    else{
        size_t found = str.find(splt);
        if(found == std::string::npos) {
            stringVector.push_back(str);
        }
        while(found != std::string::npos) {
            std::string entry = str.substr(start, found - start);
            stringVector.push_back(entry);
            start = found + spltparam.length();
            found = str.find(spltparam, start);
        }
        if (start < str.length()) {
            std::string entry = str.substr(start, found - start);
            stringVector.push_back(entry);
        }
    }
    return stringVector;
}

std::string Join(const std::string &str, const std::vector< std::string > &vect) noexcept{
    std::string newstr;
    if (vect.size() == 0) {
        return newstr;
    }
    else{
    for (size_t i = 0; i < vect.size()-1; i++) {
        newstr += vect[i];
        newstr += str;
    }
    newstr += vect[vect.size()-1]; //to not add the separation on the last one
    return newstr;
    }
}

std::string ExpandTabs(const std::string &str, int tabsize) noexcept{
    std::string newstr;
    size_t count = 0; //running count of characters outputted
    for (size_t i = 0; i < str.length(); i++) {
        //when there is a tab, the spaces needed to the next tabstop is tabsize - count%tabsize
        if (str[i] == '\t') {
            size_t remainder = 0;
            if (tabsize != 0) {
                remainder = tabsize - (count % tabsize); 
            }
             
            for (size_t j = 0; j < remainder; j++) {
                newstr += " ";
                count++; //increment count for outputted spaces
            }
        }
        else{
            newstr += str[i];
            count++;//increment count for outputted non-space characters
        }
    }
    return newstr;  
}

int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept{
    int a = left.length();
    int b = right.length();

    int previous; //vectors to hold the rows of the 2x2 matrix
    std::vector<int> current(b + 1, 0);

    std::string leftstr = left;//make modifiable versions of the strings in case ignorecase = true
    std::string rightstr = right;
    //equalize case
    if(ignorecase == true) {
        leftstr = StringUtils::Lower(left);
        rightstr = StringUtils::Lower(right);
    }

    for (int i = 0; i <= b; i++) {
        current[i] = i;
    }
    for (int j = 1; j <= a; j++) {
        previous = current[0];
        current[0] = j;
        for (int k = 1; k <= b; k++) {
            int temp = current[k];
            if (leftstr[j - 1] == rightstr[k -1]) { //previous diagonal in matrix
                current[k] = previous;
            }
            else{
                current[k] = 1 + std::min(std::min(current[k - 1], previous), current[k]); //minimum of the 3 adjacent places
            }
            previous = temp;
        }
    }
    return current[b]; //final diagonal value is the final distance
}

};