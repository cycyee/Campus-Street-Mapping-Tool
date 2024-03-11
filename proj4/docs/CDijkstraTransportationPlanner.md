CDijkstraTransportationPlanner.md

class CDijkstraTransportationPlanner : public CTransportationPlanner{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
    public:
        CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config);
        ~CDijkstraTransportationPlanner();

        std::size_t NodeCount() const noexcept override;
        std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept override;

        double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path) override;
        double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path) override;
        bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const override;
};

As can be seen, CDijkstraTransportationPlanner inherits from the Abstract interface of CTransportationPlanner, to privatize the implementation. The useage and functionality of the interface functions can be seen in CTransportationPlanner.md.

All of the following function implementations are overrides for the virtual ones in CTransportationPlanner. 

std::size_t NodeCount() const noexcept
returns the number of nodes in the Planner. Since the SConfiguration is passed in, and it holds both the BUsSystem and the StreetMap, the nodes are already stored, and the amount is easily accessible. 

std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept
The nodes are stored in a vector, and std::sort from the algorithm library is called upon it to sort the nodes. Then, they can be accessed by index. 

double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path)
FindShortestPath is populated by making edges across every node pair in each way
the path is stored in a vector passed into the Pathrouter. This only returns distances, and needs a src/dest passed in. It is also filled with vertices at the same time that the other pathrouter objects are filled, as all 3 maps need all of the same points. the function itself populates the path vector from a temporary one passed into the router, and thus adds the vertices in. 

double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path)
FindFastestPath works similar to FindShortestPath, insofar as it relies on an instance of the DijkstraPathRouter. Here, 1 pathrouter is populated with times as the edge weights from walking/bussing, and the other is populated with biking times, using the bike speed of 8mph.
The Walk/Bus one creates edges for walking all over the map along ways, and also creates 'Bus' edges that connect bus stops to each other. It is able to get the speed limit from the xml attribute "maxspeed" and uses that in place of the default when necessary. 

It compares the fastest times from the biking vs the walk/bus methods, including information surrounding bus stop waiting times.The minimum is returned, and the associated vector is also returned. 



