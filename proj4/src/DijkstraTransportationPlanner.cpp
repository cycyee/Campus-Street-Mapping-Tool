#include "DijkstraTransportationPlanner.h"
#include <unordered_map>
#include "DijkstraPathRouter.h
#include "GeographicUtils.h"

struct CDijkstraTransportationPlanner::SImplementation {
    std::shared_ptr< CStreetMap > DStreetMap;
    std::shared_ptr <CBusSystem > DBusSystem;
    std::unordered_map< CStreetMap::TNodeID, CPathRouter::TVertexID> DNodeToVertexID;
    CDijkstraPathRouter DShortestPathRouter;
    CDijkstraPathRouter DFastestPathRouterBike;
    CDijkstraPathRouter DFastesPathRouterWalkBus;


    SImplementation(std::shared_ptr< Sconfiguration> config)  {
        DStreetMap = config->StreetMap();
        DBusSystem = config->BusSystem();

        for(size_t Index = 0; Index < DStreetMap->NodeCount(); Index++) { //populate DNodeToVertexID
            auto Node = DStreetMap-> NodebyIndex(Index);
            auto VertexID = DShortestPathRouter.AddVertex(Node->ID());
            DFastestPathRouterBike.AddVertex(Node->ID());
            DFastestPathRouterWalkBus.AddVertex(Node->ID());
            DNodeToVertexID[Node->ID()] = VertexID;
        }

        for(size_t Index = 0; Index < DStreetMap->WayCount(); Index++) { //iterate through ways
            auto Way = DStreetMap->WayByIndex(index);
            bool Bikable = Way->GetAttribute("bicycle") != "no"; //means that its not bikable/bikable 
            bool Bidirectional = Way->GetAttribute("oneway") != "yes"; //decides if bidir flag is passed
            auto PreviousNodeID = Way->GetNodeID(0);
            for(size_t NodeIndex = 1; NodeIndex < Way->NodeCount(); NodeIndex++) { //go through nodes in each way
                auto NextNodeID = Way->GetNodeID(NodeIndex);
                
            }

        }
    }

    std::size_t NodeCount() const noexcept {
        return DStreetMap->NodeCount();
    }
    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const {

    }

    double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path) {
        std::vector <CPathRouter::TVertexID > ShortestPath;
        auto SourceVertexID = DNodetoVertexID[src];
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
    }
    bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const {
    }

};

CDijkstraTransportationPlanner::CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config) {
    DImplementation = std::make_unique<SImplementation>(config);
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