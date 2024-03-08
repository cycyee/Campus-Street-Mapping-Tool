#include"OpenStreetMap.h"
#include "XMLReader.h"
#include<unordered_map>
#include<vector>


struct COpenStreetMap::SImplementation {

    struct SNode : public CStreetMap::SNode {
        TNodeID DID;
        TLocation DLocation; //loc and id data
        std::unordered_map<std::string, std::string> DAttributes; //to map attributes to their title
        std::vector<std::string> DAttributeKeys;//vector of attribute keys

        SNode(TNodeID id, TLocation location) { //constructor arguments
            DID = id;
            DLocation = location;
        }

        ~SNode(){
        }

        TNodeID ID() const noexcept override { //DID is the Node id
            return DID;
        }

        TLocation Location() const noexcept override { //DLocation is the location pair
            return DLocation;
        }

        std::size_t AttributeCount() const noexcept override { //the size of the att keys vector is the amount of attributes
            return DAttributeKeys.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override { //search for the key thru the vect
            if(index < DAttributeKeys.size()) {
                return DAttributeKeys[index];
            }
            return std::string(); //empty string
        }

        bool HasAttribute(const std::string &key) const noexcept override {
            auto Search = DAttributes.find(key);
            return DAttributes.end() != Search;//search through the map to find the key
        }

        std::string GetAttribute(const std::string &key) const noexcept override {
            auto Search = DAttributes.find(key);
            if(DAttributes.end() != Search) {
                return Search->second;//the second will give the value in a key-value pair
            }
            return std::string();//empty string
        }
    };

    struct SWay : public CStreetMap::SWay {
        TWayID TID;
        std::unordered_map<std::string, std::string> WayAttributes; //map for attributes
        std::vector<std::string> WayAttributeKeys; //vector for keys
        std::vector<TNodeID> NodeRefVector; //vector of TNodeID for the node references, which are separate from attributes
        SWay(TWayID id) {
            TID = id;
        }
        ~SWay(){
        }

        TWayID ID() const noexcept override {
            return TID; //TID is the way id
        }

        std::size_t NodeCount() const noexcept override {
            return NodeRefVector.size(); //the size of the vect tells us how many node refs were in the way
        }
        TNodeID GetNodeID(std::size_t index) const noexcept override {
            if(NodeRefVector.size() < index) { //search through the vector for a node ref
                return NodeRefVector[index];
            }
            return InvalidNodeID; //special type for invaluide node id
        }
        std::size_t AttributeCount() const noexcept override {
            return WayAttributes.size(); //way attributes contains the attributes, so size is the count
        }
        std::string GetAttributeKey(std::size_t index) const noexcept override {
            if(WayAttributeKeys.size() < index) {
                return WayAttributeKeys[index];//can search through way attribute key vector to get a key
            }
            return std::string();//empty string
        }
        bool HasAttribute(const std::string &key) const noexcept override {
            auto Search = WayAttributes.find(key);
            return WayAttributes.end() != Search;//end is the value when find cannot find it
 
        }
        std::string GetAttribute(const std::string &key) const noexcept override {
            auto Search = WayAttributes.find(key);
            if(WayAttributes.end() != Search) {
                return Search->second;
            }
            return std::string(); //empty string
        }
        void SetAttribute(const std::string &key, const std::string &value) {
            WayAttributeKeys.push_back(key);
            WayAttributes[key] = value; //set attributes
        }
    };

    std::unordered_map<TNodeID, std::shared_ptr<CStreetMap::SNode> > DNodeIdToNode;
    std::vector < std::shared_ptr<CStreetMap::SNode> >DNodesByIndex;

    std::unordered_map<TWayID, std::shared_ptr<CStreetMap::SWay> > WayIDToWay;
    std::vector < std::shared_ptr<CStreetMap::SWay> >WaysByIndex;

    SImplementation(std::shared_ptr<CXMLReader> src) {
        SXMLEntity TempEntity;
        while(src->ReadEntity(TempEntity, true)) {
            if((TempEntity.DNameData == "osm") && (SXMLEntity::EType::EndElement == TempEntity.DType)) {
                break; //reached end 
            }
            else if ((TempEntity.DNameData == "node") && (SXMLEntity::EType::StartElement == TempEntity.DType)) {
                //parse node
                TNodeID NewNodeID = std::stoull(TempEntity.AttributeValue("id"));
                double Lat = std::stod(TempEntity.AttributeValue("lat"));
                double Lon = std::stod(TempEntity.AttributeValue("lon"));
                TLocation NewNodeLocation = std::make_pair(Lat, Lon);
                auto NewNode = std::make_shared<SNode>(NewNodeID, NewNodeLocation);
                DNodesByIndex.push_back(NewNode);
                DNodeIdToNode[NewNodeID] = NewNode;
                //handle tag elements
                while(src->ReadEntity(TempEntity, true)) {
                    if((TempEntity.DNameData == "node") && (SXMLEntity::EType::EndElement == TempEntity.DType)) {
                        break;
                    }
                    else if((TempEntity.DNameData == "tag") && (SXMLEntity::EType::StartElement == TempEntity.DType)) {
                        NewNode->DAttributeKeys.push_back(TempEntity.AttributeValue("k"));
                        NewNode->DAttributes[TempEntity.AttributeValue("k")] = TempEntity.AttributeValue("v");
                    }
                }
            }
            else if ((TempEntity.DNameData == "way") && (SXMLEntity::EType::StartElement == TempEntity.DType)) {
                //parse way
                TWayID NewWayID = std::stoull(TempEntity.AttributeValue("id"));
                //store the node refs and store the attributes
                auto NewWay = std::make_shared<SWay>(NewWayID);
                WaysByIndex.push_back(NewWay);
                WayIDToWay[NewWayID] = NewWay;
                while(src->ReadEntity(TempEntity, true)) {
                    if((TempEntity.DNameData == "way") && (SXMLEntity::EType::EndElement == TempEntity.DType)) {
                        break;//ways also close with a single "way" end element
                    }
                    else if((TempEntity.DNameData == "tag") && (SXMLEntity::EType::StartElement == TempEntity.DType)) {
                        NewWay->WayAttributeKeys.push_back(TempEntity.AttributeValue("k"));
                        NewWay->WayAttributes[TempEntity.AttributeValue("k")] = TempEntity.AttributeValue("v");
                        //set the attributes manually, (i couldn't properly use setattribute)
                    }
                    else if((TempEntity.DNameData == "nd") && (SXMLEntity::EType::StartElement == TempEntity.DType)) {
                        //handle node refs
                        TNodeID tempid = std::stoull(TempEntity.AttributeValue("ref"));
                        NewWay->NodeRefVector.push_back(tempid);
                    }
                }
            }
        }
    }

    std::size_t NodeCount() const{
        return DNodesByIndex.size(); //size is the count for nodes
    }
    std::size_t WayCount() const{
        return WaysByIndex.size(); //same, the vector holds an index for each way
    }
    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const{
        if(index < DNodesByIndex.size()) {
            return DNodesByIndex[index];
        }
        return nullptr; //null if the index is invalid

    }
    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const {
        auto Search = DNodeIdToNode.find(id);
        if(DNodeIdToNode.end() != Search) {
            return Search->second;
        }
        return nullptr; //null for invalid id
    }
    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const {
        if(index < WaysByIndex.size()) {
            return WaysByIndex[index];
        }
        return nullptr; //null for invalid id

    }
    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const {
        auto Search = WayIDToWay.find(id);
        if(Search != WayIDToWay.end()) {
            return Search->second;
        }
        return nullptr; //null for invalid id
    }
};

//defer implementations 
COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {
    DImplementation = std::make_unique<SImplementation>(src);
}

COpenStreetMap::~COpenStreetMap() {
}

std::size_t COpenStreetMap::NodeCount() const noexcept{
    return DImplementation->NodeCount();
}
std::size_t COpenStreetMap::WayCount() const noexcept{
    return DImplementation->WayCount();
}
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept{
    return DImplementation->NodeByIndex(index);
}
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept{
    return DImplementation->NodeByID(id);
}
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept{
    return DImplementation->WayByIndex(index);
}
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept{
    return DImplementation->WayByID(id);
}
