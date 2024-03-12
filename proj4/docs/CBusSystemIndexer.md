bussystemindexer.md

class CBusSystemIndexer{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
    public:
        using TNodeID = CStreetMap::TNodeID;
        using SStop = CBusSystem::SStop;
        using SRoute = CBusSystem::SRoute;
        CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem);
        ~CBusSystemIndexer();

        std::size_t StopCount() const noexcept;
        std::size_t RouteCount() const noexcept;
        std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept;
        std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept;
        std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept;
        bool RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute> > &routes) const noexcept;
        bool RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept;
};

The CBusSystemIndexer class is designed to efficiently manage and query bus system data. It encapsulates functionality for sorting and searching bus stops and routes, leveraging a private implementation idiom to hide implementation details.
The CBusSystemIndexer will index the CBusSystem information provided for ease of lookup of stops and routes. It will be helpful class in developing some of the later components of the project.

properties
TNodeID
An alias for CStreetMap::TNodeID, representing the unique identifier for a node within the street map. Nodes typically correspond to geographic locations, such as intersections or specific points of interest, and are used here to uniquely identify bus stops.

SStop
An alias for CBusSystem::SStop, representing a single bus stop within the system. Each stop is associated with a TNodeID for easy identification and mapping.

SRoute
An alias for CBusSystem::SRoute, representing a bus route within the system. A route is essentially a collection of stops (SStop) that the bus will pass through in order.


All of the functions:
StopCount()
Returns the total number of stops in the bus system.

RouteCount()
Returns the total number of routes in the bus system.
