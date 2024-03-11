CDijkstraPathRouter.md

class CDijkstraPathRouter : public CPathRouter{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
    public:
        CDijkstraPathRouter();
        ~CDijkstraPathRouter();

        std::size_t VertexCount() const noexcept;
        TVertexID AddVertex(std::any tag) noexcept;
        std::any GetVertexTag(TVertexID id) const noexcept;
        bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept;
        bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept;
        double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept;
};

As can be seen above, and in Pathrouter.md, CDijkstraPathRouter inherits from CPathRouter, and is there to privately implement all of the functions outlined in the interface. 

Each function's useage and functionality can be found in CPathRouter.md. Here, their logic will be outlined:

- std::size_t VertexCount() const noexcept;
see CPathRouter.md's vertexcount 

Is able to return the VertexCount via the fact that they are stored in a vector

- TVertexID AddVertex(std::any tag) noexcept;
see CPathRouter.md's AddVertex

AddVertex pushes the new vertex along with the tag to the Vertex vector, the TVertexID is created from the size of the vertex vector, so that each new vertex increases the id # by 1. In a graph, it is like having the vertices numbered from 1-n

-std::any GetVertexTag(TVertexID id) const noexcept;
see CPathRouter.md's GetVertexTag

GetVertexTag just looks up the VertexID in the Vertex vector, and returns the associated tag. 

-bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept;
see CPathRouter.md's AddEdge

Creates a pair of the weight and destination node
if Bidir is true, creates a pair of the weight and src node as well. 
these are then pushed to the Edge vector, which is embedded in the SVertex Struct. Each Vertex holds the edges associated with it. these vertices are also stored in a vector for other functions

- double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept;
see CPathRouter.md's FindShortestPath

Implements Dijkstra's algorithm once the graph has been constructed of edges and vertices. 
The function makes use of a min-heap and a VertexCompare function which allows it to differentiate between smaller edges
Uses a Distances vector to keep track of all of the distances associated with vertices, and a Previous Vector to keep track of Vertices

It will then loop until all vertices have been processed or until the destination vertex is reached, and pop the vertex with the shortest known distance from PendingVertices.
There is a check if the current vertex ID is out of range. If so, handle the error.
If the current vertex is the destination, break out of the loop, as we are done. 
Iterate through all edges of the current vertex, for comparison. 
Calculate the total distance from the source vertex to the destination vertex via the current edge.
Update the shortest distance to the destination if the calculated distance is shorter.
If the destination vertex has not been visited yet (distance is still set to indicate no path exists), add it to PendingVertices.
Re-heapify PendingVertices to maintain the min-heap property.
The path is reconstructed by reversing through the Previously visited Vertices, and the Distance associated with the dst/src is returned. 

