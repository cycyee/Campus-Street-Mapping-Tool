#include<gtest/gtest.h>
#include "DijkstraPathRouter.h"

TEST(DijkstraPathRouter, RouteTest) {
    CDijkstraPathRouter PathRouter;
    std::vector<CPathRouter::TVertexID> Vertices;
    for (std::size_t Index = 0; Index < 6; Index++) {
        Vertices.push_back(PathRouter.AddVertex(Index));
        EXPECT_EQ(Index, std::any_cast<std::size_t>(PathRouter.GetVertexTag(Vertices.back())));
    }
    PathRouter.AddEdge(Vertices[0], Vertices[4], 3);
    PathRouter.AddEdge(Vertices[4], Vertices[5], 90);
    PathRouter.AddEdge(Vertices[5], Vertices[3], 6);
    PathRouter.AddEdge(Vertices[3], Vertices[2], 8);
    PathRouter.AddEdge(Vertices[2], Vertices[0], 1);
    PathRouter.AddEdge(Vertices[2], Vertices[1], 3);
    PathRouter.AddEdge(Vertices[1], Vertices[3], 9);
    std::vector<CPathRouter::TVertexID> Route;
    std::vector<CPathRouter::TVertexID> ExpectedRoute = {Vertices[2], Vertices[1], Vertices[3]};
    EXPECT_EQ(12.0, PathRouter.FindShortestPath(Vertices[2], Vertices[3], Route));
    EXPECT_EQ(Route, ExpectedRoute);
}