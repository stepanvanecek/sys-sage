#include "Topology.hpp"

void Component::PrintSubtree() { PrintSubtree(0); }
void Component::PrintSubtree(int level)
{
    //cout << "---PrintSubtree---" << endl;
    for (int i = 0; i < level; ++i)
        cout << "  " ;
    cout << GetComponentTypeStr() << " (name " << name << ") id " << id << " - children: " << children.size() <<  endl;

    if(!children.empty())
    {
        for(auto it = begin(children); it != end(children); ++it)
        {
            (*it)->PrintSubtree(level+1);
        }
    }
}
void Component::PrintAllDataPathsInSubtree()
{
    vector<Component*> subtreeList;
    GetSubtreeNodeList(&subtreeList);
    for(auto it = std::begin(subtreeList); it != std::end(subtreeList); ++it)
    {
        vector<DataPath*>* dp_in = (*it)->GetDataPaths(SYS_SAGE_DATAPATH_INCOMING);
        vector<DataPath*>* dp_out = (*it)->GetDataPaths(SYS_SAGE_DATAPATH_OUTGOING);
        if(dp_in->size() > 0 || dp_out->size() > 0 )
        {
            cout << "DataPaths regarding Component (" << (*it)->GetComponentTypeStr() << ") id " << (*it)->GetId() << endl;
            for(auto it2 = std::begin(*dp_out); it2 != std::end(*dp_out); ++it2)
            {
                cout << "    ";
                (*it2)->Print();
            }
            for(auto it2 = std::begin(*dp_in); it2 != std::end(*dp_in); ++it2)
            {
                cout << "    ";
                (*it2)->Print();
            }
        }
    }
}

void Component::InsertChild(Component * child)
{
    child->SetParent(this);
    children.push_back(child);
}
Component* Component::GetChild(int _id)
{
    for(auto it = begin(children); it != end(children); ++it)
    {
        if((*it)->id == _id)
            return (Component*)(*it);
    }
    return NULL;
}

int Component::GetNumThreads()
{
    if(componentType == SYS_SAGE_COMPONENT_THREAD)
        return 1;
    int numPu = 0;
    for(auto it = std::begin(children); it != std::end(children); ++it)
    {
        numPu+=(*it)->GetNumThreads();
    }
    return numPu;
}

int Component::GetTopoTreeDepth()
{
    if(children.empty()) //is leaf
        return 1;
    int maxDepth = 0;
    for(auto it = std::begin(children); it != std::end(children); ++it)
    {
        int subtreeDepth = (*it)->GetTopoTreeDepth();
        if(subtreeDepth > maxDepth)
            maxDepth = subtreeDepth;
    }
    return maxDepth+1;
}

void Component::GetComponentsNLevelsDeeper(vector<Component*>* outArray, int depth)
{
    if(depth <= 0)
    {
        outArray->push_back(this);
        return;
    }
    for(auto it = std::begin(children); it != std::end(children); ++it)
    {
        (*it)->GetComponentsNLevelsDeeper(outArray, depth-1);
    }
    return;
}

void Component::GetSubcomponentsByType(vector<Component*>* outArray, int _componentType)
{
    if(_componentType == componentType)
    {
        outArray->push_back(this);
    }
    for(auto it = std::begin(children); it != std::end(children); ++it)
    {
        (*it)->GetSubcomponentsByType(outArray, _componentType);
    }
}

void Component::GetSubtreeNodeList(vector<Component*>* outArray)
{
    outArray->push_back(this);
    for(auto it = std::begin(children); it != std::end(children); ++it)
    {
        (*it)->GetSubtreeNodeList(outArray);
    }
    return;
}

Component* Component::FindSubcomponentById(int _id, int _componentType)
{
    if(componentType == _componentType && id == _id){
        //cout << "   found component " << GetId() << " (" << GetComponentType() << ") found .." << _id << " " << _componentType << endl;
        return this;
    }
    for(auto it = std::begin(children); it != std::end(children); ++it)
    {
        Component* ret = (*it)->FindSubcomponentById(_id, _componentType);
        if(ret != NULL)
        {
            return ret;
        }
    }
    return NULL;
}

Component* Component::FindParentByType(int _componentType)
{
    if(componentType == _componentType){
        return this;
    }
    if(parent != NULL){
        cout << "   passing through component " << GetId() << " " << GetComponentType() << "(" << GetComponentTypeStr() << ") - searching for " << _componentType << endl;
        return parent->FindParentByType(_componentType);
    }

    return NULL;
}

void Component::AddDataPath(DataPath* p, int orientation)
{
    if(orientation == SYS_SAGE_DATAPATH_OUTGOING)
        dp_outgoing.push_back(p);
    else if(orientation == SYS_SAGE_DATAPATH_INCOMING)
        dp_incoming.push_back(p);
}

DataPath* Component::GetDpByType(int dp_type, int orientation)
{
    if(orientation & SYS_SAGE_DATAPATH_OUTGOING){
        for(DataPath* dp : dp_outgoing){
            if(dp->GetDpType() == dp_type)
                return dp;
        }
    }
    if(orientation & SYS_SAGE_DATAPATH_INCOMING){
        for(DataPath* dp : dp_incoming){
            if(dp->GetDpType() == dp_type)
                return dp;
        }
    }
    return NULL;
}
void Component::GetAllDpByType(vector<DataPath*>* outDpArr, int dp_type, int orientation)
{
    if(orientation & SYS_SAGE_DATAPATH_OUTGOING){
        for(DataPath* dp : dp_outgoing){
            if(dp->GetDpType() == dp_type)
                outDpArr->push_back(dp);
        }
    }
    if(orientation & SYS_SAGE_DATAPATH_INCOMING){
        for(DataPath* dp : dp_incoming){
            if(dp->GetDpType() == dp_type)
                outDpArr->push_back(dp);
        }
    }
    return;
}

vector<DataPath*>* Component::GetDataPaths(int orientation)
{
    if(orientation == SYS_SAGE_DATAPATH_INCOMING)
        return &dp_incoming;
    else if(orientation == SYS_SAGE_DATAPATH_OUTGOING)
        return &dp_outgoing;
    else //TODO
        return NULL;
}

string Component::GetComponentTypeStr()
{
    switch(componentType)
    {
        case SYS_SAGE_COMPONENT_NONE:
            return "None";
        case SYS_SAGE_COMPONENT_THREAD:
            return "HW_thread";
        case SYS_SAGE_COMPONENT_CORE:
            return "Core";
        case SYS_SAGE_COMPONENT_CACHE:
            return "Cache";
        case SYS_SAGE_COMPONENT_NUMA:
            return "NUMA";
        case SYS_SAGE_COMPONENT_CHIP:
            return "Chip";
        case SYS_SAGE_COMPONENT_NODE:
            return "Node";
        case SYS_SAGE_COMPONENT_TOPOLOGY:
            return "Topology";
    }
    return "";
}

int Component::GetTopologySize(unsigned * out_component_size, unsigned * out_dataPathSize)
{
    return GetTopologySize(out_component_size, out_dataPathSize, NULL);
}
int Component::GetTopologySize(unsigned * out_component_size, unsigned * out_dataPathSize, std::set<DataPath*>* counted_dataPaths)
{
    if(counted_dataPaths == NULL)
        counted_dataPaths = new std::set<DataPath*>();

    int component_size = 0;
    switch(componentType)
    {
        case SYS_SAGE_COMPONENT_NONE:
        break;
        case SYS_SAGE_COMPONENT_THREAD:
        component_size += sizeof(Thread);
        break;
        case SYS_SAGE_COMPONENT_CORE:
        component_size += sizeof(Core);
        break;
        case SYS_SAGE_COMPONENT_CACHE:
        component_size += sizeof(Cache);
        break;
        case SYS_SAGE_COMPONENT_SUBDIVISION:
        component_size += sizeof(Subdivision);
        break;
        case SYS_SAGE_COMPONENT_NUMA:
        component_size += sizeof(Numa);
        break;
        case SYS_SAGE_COMPONENT_CHIP:
        component_size += sizeof(Chip);
        break;
        case SYS_SAGE_COMPONENT_MEMORY:
        component_size += sizeof(Memory);
        break;
        case SYS_SAGE_COMPONENT_STORAGE:
        component_size += sizeof(Storage);
        break;
        case SYS_SAGE_COMPONENT_NODE:
        component_size += sizeof(Node);
        break;
        case SYS_SAGE_COMPONENT_TOPOLOGY:
        component_size += sizeof(Topology);
        break;
    }
    component_size += attrib.size()*(sizeof(string)+sizeof(void*)); //TODO improve
    component_size += children.size()*sizeof(Component*);
    (*out_component_size) += component_size;

    int dataPathSize = 0;
    dataPathSize += dp_incoming.size()*sizeof(DataPath*);
    dataPathSize += dp_outgoing.size()*sizeof(DataPath*);
    for(auto it = std::begin(dp_incoming); it != std::end(dp_incoming); ++it) {
        if(!counted_dataPaths->count((DataPath*)(*it))) {
            //cout << "new datapath " << (DataPath*)(*it) << endl;
            dataPathSize += sizeof(DataPath);
            dataPathSize += (*it)->attrib.size()*(sizeof(string)+sizeof(void*)); //TODO improve
            counted_dataPaths->insert((DataPath*)(*it));
        }
    }
    for(auto it = std::begin(dp_outgoing); it != std::end(dp_outgoing); ++it) {
        if(!counted_dataPaths->count((DataPath*)(*it))){
            //cout << "new datapath " << (DataPath*)(*it) << endl;
            dataPathSize += sizeof(DataPath);
            dataPathSize += (*it)->attrib.size()*(sizeof(string)+sizeof(void*)); //TODO improve
            counted_dataPaths->insert((DataPath*)(*it));
        }
    }
    (*out_dataPathSize) += dataPathSize;

    int subtreeSize = 0;
    for(auto it = std::begin(children); it != std::end(children); ++it) {
        subtreeSize += (*it)->GetTopologySize(out_component_size, out_dataPathSize, counted_dataPaths);
    }

    return component_size + dataPathSize + subtreeSize;
}

Component* Component::GetParent(){return parent;}
vector<Component*>* Component::GetChildren(){return &children;}
int Component::GetComponentType(){return componentType;}

string Component::GetName(){return name;}
int Component::GetId(){return id;}

void Component::SetParent(Component* _parent){parent = _parent;}

int Numa::GetSize(){return size;}
int Cache::GetCacheLevel(){return cache_level;}
long long Cache::GetCacheSize(){return cache_size;}
int Cache::GetCacheAssociativityWays(){return cache_associativity_ways;}

Component::Component(int _id, string _name, int _componentType) : id(_id), name(_name), componentType(_componentType){}
Component::Component() :Component(0,"unknown",SYS_SAGE_COMPONENT_NONE){}

Topology::Topology():Component(0, "topology", SYS_SAGE_COMPONENT_TOPOLOGY){}

Memory::Memory():Component(0, "Memory", SYS_SAGE_COMPONENT_MEMORY){}

Storage::Storage():Component(0, "Storage", SYS_SAGE_COMPONENT_STORAGE){}

Node::Node(int _id):Component(_id, "sys-sage node", SYS_SAGE_COMPONENT_NODE){}
Node::Node():Node(0){}

Chip::Chip(int _id):Component(_id, "Chip", SYS_SAGE_COMPONENT_CHIP){}
Chip::Chip():Chip(0){}

Cache::Cache(int _id, int  _cache_level, unsigned long long _cache_size, int _associativity): Component(_id, "cache", SYS_SAGE_COMPONENT_CACHE), cache_level(_cache_level), cache_size(_cache_size), cache_associativity_ways(_associativity){}
Cache::Cache():Cache(0,0,0,0){}

Subdivision::Subdivision(int _id, string _name, int _componentType): Component(_id, _name, _componentType){}
Subdivision::Subdivision(int _id): Component(_id, "Subdivision", SYS_SAGE_COMPONENT_SUBDIVISION){}
Subdivision::Subdivision():Subdivision(0){}

Numa::Numa(int _id, int _size):Subdivision(_id, "Numa", SYS_SAGE_COMPONENT_NUMA), size(_size){}
Numa::Numa(int _id):Numa(_id, 0){}
Numa::Numa():Numa(0){}

Core::Core(int _id):Component(_id, "Core", SYS_SAGE_COMPONENT_CORE){}
Core::Core():Core(0){}

Thread::Thread(int _id):Component(_id, "Thread", SYS_SAGE_COMPONENT_THREAD){}
Thread::Thread():Thread(0){}
