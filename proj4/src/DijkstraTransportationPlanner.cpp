#include "DijkstraTransportationPlanner.h"
#include <unordered_map>
#include "DijkstraPathRouter.h"
#include "GeographicUtils.h"
#include "BusSystemIndexer.h"
#include "TransportationPlannerConfig.h"
#include "OpenStreetMap.h"
#include <iostream>
#include <string>
#include <algorithm>

struct CDijkstraTransportationPlanner::SImplementation {
    std::shared_ptr < CStreetMap > DStreetMap;
    std::shared_ptr < CBusSystem > DBusSystem;

    std::unordered_map< CStreetMap::TNodeID, CPathRouter::TVertexID> DNodeToVertexID;
    CDijkstraPathRouter DShortestPathRouter;
    CDijkstraPathRouter DFastestPathRouterBike;
    CDijkstraPathRouter DFastestPathRouterWalkBus;

    std::unordered_map <CStreetMap::TNodeID, CTransportationPlanner::ETransportationMode> TransportType;
    std::unordered_map <CStreetMap::TNodeID, double> BusSpeedLimits;
    std::unordered_map <CStreetMap::TNodeID, bool> BIDIR;
    std::vector<TNodeID> NodeVect;

    SImplementation(std::shared_ptr<SConfiguration> config)  {
        DStreetMap = config->StreetMap();//pull map and busSystem from config
        DBusSystem = config->BusSystem();
        CBusSystemIndexer BusSystemIndexer(DBusSystem);

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
            NodeVect.push_back(Node->ID());
        }
        std::sort(NodeVect.begin(), NodeVect.end());
        std::cout<<"Way Count: "<<DStreetMap->WayCount()<<std::endl;
        
        for(size_t Index = 0; Index < DStreetMap->WayCount(); Index++) { //iterate through ways
            auto Way = DStreetMap->WayByIndex(Index);
            std::cout<<"Way ID: "<<Way->ID()<<std::endl;
            std::cout<<"nodes in way:"<<Way->NodeCount()<<std::endl;
            bool Bikable = Way->GetAttribute("bicycle") != "no"; //means that its not bikable/bikable 
            std::cout<<"bikable? "<<Bikable<<std::endl;
            bool Bidirectional = Way->GetAttribute("oneway") != "yes"; //decides if bidir flag is passed
            double speed = config->DefaultSpeedLimit();
            if(Way->HasAttribute("maxspeed")) {
                std::string str = Way->GetAttribute("maxspeed").substr(0, Way->GetAttribute("maxspeed").length() - 4);
                speed = stod(str);               
                std::cout<<speed<<"speed: "<<std::endl;
            }
            for(size_t NodeIndex = 0; NodeIndex < Way->NodeCount(); NodeIndex++) { //go through nodes in each way
            
                std::cout<<"entering node loop: "<<std::endl;
                auto PreviousNodeID = Way->GetNodeID(NodeIndex); 
                auto NextNodeID = Way->GetNodeID(NodeIndex + 1);//get next node id 
                auto PreviousNode = DStreetMap->NodeByID(PreviousNodeID); 
                auto NextNode = DStreetMap->NodeByID(NextNodeID);
                CPathRouter::TVertexID PreviousVertexID = DNodeToVertexID[PreviousNodeID];
                CPathRouter::TVertexID NextVertexID = DNodeToVertexID[NextNodeID];
                std::cout<<"Prev vertex: "<<PreviousVertexID<<" next vertex "<<NextVertexID<<std::endl;
                if(PreviousNode == nullptr) {std::cout<<"Previous Node null"<<std::endl; break;}
                if(NextNode == nullptr) {std::cout<<"Next Node null"<<std::endl; break;}

                auto stop = BusSystemIndexer.StopByNodeID(PreviousNodeID); 
                BusSpeedLimits[PreviousNode->ID()] = speed;//store speed limit
                double weightDist = SGeographicUtils::HaversineDistanceInMiles(PreviousNode->Location(), NextNode->Location());
                double weightTimeWalk = weightDist/config->WalkSpeed();
                double weightTimeBike = weightDist/config->BikeSpeed();
                DShortestPathRouter.AddEdge(PreviousVertexID, NextVertexID, weightDist, Bidirectional);
                DFastestPathRouterWalkBus.AddEdge(PreviousVertexID, NextVertexID, weightTimeWalk, true);
                BusSpeedLimits[PreviousNodeID] = speed;
                BIDIR[PreviousNodeID] = Bidirectional;
                TransportType[PreviousNodeID] = CTransportationPlanner::ETransportationMode::Walk;
                if(Bikable == true) {//bikable edge always bidirectional
                    DFastestPathRouterBike.AddEdge(PreviousVertexID, NextVertexID, weightTimeBike, Bidirectional);
                    std::cout<<"Bike edge added"<< weightTimeBike<< PreviousVertexID<<" to "<<NextVertexID<<std::endl;
                }
            }
        }

        for(size_t Index = 0; Index < DBusSystem->RouteCount(); Index ++) {
            std::cout<<"entering route loop: "<<std::endl;
            auto Route = DBusSystem->RouteByIndex(Index);
            std::vector <CPathRouter::TVertexID> Path;
            for(size_t RouteIndex = 0; RouteIndex < Route->StopCount(); RouteIndex++) {
                auto previousStopID = Route->GetStopID(RouteIndex);
                auto nextStopID = Route->GetStopID(RouteIndex + 1);
                auto prevstop = DBusSystem->StopByID(previousStopID);
                if(prevstop == nullptr) {std::cout<<"prev stop is null"<<std::endl; break;}
                auto nextstop = DBusSystem->StopByID(nextStopID);
                if(nextstop == nullptr) {std::cout<<"next stop is null"<<std::endl; break;}
                auto search = BusSpeedLimits.find(prevstop->NodeID());
                CPathRouter::TVertexID PreviousVertexID = DNodeToVertexID[prevstop->NodeID()];
                CPathRouter::TVertexID NextVertexID = DNodeToVertexID[nextstop->NodeID()];
                double busspeed = config->DefaultSpeedLimit();
                if(search != BusSpeedLimits.end()) {
                    busspeed = search->second;
                }
                bool bidir = BIDIR[prevstop->NodeID()];
                double distance = DShortestPathRouter.FindShortestPath(PreviousVertexID, NextVertexID, Path);
                double weightTime = distance/busspeed + (config->BusStopTime()/3600);
                std::cout<<weightTime<<std::endl;
                DFastestPathRouterWalkBus.AddEdge(PreviousVertexID, NextVertexID, weightTime, bidir);
                TransportType[prevstop->NodeID()] = CTransportationPlanner::ETransportationMode::Bus;
                std::cout<<"BusEdge added"<<std::endl;
            }
        }
    }

    std::size_t NodeCount() const noexcept {
        return DStreetMap->NodeCount();
    }
    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const { //DNodeToVertexID holds the sorted nodes for lookup
        if(!NodeVect.empty()) {
            auto nodeid = NodeVect[index];
            auto nodeptr = DStreetMap->NodeByID(nodeid);
            return nodeptr;
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
        path.clear();
        std::vector <CPathRouter::TVertexID> ShortestPathBike;
        std::vector <CPathRouter::TVertexID> ShortestPathWalkBus;
        auto PreviousVertexID = DNodeToVertexID[src];
        auto NextVertexID  = DNodeToVertexID[dest];
        
        auto TimeBike = DFastestPathRouterBike.FindShortestPath(PreviousVertexID, NextVertexID, ShortestPathBike);
        auto TimeWalkBus = DFastestPathRouterWalkBus.FindShortestPath(PreviousVertexID, NextVertexID, ShortestPathWalkBus);

        CStreetMap::TNodeID nodeID;
        if(TimeBike < TimeWalkBus) {
            for(auto VertexID : ShortestPathBike) {
                auto nodeSearch = DNodeToVertexID.find(VertexID);
                if(nodeSearch != DNodeToVertexID.end()) {
                    nodeID = nodeSearch->second;
                }
            auto nodepair = std::make_pair(CTransportationPlanner::ETransportationMode::Bike, nodeID);
            path.push_back(nodepair);
            }
            return TimeBike;
        }
        else{
            for(auto VertexID : ShortestPathWalkBus) {
                auto nodeSearch = DNodeToVertexID.find(VertexID);
                if(nodeSearch != DNodeToVertexID.end()) {
                    nodeID = nodeSearch->second;
                }
                auto search = TransportType.find(nodeID);
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