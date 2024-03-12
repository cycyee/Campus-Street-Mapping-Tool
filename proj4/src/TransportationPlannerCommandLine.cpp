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

    bool ProcessCommands() {
        std::vector<char> outsinkV;
        std::vector<char> errsinkV;
        char ch;
        std::string line;
        while(CmdSrc->Get(ch)) {
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
                for(size_t i = 0; i < str.length(); i++) {
                    outsinkV.push_back(str[i]);
                }
                OutSink->Write(outsinkV);
                outsinkV.clear();
                line = "";
            }
            if (line == "exit") {
                outsinkV.push_back('>');
                outsinkV.push_back(' ');
                OutSink->Write(outsinkV);
                outsinkV.clear();
                return true;
            }
            if (line == "count") {
                auto nodeCount = Planner->NodeCount();
                std::string str = "> "+ std::to_string(nodeCount) + " nodes\n" "> ";
                for (size_t i = 0; i < str.length(); i++) {
                    outsinkV.push_back(str[i]);
                }
                OutSink->Write(outsinkV);
                outsinkV.clear();
            }
            if (line == "node") {
                std::string nodeid;
                while(CmdSrc->Get(ch)){
                    nodeid += ch;
                }
                if (nodeid == "") {
                    std::string str = "Invalid node command, see help.\n";
                    for(size_t i = 0; i< str.size(); i++) {
                        errsinkV.push_back(str[i]);
                    }
                    ErrSink->Write(errsinkV);
                    errsinkV.clear();
                    line = "";
                }
                else {
                    int IntNodeID;
                    try {
                        // Try converting the string to an integer
                        IntNodeID = std::stoi(nodeid);
                        auto node = Planner->SortedNodeByIndex(IntNodeID);
                        std::string output = "> " "Node " + std::to_string(IntNodeID) + ": id = " + std::to_string(node->ID()) +
                                " is at " + SGeographicUtils::ConvertLLToDMS(node->Location()) + "\n> ";
                        for (size_t i = 0; i < output.size(); i++) {
                            outsinkV.push_back(output[i]);
                        }
                        OutSink->Write(outsinkV);
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

            if (line == "fastest") {
                std::string fastids;
                while(CmdSrc->Get(ch)){
                    fastids += ch;
                }
                if(fastids == "") {
                    std::cout<<"shortids was empty"<<std::endl;
                    std::string str = "Invalid shortest command, see help.\n";
                    for(size_t i = 0; i< str.size(); i++) {
                        errsinkV.push_back(str[i]);
                    }
                    ErrSink->Write(errsinkV);
                    errsinkV.clear();
                    line = "";
                    return false;
                }
                std::vector<std::string> idvect = StringUtils::Split(fastids);
                std::string Fnodeid1 = idvect[1];
                std::string Fnodeid2 = idvect[2];
                std::cout<<"fnode 1: "<<Fnodeid1<<"fnode 2: "<<Fnodeid2<<std::endl;

                try{
                    std::vector <CTransportationPlanner::TTripStep> path;
                    int IntNodeID1 = std::stoull(Fnodeid1);
                    int IntNodeID2 = std::stoull(Fnodeid2);
                    double Time = Planner->FindFastestPath(IntNodeID1, IntNodeID2, path);
                    int hours = static_cast<int>(Time);
                    double remainder = Time - hours;
                    remainder *= 60;
                    int minutes = static_cast<int>(remainder);
                    remainder = (remainder - minutes)*60;
                    int seconds = static_cast<int>(remainder);
                    std::string str = "> Fastest path takes";
                    if(hours > 0){
                        str += " " + std::to_string(hours) + " hr";
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
                catch(const std::invalid_argument&) {
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

            if (line == "shortest") {
                std::string shortids;
                while(CmdSrc->Get(ch)){
                    shortids += ch;
                }
                if (shortids == "") {
                    std::cout<<"shortids was empty"<<std::endl;
                    std::string str = "Invalid shortest command, see help.\n";
                    for(size_t i = 0; i< str.size(); i++) {
                        errsinkV.push_back(str[i]);
                    }
                    ErrSink->Write(errsinkV);
                    errsinkV.clear();
                    line = "";
                    return false;
                }
                std::vector<std::string> idvect = StringUtils::Split(shortids);
                std::string nodeid1 = idvect[1];
                std::string nodeid2 = idvect[2];
                std::cout<<"node 1: "<<nodeid1<<"node 2: "<<nodeid2<<std::endl;
                try{
                    std::vector <CTransportationPlanner::TNodeID> path;
                    int IntNodeID1 = std::stoull(nodeid1);
                    int IntNodeID2 = std::stoull(nodeid2);
                    double distance = Planner->FindShortestPath(IntNodeID1, IntNodeID2, path);
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << distance;
                    std::string dist = ss.str();
                    std::string str = "> " "Shortest path is " + dist + " mi.\n" "> ";
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
                    ErrSink->Write(errsinkV);
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