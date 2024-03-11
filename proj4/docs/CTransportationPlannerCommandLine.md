CTransportationPlannerCommandLine.md

class CTransportationPlannerCommandLine{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
    public:
        CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner);
        ~CTransportationPlannerCommandLine();
        bool ProcessCommands();
};

The CTransportationPlannerCommandLine class is something that provides information and write to the cmnd line based on arguments passed in. 
it is able to take in commands and output expected things, for example:
        "> "
        "------------------------------------------------------------------------\n"
        "help     Display this help menu\n"
        "exit     Exit the program\n"
        "count    Output the number of nodes in the map\n"
        "node     Syntax \"node [0, count)\" \n"
        "         Will output node ID and Lat/Lon for node\n"
        "fastest  Syntax \"fastest start end\" \n"
        "         Calculates the time for fastest path from start to end\n"
        "shortest Syntax \"shortest start end\" \n"
        "         Calculates the distance for the shortest path from start to end\n"
        "save     Saves the last calculated path to file\n"
        "print    Prints the steps for the last calculated path\n"
        "> "

        writing help displays this list of commands, each of which has orbital info, as well as syntax/usage
    The class itself serves as a command line interface for the transplanner, which is the actual program which will take in cmnd line args and deal with them appropriately. 

    It is given only the bool ProcessCommands(), which just returns true on expected behavior

    It is able to process nodes, location data, and bus queries, all while returning the fastest distance, or shortest path. 
