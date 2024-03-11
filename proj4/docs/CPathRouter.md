CPathRouter.md:

class CPathRouter{
    public:
        using TVertexID = std::size_t;

        static constexpr TVertexID InvalidVertexID = std::numeric_limits<TVertexID>::max();
        static constexpr double NoPathExists = std::numeric_limits<double>::max();

        virtual ~CPathRouter(){};

        virtual std::size_t VertexCount() const noexcept = 0;
        virtual TVertexID AddVertex(std::any tag) noexcept = 0;
        virtual std::any GetVertexTag(TVertexID id) const noexcept = 0;
        virtual bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept = 0;
        virtual bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept = 0;
        virtual double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept = 0;
};


The CPathRouter class is the parent class of the CDijkstraPathRouter, and holds the virtual functions that make up the DijkstraPathRouter interface. It is an abstract class, as it contains purely virtual functions. 
It defines 2 error types: NoPathExists, and InvalidVertexID. 
These error types are returned when blocks of the DijkstraPathRouter fail to return their intended type. 
For example, a function such as FIndShortestPath has error checking in it that will return NoPathExists if a situation such as the src and destination are unconnected occurs. 
InvalidVertexID would be returned from functions that return a VertexID, such as AddVertex, which is defined as TVertexID. 

TVertexID is actually of type size_t, and is the denomination for the Vertices of the graph that DisjkstraPathRouter is applied to. Additionally, the subsequent files that make use of the DijkstraPathRouter deal heavily with the VertexID type. 

All of the functions: 
VertexCount()
Which returns the number of Vertices in the pathrouter, no argument is passed in. has return type of size_t.

GetVertexTag(id)
which returns the Vertextag for any Vertex, provided its TVertexID is passed in. Returns an std::any if an invalid tag is passed in. 

AddVertex(tag)
Adds a vertex to the pathrouter, is used for constructing the directed graph. Returns the Vertex associated with the tag. tag can be of any type. 

AddEdge(src, dest, weight, bidir)
Adds an edge to the graph. Needs to be provided a weight, a source node, and a destination node. Can take in a bidir boolean value that defaults to false, and allows for the edge to go both ways. 

Precompute(deadline)
*extra credit: 
Given a timer, allows the pathrouter to precompute in an effort to improve speeds of returning queries

FindShortestPath(src, dest, path) 
Traverses the graph in such a way that the shortest path is returned. The path vector passed in is populated with the optimal traversal. A src and Destination TVertexID are passed in. Returns the distance in a type of double. 



are all purely virtual in this class, meaning they have to be overridden in a derived class. As such, their implementation is deferred to CDijkstraPathRouter, which implements them. The implementation logic is purposely witheld from the interface. 

