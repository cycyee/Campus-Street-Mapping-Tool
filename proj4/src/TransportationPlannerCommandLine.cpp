#include "TransportationPlannerCommandLine.h"
#include <sstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <iomanip>
#include "StringDataSource.h"
#include "GeographicUtils.h"
#include "StringUtils.h"

// Helper function for formatting latitude and longitude into a human-readable string
std::string formatLatLon(double lat, double lon) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2);
    stream << lat << ", " << lon;
    return stream.str();
}

struct CTransportationPlannerCommandLine::SImplementation{
    std::shared_ptr<CDataSource> CmdSrc;
    
    std::shared_ptr<CDataSink> OutSink;
    std::shared_ptr<CDataSink> ErrSink;
    std::shared_ptr<CDataFactory> Results;
    std::shared_ptr<CTransportationPlanner> Planner;

    //assign to defined corresponding variable
    SImplementation(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner)
    : CmdSrc(std::move(cmdsrc)), OutSink(std::move(outsink)), ErrSink(std::move(errsink)), Results(std::move(results)), Planner(std::move(planner)) {
    }
    //assign sources
    bool ProcessCommands() {
        //char vectors to be applied to write()
        std::vector<char> outsinkV;
        std::vector<char> errsinkV;
        char ch;
        std::string line;
        while(CmdSrc->Get(ch)) {//iterate through characters
            line += ch;
            if (line == "help") {
                std::string str ="> "
                                "------------------------------------------------------------------------\n"
                               "help     Display this help menu\n"
                               "exit     Exit the program\n"
                               "count    Output the number of nodes in the map\n"
                               "node     Syntax \"node [0, count)\" \n"
                               "         Will output node ID and Lat/Lon for node\n"
                               "fastest  Syntax \"fastest start end\" \n"
                               "         Calculates the time for fastest path from start to end\n"
                               "shortest Syntax \"shortest start end\" \n"
                               "         Calculates the distance for the shortest path from start to end\n"
                               "save     Saves the last calculated path to file\n"
                               "print    Prints the steps for the last calculated path\n"
                               "> ";             
                for(size_t i = 0; i < str.length(); i++) {//fill vector
                    outsinkV.push_back(str[i]);
                }
                OutSink->Write(outsinkV);//write
                outsinkV.clear();//always clear vector after command is processed
                line = "";
            }
            if (line == "exit") {//exit is the only return true path
                outsinkV.push_back('>');
                outsinkV.push_back(' ');
                OutSink->Write(outsinkV);
                outsinkV.clear();
                return true;
            }
            if (line == "count") {//get nodecount from planner
                auto nodeCount = Planner->NodeCount();
                std::string str = "> "+ std::to_string(nodeCount) + " nodes\n" "> ";
                for (size_t i = 0; i < str.length(); i++) {
                    outsinkV.push_back(str[i]);
                }
                OutSink->Write(outsinkV);
                outsinkV.clear();//clear vect
            }
            if (line == "node") {
                std::string nodeid;
                while(CmdSrc->Get(ch)){//get remaining chars after "node" to see id
                    nodeid += ch;
                }
                if (nodeid == "") {//empty id
                    std::string str = "Invalid node command, see help.\n";
                    for(size_t i = 0; i< str.size(); i++) {
                        errsinkV.push_back(str[i]);
                    }
                    ErrSink->Write(errsinkV);
                    errsinkV.clear();
                    line = "";//reassign line and clear vects
                }
                else {
                    int IntNodeID;
                    try {
                        // Try converting the string to an integer
                        IntNodeID = std::stoi(nodeid);//get node index into int
                        auto node = Planner->SortedNodeByIndex(IntNodeID);//get node to get nodeid
                        std::string output = "> " "Node " + std::to_string(IntNodeID) + ": id = " + std::to_string(node->ID()) +
                                " is at " + SGeographicUtils::ConvertLLToDMS(node->Location()) + "\n> ";
                        for (size_t i = 0; i < output.size(); i++) {
                            outsinkV.push_back(output[i]);
                        }
                        OutSink->Write(outsinkV);//write, reassign  line, clear vect
                        outsinkV.clear();
                        line = "";
                    } 
                    catch (const std::invalid_argument&) {
                        // Conversion failed due to invalid argument
                        std::string err = "Invalid node index.\n";
                        for (size_t i = 0; i < err.size(); i++) {
                            errsinkV.push_back(err[i]);
                        }
                        ErrSink->Write(errsinkV);
                        errsinkV.clear();
                        line = "";
                        return false;
                    } 
                    catch (const std::out_of_range&) {
                        // Conversion failed due to out of range
                        std::string err = "Invalid node index.\n";
                        for (size_t i = 0; i < err.size(); i++) {
                            errsinkV.push_back(err[i]);
                        }
                        ErrSink->Write(errsinkV);
                        errsinkV.clear();
                        line = "";
                        return false;
                    }
                } 
            }

            if (line == "fastest") {//get src dest ids in a big string
                std::string fastids;
                while(CmdSrc->Get(ch)){//remainder of characters after "fastest"
                    fastids += ch;
                }
                if(fastids == "") {
                    //std::cout<<"shortids was empty"<<std::endl;  testing line
                    std::string str = "Invalid shortest command, see help.\n";
                    for(size_t i = 0; i< str.size(); i++) {
                        errsinkV.push_back(str[i]);
                    }
                    ErrSink->Write(errsinkV);//write error message
                    errsinkV.clear();
                    line = "";
                    return false;
                }
                std::vector<std::string> idvect = StringUtils::Split(fastids);//split to separate ids
                std::string Fnodeid1 = idvect[1];//a space being passed to split as the first char means idvect[0] is empty
                std::string Fnodeid2 = idvect[2];
                //std::cout<<"fnode 1: "<<Fnodeid1<<"fnode 2: "<<Fnodeid2<<std::endl;

                try{
                    std::vector <CTransportationPlanner::TTripStep> path;
                    int IntNodeID1 = std::stoull(Fnodeid1);//stoull on nodeids
                    int IntNodeID2 = std::stoull(Fnodeid2);
                    double Time = Planner->FindFastestPath(IntNodeID1, IntNodeID2, path);//get time in hrs as double type
                    int hours = static_cast<int>(Time);//truncate for hours
                    double remainder = Time - hours;//get remainder as fraction
                    remainder *= 60;//convert minutes
                    int minutes = static_cast<int>(remainder);//truncate minutes
                    remainder = (remainder - minutes)*60;
                    int seconds = static_cast<int>(remainder);//get seconds
                    std::string str = "> Fastest path takes";
                    if(hours > 0){
                        str += " " + std::to_string(hours) + " hr";//write values in format
                    }
                    if(minutes > 0) {
                        str += " " + std::to_string(minutes) + " min";
                    }
                    if(seconds > 0) {
                        str += " " + std::to_string(seconds) + " sec";
                    }
                    str += ".\n> ";
                    for(size_t i = 0; i< str.size(); i++) {
                        outsinkV.push_back(str[i]);
                    }
                    OutSink->Write(outsinkV);
                    outsinkV.clear();
                }
                catch(const std::invalid_argument&) {//if something other than ids are passed
                    std::string err = "Invalid shortest parameter, see help.\n";
                    for(size_t i = 0; i< err.size(); i++) {
                        errsinkV.push_back(err[i]);
                    }
                    ErrSink->Write(errsinkV);
                    errsinkV.clear();
                    line = "";
                    return false;
                }
            }

            if (line == "shortest") {//shortest distance
                std::string shortids;
                while(CmdSrc->Get(ch)){//remainder of str is the ids
                    shortids += ch;
                }
                if (shortids == "") {
                    //std::cout<<"shortids was empty"<<std::endl;
                    std::string str = "Invalid shortest command, see help.\n";
                    for(size_t i = 0; i< str.size(); i++) {
                        errsinkV.push_back(str[i]);
                    }
                    ErrSink->Write(errsinkV);
                    errsinkV.clear();
                    line = "";
                    return false;
                }
                std::vector<std::string> idvect = StringUtils::Split(shortids);//split up ids
                std::string nodeid1 = idvect[1];//same as before, there is a space passed in
                std::string nodeid2 = idvect[2];
                //std::cout<<"node 1: "<<nodeid1<<"node 2: "<<nodeid2<<std::endl;
                try{
                    std::vector <CTransportationPlanner::TNodeID> path;
                    int IntNodeID1 = std::stoull(nodeid1);
                    int IntNodeID2 = std::stoull(nodeid2);
                    double distance = Planner->FindShortestPath(IntNodeID1, IntNodeID2, path); //find distance
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << distance;//truncate 
                    std::string dist = ss.str();
                    //std::string str = "> " "Shortest path is " + dist + " mi.\n" "> ";
                    for(size_t i = 0; i< str.size(); i++) {
                        outsinkV.push_back(str[i]);
                    }
                    OutSink->Write(outsinkV);
                    outsinkV.clear();
                    line = "";
                }
                catch(const std::invalid_argument&) {
                    std::string err = "Invalid shortest parameter, see help.\n";
                    for(size_t i = 0; i< err.size(); i++) {
                        errsinkV.push_back(err[i]);
                    }
                    ErrSink->Write(errsinkV);//write error 
                    errsinkV.clear();
                    return false;
                }
            }
            if (line == "save") {
                return true;
            }
            if (line == "print") {
                return true;
            }
        }
        return true;
    }            
};

//constructor
CTransportationPlannerCommandLine::CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner){
    DImplementation = std::make_unique<SImplementation>(cmdsrc, outsink, errsink, results, planner);
}

//destructor
CTransportationPlannerCommandLine::~CTransportationPlannerCommandLine(){
}

//function processcommands
bool CTransportationPlannerCommandLine::ProcessCommands(){
    return DImplementation->ProcessCommands();
}