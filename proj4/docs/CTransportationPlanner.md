CTransportationPlanner.md

class CTransportationPlanner{
    public:
        using TNodeID = CStreetMap::TNodeID;
        enum class ETransportationMode {Walk, Bike, Bus};
        using TTripStep = std::pair<ETransportationMode, TNodeID>;

        struct SConfiguration{
            virtual ~SConfiguration(){};
            virtual std::shared_ptr<CStreetMap> StreetMap() const noexcept = 0;
            virtual std::shared_ptr<CBusSystem> BusSystem() const noexcept = 0;
            virtual double WalkSpeed() const noexcept = 0;
            virtual double BikeSpeed() const noexcept = 0;
            virtual double DefaultSpeedLimit() const noexcept = 0;
            virtual double BusStopTime() const noexcept = 0;
            virtual int PrecomputeTime() const noexcept = 0;
        };

        virtual ~CTransportationPlanner(){};

        virtual std::size_t NodeCount() const noexcept = 0;
        virtual std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept = 0;

        virtual double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path) = 0;
        virtual double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path) = 0;
        virtual bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const = 0;
};


The CTransportationPlanner utilizes all of the previously constructed classes in order to implement the mapping system. 

Its purely virtual interface contains a struct SConfiguration, which provides shared ptrs to a CBusSystem and a CStreetMap.

Additionally, it provides many different functions: SConfiguration provides various default speeds and times to the TransportationPlanner implementation privately. It also denotes 3 different enum types, which denote the transportation method. 

its virtual interface functions have their implementation deferred to the CDijkstraTransportationPlanner, which inherits from this:
    NodeCount() provides the number of nodes.

    SortedNodeByIndex sorts the nodes and allowes for lookup via node index. It returns a shared_ptr to the node, and a nullptr if the call fails. 

    FindShortestPath uses the DijkstraPathRouter to calculate the shortest path, and returns a distance double value. A src and destination node must be passed in, similar to the call of the DijkstraPathRouter. Here, the edge weights of the graph are given as distances on the map
    it also returns the shortest path as in the DPR. 

    FindFastestPath returns the fastest possible path, given 2 methods of traversal: Walk/bus or bike
    it calculates the time necessary to traverse from the src locaton to the dest location and provides the lesser of the two. It is also able to return the shortest path, as bike/bus may have different paths. 

    GetPathDescription is extra credit, and gets XML tags as the path description to provide to the user. 