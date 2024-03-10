#include "DijkstraTransportationPlanner.h"
#include <unordered_map>
#include "DijkstraPathRouter.h"
#include "GeographicUtils.h"
#include "BusSystemIndexer.h"
#include "TransportationPlannerConfig.h"
#include <iostream>
#include <string>

struct CDijkstraTransportationPlanner::SImplementation {
    std::shared_ptr < CStreetMap > DStreetMap;
    std::shared_ptr < CBusSystem > DBusSystem;
    std::unordered_map< CStreetMap::TNodeID, CPathRouter::TVertexID> DNodeToVertexID;
    CDijkstraPathRouter DShortestPathRouter;
    CDijkstraPathRouter DFastestPathRouterBike;
    CDijkstraPathRouter DFastestPathRouterWalkBus;
    std::unordered_map <TNodeID, CTransportationPlanner::ETransportationMode> TransportType;

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
                auto NextNodeID = Way->GetNodeID(NodeIndex);//get next node id
                auto PreviousNode = DStreetMap->NodeByID(PreviousNodeID); //get nodes themselves
                auto NextNode = DStreetMap->NodeByID(NextNodeID);
                //distance in miles from the 2 nodes'locations
                double busSpeed = config->DefaultSpeedLimit(); //default init
                auto weightDist = SGeographicUtils::HaversineDistanceInMiles(PreviousNode->Location(), NextNode->Location());
                DShortestPathRouter.AddEdge(PreviousNode->ID(), NextNode->ID(), weightDist, Bidirectional);
                if(Way->HasAttribute("maxspeed")) {
                    busSpeed = std::stod(Way->GetAttribute("maxspeed")); //pull speed limit if its listed
                }
                double weightTimeBus = weightDist/busSpeed; //distance/speed = time, and weightTimeBus will depend on whether there is a posted speed
                double weightTimeWalk = weightDist/config->WalkSpeed();
                double weightTimeBike = weightDist/config->BikeSpeed();
                bool bussable = false;//some default initialization
                if(BusSystemIndexer.RouteBetweenNodeIDs(PreviousNode->ID(), NextNode->ID())) {
                    bussable = true;
                }
                if(bussable == true) {
                    weightTimeBus += (config->BusStopTime()/3600);//check if we need to be walking or not, and busstop time is in seconds, so convert to hours
                    DFastestPathRouterWalkBus.AddEdge(PreviousNode->ID(), NextNode->ID(), weightTimeBus, Bidirectional);
                    std::cout<<"Bus speed: "<<busSpeed<<std::endl;
                    TransportType[PreviousNodeID] = ETransportationMode::Bus;
                }
                else{
                    DFastestPathRouterWalkBus.AddEdge(PreviousNode->ID(), NextNode->ID(), weightTimeWalk, true);
                    TransportType[PreviousNodeID] = ETransportationMode::Walk;
                }
                if(Bikable) {//bikable always bidirectional
                    DFastestPathRouterBike.AddEdge(PreviousNode->ID(), NextNode->ID(), weightTimeBike, true);
                }
                //walkbus will have different time as the weight depending on bussable flag

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
        std::vector <CPathRouter::TVertexID> ShortestPathBike;
        std::vector <CPathRouter::TVertexID> ShortestPathWalkBus;
        auto TimeBike = DFastestPathRouterBike.FindShortestPath(src, dest, ShortestPathBike);
        auto TimeWalkBus = DFastestPathRouterWalkBus.FindShortestPath(src, dest, ShortestPathWalkBus);
        if(TimeBike < TimeWalkBus) {
            for(auto VertexID : ShortestPathBike) {
            auto nodepair = std::make_pair(ETransportationMode::Bike, VertexID);
            path.push_back(nodepair);
            }
            return TimeBike;
        }
        else{
            for(auto VertexID : ShortestPathWalkBus) {
                auto search = TransportType.find(VertexID);
                if(search != TransportType.end()) {
                    auto nodepair = std::make_pair(search->second, VertexID);
                    path.push_back(nodepair);
                }
            }
            return TimeWalkBus;
        }
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