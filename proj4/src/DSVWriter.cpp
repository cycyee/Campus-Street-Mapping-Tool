#include "DSVWriter.h"
#include "iostream"

struct CDSVWriter::SImplementation {
    std::shared_ptr< CDataSink > sink;
    char delimiter;
    bool quoteall;
    bool rowwritten = false;//false only before the first write
    SImplementation(std::shared_ptr< CDataSink > sink, char delimiter, bool quoteall) : sink(sink), delimiter(delimiter), quoteall(quoteall){
    }
    bool WriteRow(const std::vector<std::string> &row) {
    bool wrappedflag;
    if(quoteall == false) {
        if(rowwritten == true){//prepend newline
            sink->Put('\n');
        }
        for(size_t i = 0; i < row.size() - 1; i++) {//iterate through row and pull out each string
            std::string col = row[i];
            wrappedflag = false;
            for(size_t j = 0; j < col.length(); j++) {
                if(col[j] == '\n' || col[j] == '\"' || col[j] == delimiter) { //check for special char
                    wrappedflag = true;
                }
            }
            if(wrappedflag == true) { //wrap column in quotes
                sink->Put('\"');
                for(size_t r = 0; r < col.length(); r++) {
                    if(col[r] == '\"') {
                        sink->Put('\"');
                    }
                    sink->Put(col[r]);
                }
                sink->Put('\"');
            }
            else if (wrappedflag == false){ //add characters normally
                for(size_t s = 0; s < col.length(); s++) {
                    sink->Put(col[s]);
                }
            }
            if(row.size() > 1) {sink->Put(delimiter);} //only put delimiter if the row has more than 1 column
        }
        
        std::string lastcol = row[row.size() - 1]; //dont add a delimiter on the last column
        if(lastcol.find('\"') != std::string::npos || lastcol.find('\n') != std::string::npos || lastcol.find(delimiter) != std::string::npos) {
            sink->Put('\"');
            for (size_t k = 0; k < lastcol.length(); k++) { //if specuial character found, wrap in quotes
            sink->Put(lastcol[k]);
            }
            sink->Put('\"');
        }
        else{ //add last column in normally, just omitting delimiter
            for (size_t d = 0; d < lastcol.length(); d++) {
                sink->Put(lastcol[d]);
            }
        }
    }
    else if (quoteall == true) {
        if(rowwritten == true){//prepend newline
            sink->Put('\n');
        }
        for(size_t i = 0; i < row.size() - 1; i++) {//iterate through row, wrap each string in quotes
            std::string col = row[i];
            sink->Put('\"');
            for(size_t j = 0; j < col.length(); j++) {   
                if(col[j] == '\"') {
                    sink->Put('\"');
                }           
                sink->Put(col[j]);             
            }
            sink->Put('\"');
            sink->Put(delimiter);//add delimiter for all but the last one
        }
        std::string lastcol = row[row.size() - 1]; //dont add a delimiter on the last column
        sink->Put('\"');//wrap last column too
        for (size_t q = 0; q < lastcol.length(); q++) {
            sink->Put(lastcol[q]);
        }
        sink->Put('\"');
    }
    if(rowwritten == false) {rowwritten = true;} //trip rowwritten flag
    return rowwritten; //return true if the row was written
    }
};

CDSVWriter::CDSVWriter(std::shared_ptr< CDataSink > sink, char delimiter, bool quoteall) {
    DImplementation = std::make_unique<SImplementation>(sink, delimiter, quoteall);
}

CDSVWriter::~CDSVWriter() {
}

bool CDSVWriter::WriteRow(const std::vector<std::string> &row) {
    return DImplementation->WriteRow(row);
}