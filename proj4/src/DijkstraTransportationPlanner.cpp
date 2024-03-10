#include "DijkstraTransportationPlanner.h"
#include <unordered_map>
#include "DijkstraPathRouter.h"
#include "GeographicUtils.h"
#include "BusSystemIndexer.h"
#include "TransportationPlannerConfig.h"
#include <iostream>

struct CDijkstraTransportationPlanner::SImplementation {
    std::shared_ptr < CStreetMap > DStreetMap;
    std::shared_ptr < CBusSystem > DBusSystem;
    std::unordered_map< CStreetMap::TNodeID, CPathRouter::TVertexID> DNodeToVertexID;
    CDijkstraPathRouter DShortestPathRouter;
    CDijkstraPathRouter DFastestPathRouterBike;
    CDijkstraPathRouter DFastestPathRouterWalkBus;


    SImplementation(std::shared_ptr<SConfiguration> config)  {
        DStreetMap = config->StreetMap();//pull map and busSystem from config
        DBusSystem = config->BusSystem();
        if(DStreetMap == nullptr || DBusSystem == nullptr) {
            std::cout<<"Huge error"<<std::endl;
        }

        for(size_t Index = 0; Index < DStreetMap->NodeCount(); Index++) { //populate DNodeToVertexID + routers with vertices
            auto Node = DStreetMap->NodeByIndex(Index);
            std::cout<<"Node ID: "<<Node->ID()<<std::endl;
            auto VertexID = DShortestPathRouter.AddVertex(Node->ID());//add in vertex after getting it from streetmap
            DFastestPathRouterBike.AddVertex(Node->ID());//add vertices for fastest pathrouters too
            DFastestPathRouterWalkBus.AddVertex(Node->ID());
            DNodeToVertexID[Node->ID()] = VertexID;//populate DNodeToVertexID
        }

        for(size_t Index = 0; Index < DStreetMap->WayCount(); Index++) { //iterate through ways
            auto Way = DStreetMap->WayByIndex(Index);
            bool Bikable = Way->GetAttribute("bicycle") != "no"; //means that its not bikable/bikable 
            bool Bidirectional = Way->GetAttribute("oneway") != "yes"; //decides if bidir flag is passed
            auto PreviousNodeID = Way->GetNodeID(0);
            CBusSystemIndexer BusSystemIndexer(DBusSystem);
            
            for(size_t NodeIndex = 1; NodeIndex < Way->NodeCount(); NodeIndex++) { //go through nodes in each way
                auto NextNodeID = Way->GetNodeID(NodeIndex);
                //distance in miles from the 2 nodes'locations
                double busSpeed; //default init
                auto weightDist = SGeographicUtils::HaversineDistanceInMiles(DStreetMap->NodeByID(PreviousNodeID)->Location(),DStreetMap->NodeByID(NextNodeID)->Location());
                DShortestPathRouter.AddEdge(DStreetMap->NodeByID(PreviousNodeID)->ID(), DStreetMap->NodeByID(NextNodeID)->ID(), weightDist, Bidirectional);
                if(Way->GetAttribute("maxspeed") != std::string()) {
                    auto busSpeed = std::stod(Way->GetAttribute("maxspeed")); //pull speed limit if its listed
                }
                else{
                    auto busSpeed = config->DefaultSpeedLimit(); //if not, default is speed
                }
                double weightTimeBus = weightDist/busSpeed; //distance/speed = time, and weightTimeBus will depend on whether there is a posted speed
                double weightTimeWalk = weightDist/config->WalkSpeed();
                double weightTimeBike = weightDist/config->BikeSpeed();
                double weightTime;
                bool bussable;//some default initialization
                if(BusSystemIndexer.RouteBetweenNodeIDs(DStreetMap->NodeByID(PreviousNodeID)->ID(), DStreetMap->NodeByID(NextNodeID)->ID())) {
                    bussable = true;
                }
                if(bussable) {
                    auto weightTime = weightTimeBus + (config->BusStopTime()/3600);//check if we need to be walking or not, and busstop time is in seconds, so convert to hours
                }
                else{
                    auto weightTime = weightTimeWalk + (config->BusStopTime()/3600);//convert to hours from seconds
                }
                if(Bikable) {//bikable always bidirectional
                    DFastestPathRouterBike.AddEdge(DStreetMap->NodeByID(PreviousNodeID)->ID(), DStreetMap->NodeByID(NextNodeID)->ID(), weightTimeBike, true);
                }
                //walkbus will have different time as the weight depending on bussable flag
                DFastestPathRouterWalkBus.AddEdge(DStreetMap->NodeByID(PreviousNodeID)->ID(), DStreetMap->NodeByID(NextNodeID)->ID(), weightTime, Bidirectional);

                PreviousNodeID = NextNodeID;//reassign prevnode id
            }

        }
    }

    std::size_t NodeCount() const noexcept {
        return DStreetMap->NodeCount();
    }
    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const { //DNodeToVertexID holds the sorted nodes for lookup
        auto search = DNodeToVertexID.find(index);
        if(search != DNodeToVertexID.end()){
            return nullptr;
        }
        return nullptr;
    }

    double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path) {//calls FindShortestPath by looking up the vertices in DNodetoVertexID
        std::vector <CPathRouter::TVertexID > ShortestPath;//vector for path to be drawn in
        if (DNodeToVertexID.find(src) == DNodeToVertexID.end() || DNodeToVertexID.find(dest) == DNodeToVertexID.end()) {
            // Handle error: Source or destination node not found
            std::cout<<"error here 1"<<std::endl;
            return CPathRouter::NoPathExists; // or any appropriate error code
        }
        if (ShortestPath.empty()) {
            // Handle error: Shortest path not found
            std::cout<<"error here 2"<<std::endl;
            return -1; // or any appropriate error code
        }
        auto SourceVertexID = DNodeToVertexID[src];
        auto DestinationVertexID = DNodeToVertexID[dest];
        auto Distance = DShortestPathRouter.FindShortestPath(SourceVertexID, DestinationVertexID, ShortestPath);
        path.clear();
        for(auto VertexID : ShortestPath) {
            path.push_back(std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(VertexID)));
        }
        return Distance;
    }
    double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path) {
        //divide shortest distances by respective speeds
        std::vector <CPathRouter::TVertexID> ShortestPath;
        auto DistanceBike = DFastestPathRouterBike.FindShortestPath(src, dest, ShortestPath);
        auto DistanceBusWalk = DFastestPathRouterWalkBus.FindShortestPath(src, dest, ShortestPath);
        return std::min(DistanceBike, DistanceBusWalk);
        
    }
    bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const {
        return true;
    }

};

CDijkstraTransportationPlanner::CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config) {
    DImplementation = std::make_unique<SImplementation>(config);
}
CDijkstraTransportationPlanner::~CDijkstraTransportationPlanner(){
}

std::size_t CDijkstraTransportationPlanner::NodeCount() const noexcept{
    return DImplementation->NodeCount();
}

std::shared_ptr<CStreetMap::SNode> CDijkstraTransportationPlanner::SortedNodeByIndex(std::size_t index) const noexcept {
    return DImplementation->SortedNodeByIndex(index);
}

double CDijkstraTransportationPlanner::FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path) {
    return DImplementation->FindShortestPath(src, dest, path);
}

double CDijkstraTransportationPlanner::FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path) {
    return DImplementation->FindFastestPath(src, dest, path);
}

bool CDijkstraTransportationPlanner::GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const {
    return DImplementation->GetPathDescription(path, desc);
}