#include "TransportationPlannerCommandLine.h"
#include <sstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <iomanip>
#include "StringDataSource.h"
#include "GeographicUtils.h"


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

    // bool ProcessCommands() {
    //     std::string line;
    //     while (std::getline(CmdSrc->GetStream(), line)) {
    //         std::istringstream iss(line);
    //         std::string command;
    //         iss >> command;

    //         if (command == "FindShortestPath") {
    //             TNodeID src, dest;
    //             iss >> src >> dest;
    //             std::vector<TNodeID> path;
    //             double distance = Planner->FindShortestPath(src, dest, path);
    //             if (distance != CTransportationPlanner::NoPathExists) {
    //                 OutSink->Write("Shortest path distance: " + std::to_string(distance) + " miles\n");
    //                 // Additionally, output the path
    //             } else {
    //                 ErrSink->Write("No path exists between " + std::to_string(src) + " and " + std::to_string(dest) + ".\n");
    //             }
    //         } else if (command == "FindFastestPath") {
    //             // Implement FindFastestPath
    //         } else if (command == "GetPathDescription") {
    //             // Implement GetPathDescription
    //         } else {
    //             ErrSink->Write("Unknown command: " + command + "\n");
    //             return false;
    //         }
    //     }
    //     return true;
    // }
    bool ProcessCommands() {
        std::vector<char> outsinkV;
        std::vector<char> errsinkV;
        std::vector<char> resultsV;
        char ch;
        std::string command;
        std::string nodeid;
        std::string shortids;
        bool nodeflag;
        bool shortflag;
        while(CmdSrc->Get(ch)) {
            if(ch == '\n') {
                break;
            }
            command += ch;
            if (command == "node") {
                while(CmdSrc->Get(ch)){
                    nodeid += ch;
                }
                nodeflag = true;
            }
            if (command == "shortest") {
                while(CmdSrc->Get(ch)){
                    shortids += ch;
                }
                shortflag = true;
            }
        }
        if(nodeflag) {std::cout<<"NODE!"<<nodeid<<std::endl;}
            if (command == "exit") {
                outsinkV.push_back('>');
                outsinkV.push_back(' ');
                OutSink->Write(outsinkV);
                return true; // Exit the loop and finish processing


            } 
            else if (command == "help") {
                std::string str = "> "
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

            } 
            else if (command == "count") {
                auto nodeCount = Planner->NodeCount();
                std::string str = "> " + std::to_string(nodeCount) + " nodes\n" "> ";
                for (size_t i = 0; i < str.length(); i++) {
                    outsinkV.push_back(str[i]);
                }
                OutSink->Write(outsinkV);
            }
            
            
            else if (nodeflag == true) {
                if (nodeid == "") {
                    std::string str = "Invalid node command, see help.\n";
                    for(size_t i = 0; i< str.size(); i++) {
                        errsinkV.push_back(str[i]);
                    }
                    ErrSink->Write(errsinkV);
                    return false;
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
                        return true;
                    } 
                    catch (const std::invalid_argument&) {
                        // Conversion failed due to invalid argument
                        std::string err = "Invalid node index.\n";
                        for (size_t i = 0; i < err.size(); i++) {
                            errsinkV.push_back(err[i]);
                        }
                        ErrSink->Write(errsinkV);
                        return false;
                    } 
                    catch (const std::out_of_range&) {
                        // Conversion failed due to out of range
                        std::string err = "Invalid node index.\n";
                        for (size_t i = 0; i < err.size(); i++) {
                            errsinkV.push_back(err[i]);
                        }
                        ErrSink->Write(errsinkV);
                        return false;
                    }
                } 
            }
            else if (shortflag == true) {
                // Implementation for shortest and fastest should follow a similar pattern
                // to the mock tests, using Planner->FindShortestPath or Planner->FindFastestPath






                Planner->FindShortestPath();
                return true;
            } 

            else if (command == "fastest") {
                
                return true;
            }

            else {
                std::string err = "Unknown command \"" + command + "\", type help for help.\n";
                for (size_t i = 0; i< err.size(); i++){
                    errsinkV.push_back(err[i]);
                }
                ErrSink->Write(errsinkV);
                return false;
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