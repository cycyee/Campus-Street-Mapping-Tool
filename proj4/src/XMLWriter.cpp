#include"XMLWriter.h"
#include<expat.h>
#include"XMLEntity.h"
#include "DataSink.h"
#include<string.h>
#include<cstddef>
#include<stack>

struct CXMLWriter::SImplementation {
    std::shared_ptr< CDataSink > DDataSink;
    std::stack<SXMLEntity> flushstack;
    SImplementation(std::shared_ptr< CDataSink > sink) : DDataSink(sink){
    }
    bool Flush() {
        std::vector<char> temp;
        while(!flushstack.empty()) {
            SXMLEntity entity = flushstack.top();
            temp.push_back('<');
            flushstack.pop();
            for(size_t i = 0; i < entity.DNameData.length(); i++){
                temp.push_back(entity.DNameData[i]);
            }
            temp.push_back('/');
            temp.push_back('>');
            DDataSink->Write(temp);
        }
        return flushstack.empty();
    }
    bool WriteEntity(const SXMLEntity &entity){
        std::string startTag; //temp string to hold stuff to eventually write
        std::vector<char> NameData; //char vector so I can Write to sink
        NameData.clear(); //make sure NameData doesn't have content from last write
        switch (entity.DType) {
        case SXMLEntity::EType::StartElement: {
            flushstack.push(entity);
            startTag = "<" + entity.DNameData; //starting format
            for (const auto &attribute : entity.DAttributes) {
                startTag += " " + attribute.first + "=\"" + attribute.second + "\""; //attribute format
            }
            startTag += ">"; //starting elements only have <> as casing
            for(size_t i = 0; i < startTag.length(); i++) {//pushing into NameData so I can write, Write only takes chars
                NameData.push_back(startTag[i]);
            }
            DDataSink->Write(NameData);
                break;
        }
        case SXMLEntity::EType::CompleteElement: {
            startTag = "<" + entity.DNameData; //starting format
            for (const auto &attribute : entity.DAttributes) { //loop through attributes listed in DAttributes
                startTag+= " " + attribute.first + "=\"" + attribute.second + "\"";
            }
            startTag += "/>"; //format for end
            for(size_t i = 0; i < startTag.length(); i++) {
                NameData.push_back(startTag[i]);
            }
            DDataSink->Write(NameData);
            break;
        }
        case SXMLEntity::EType::EndElement: {
            flushstack.pop();
            startTag = "</" + entity.DNameData + ">"; //add end element formatters
            for(size_t i = 0; i < startTag.length(); i++) {
                NameData.push_back(startTag[i]);
            }
            DDataSink->Write(NameData);          
            break;
        }
        case SXMLEntity::EType::CharData: { //convert different esc characters to their equivalents:
            startTag += entity.DNameData;   //could have used replace, but only saw the piazza too late
            for(size_t i = 0; i < startTag.length(); i++) {
                if(startTag[i] == '&') {    //& -> &amp;
                    NameData.push_back('&');
                    NameData.push_back('a');
                    NameData.push_back('m');
                    NameData.push_back('p');
                    NameData.push_back(';');
                }
                else if(startTag[i] == '<') {//< -> &lt;
                    NameData.push_back('&');
                    NameData.push_back('l');
                    NameData.push_back('t');
                    NameData.push_back(';');
                }
                else if(startTag[i] == '>') {//> -> &gt;
                    NameData.push_back('&');
                    NameData.push_back('g');
                    NameData.push_back('t');
                    NameData.push_back(';');
                }
                else if(startTag[i] == '\"') {//" -> &quot;
                    NameData.push_back('&');
                    NameData.push_back('q');
                    NameData.push_back('u');
                    NameData.push_back('o');
                    NameData.push_back('t');
                    NameData.push_back(';');
                }
                else if(startTag[i] == '\'') {//' -> &apos;
                    NameData.push_back('&');
                    NameData.push_back('a');
                    NameData.push_back('p');
                    NameData.push_back('o');
                    NameData.push_back('s');
                    NameData.push_back(';');
                }
                else{ //add characters as normal
                    NameData.push_back(startTag[i]);
                }
            }
            DDataSink->Write(NameData); 
            break;
        }
        }
        return !NameData.empty();
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr< CDataSink > sink){
    DImplementation = std::make_unique<SImplementation>(sink);
}

CXMLWriter::~CXMLWriter(){
}

bool CXMLWriter::Flush(){
    return DImplementation->Flush();
}

bool CXMLWriter::WriteEntity(const SXMLEntity &entity){
    return DImplementation->WriteEntity(entity);
}
