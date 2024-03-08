#include "DSVReader.h"
#include "DataSource.h"
#include<iostream>

struct CDSVReader::SImplementation{
    std::shared_ptr<CDataSource>src;
    char delimiter;
    char ch;
    SImplementation(std::shared_ptr< CDataSource > Src, char delim){
        delimiter = delim;
        src = Src;
    }
    ~SImplementation() {
    }
    bool ReadRow(std::vector<std::string> &row) {
    row.clear();//clear row before each read
    std::vector<char> buffer; //buffer to hold columns
    bool quoteflag = false;//default quoteflag is false
    if(delimiter == '\"') {//set double quote delimiter to comma separator
        delimiter = ',';
    }
    while(src->Get(ch)) {
        if(quoteflag == false) {
            if(ch == '\"') {
                quoteflag = true;//set quoteflag if double quote encountered
                if(src->Peek(ch) && ch == '\"'){                
                    buffer.push_back('\"');//take quote literally
                    quoteflag = false;
                }
            }
            else if (ch == delimiter) {//these three signify the end of column or even end of row
                if(buffer.empty()){
                    row.push_back("");
                }
                else{
                    std::string column(buffer.begin(), buffer.end());
                    row.push_back(column);
                    buffer.clear();
                }
            }
            else if(ch == '\n') {
                //newline not encased in quotes denotes the end of the row
                ch = buffer.back();//set ch to store where the last read left off
                std::string column(buffer.begin(), buffer.end());
                row.push_back(column);
                buffer.clear();//clear buffer
                return true;
            }
            else{
                buffer.push_back(ch);//push back as normal if regular chars
            }
        }
        else{//quoteflag true
            if(ch == '\"') {//while quoteflag is true, another quote either means to take a quote literally, or the end of a quoted section
                if(src->Peek(ch) && ch == '\"'){                
                    buffer.push_back('\"');//take quote literally
                }
                else{
                    quoteflag = false;
                }
            }
            else if(ch == EOF) {//eof unexpectedly pushes back whole entry
                if(buffer.empty()){
                    row.push_back("");//empty entry in case, since begin() and end() dont work on empty vect
                }
                else{
                std::string column(buffer.begin(), buffer.end());
                row.push_back(column);
                //std::cout<<column<<std::endl;
                buffer.clear();
                }
            }
            // else if (ch == delimiter) {
            //     if(buffer.empty()) {
            //         row.push_back("");
            //     }
            //     std::string column(buffer.begin(), buffer.end());
            //     row.push_back(column);
            //     buffer.clear();
            // }
            else {
                buffer.push_back(ch);
            }
        }
    }
    if(!buffer.empty()) { //remaining contents 
            std::string column(buffer.begin(), buffer.end());
            row.push_back(column);
            //std::cout<<column<<std::endl;
            buffer.clear();     
    }
        return !row.empty();
    }

    bool End() {
        return (!src->Peek(ch) && !src->Get(ch));
    }


};




// Constructor implementation
CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter) {
    DImplementation = std::make_unique<SImplementation>(src, delimiter);
    
}


// Destructor implementation
CDSVReader::~CDSVReader(){

}


// End method implementation
bool CDSVReader::End() const {
    return DImplementation->End();
}

// ReadRow method implementation
bool CDSVReader::ReadRow(std::vector<std::string> &row) {
    return DImplementation->ReadRow(row);
}