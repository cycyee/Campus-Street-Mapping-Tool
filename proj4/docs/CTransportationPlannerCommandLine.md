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

The CTransportationPlannerCommandLine class is 