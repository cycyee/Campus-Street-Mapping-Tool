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
    std::vector<CPathRouter::TVertexID> ExpectedRoute = {Vertices[2], Vertices[0], Vertices[4]};
    EXPECT_EQ(4.0, PathRouter.FindShortestPath(Vertices[2], Vertices[4], Route)); //attention needed
    EXPECT_EQ(Route, ExpectedRoute);
    std::vector<CPathRouter::TVertexID> Route2;
    std::vector<CPathRouter::TVertexID> ExpectedRoute2 = {Vertices[0], Vertices[4], Vertices[5]};
    EXPECT_EQ(93.0, PathRouter.FindShortestPath(Vertices[0], Vertices[5], Route2));
    std::vector<CPathRouter::TVertexID> Route3;
    std::vector<CPathRouter::TVertexID> ExpectedRoute3 = {Vertices[5], Vertices[3], Vertices[2], Vertices[0]};
    EXPECT_EQ(15.0, PathRouter.FindShortestPath(Vertices[5], Vertices[0], Route3));
}

TEST(DijkstraPathRouter, NoEdgesTest) {
    CDijkstraPathRouter PathRouter;
    std::vector<CPathRouter::TVertexID> Vertices;
    for (std::size_t Index = 0; Index < 6; Index++) {
        Vertices.push_back(PathRouter.AddVertex(Index));
        EXPECT_EQ(Index, std::any_cast<std::size_t>(PathRouter.GetVertexTag(Vertices.back())));
    }
    std::vector<CPathRouter::TVertexID> Route;
    std::vector<CPathRouter::TVertexID> ExpectedRoute {};
    EXPECT_EQ((CPathRouter::NoPathExists), PathRouter.FindShortestPath(Vertices[2], Vertices[4], Route));
    EXPECT_EQ(Route, ExpectedRoute);
}

TEST(DijkstraPathRouter, BidirTest) {
    CDijkstraPathRouter PathRouter;
    std::vector<CPathRouter::TVertexID> Vertices;
    for (std::size_t Index = 0; Index < 6; Index++) {
        Vertices.push_back(PathRouter.AddVertex(Index));
        EXPECT_EQ(Index, std::any_cast<std::size_t>(PathRouter.GetVertexTag(Vertices.back())));
    }
    (PathRouter.AddEdge(Vertices[0], Vertices[4], 3, true));
    (PathRouter.AddEdge(Vertices[4], Vertices[5], 90, true));
    (PathRouter.AddEdge(Vertices[5], Vertices[3], 6, true));
    (PathRouter.AddEdge(Vertices[3], Vertices[2], 8, true));
    (PathRouter.AddEdge(Vertices[2], Vertices[0], 1, true));
    (PathRouter.AddEdge(Vertices[2], Vertices[1], 3, true));
    (PathRouter.AddEdge(Vertices[1], Vertices[3], 9, true));
    std::vector<CPathRouter::TVertexID> Route;
    std::vector<CPathRouter::TVertexID> ExpectedRoute {Vertices[4], Vertices[0], Vertices[2]};
    EXPECT_EQ(4.0, PathRouter.FindShortestPath(Vertices[4], Vertices[2], Route));
    EXPECT_EQ(Route, ExpectedRoute);
}

TEST(DijkstraPathRouter, parallelTest) {
    CDijkstraPathRouter PathRouter;
    std::vector<CPathRouter::TVertexID> Vertices;
    for (std::size_t Index = 0; Index < 6; Index++) {
        Vertices.push_back(PathRouter.AddVertex(Index));
        EXPECT_EQ(Index, std::any_cast<std::size_t>(PathRouter.GetVertexTag(Vertices.back())));
    }
    (PathRouter.AddEdge(Vertices[0], Vertices[4], 1, true));
    (PathRouter.AddEdge(Vertices[4], Vertices[5], 1, true));
    (PathRouter.AddEdge(Vertices[5], Vertices[3], 1, true));
    (PathRouter.AddEdge(Vertices[3], Vertices[2], 1, true));
    (PathRouter.AddEdge(Vertices[2], Vertices[0], 1));
    (PathRouter.AddEdge(Vertices[2], Vertices[1], 1, true));
    (PathRouter.AddEdge(Vertices[1], Vertices[3], 1));
    (PathRouter.AddEdge(Vertices[1], Vertices[4], 1, true));
    (PathRouter.AddEdge(Vertices[4], Vertices[0], 1));
    (PathRouter.AddEdge(Vertices[5], Vertices[2], 1, true));
    (PathRouter.AddEdge(Vertices[0], Vertices[2], 1));
    std::vector<CPathRouter::TVertexID> Route;
    std::vector<CPathRouter::TVertexID> ExpectedRoute {Vertices[1], Vertices[4], Vertices[5]};
    EXPECT_EQ(2.0, PathRouter.FindShortestPath(Vertices[1], Vertices[5], Route));
    
    std::cout<<Route.size()<<std::endl;
    EXPECT_EQ(Route, ExpectedRoute);
}
