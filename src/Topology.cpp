 #include "Topology.hpp"

Topology::~Topology(){DestroyTopo();}
void Topology::DestroyTopo(){}//TODO

void Component::PrintSubtree(int level)
{
    //cout << "---PrintSubtree---" << endl;
    for (int i = 0; i < level; ++i)
        cout << " " ;
    cout << componentType << " (" << name << ") " << id << " - children: " << children.size() <<  endl;

    if(!children.empty())
    {
        for(auto it = begin(children); it != end(children); ++it)
        {
            (*it)->PrintSubtree(level+1);
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
    if(componentType == SYS_TOPO_COMPONENT_THREAD)
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

void Component::GetSubtreeNodeList(vector<Component*>* outArray)
{
    outArray->push_back(this);
    for(auto it = std::begin(children); it != std::end(children); ++it)
    {
        (*it)->GetSubtreeNodeList(outArray);
    }
    return;
}

Component* Component::GetParent(){return parent;}
vector<Component*>* Component::GetChildren(){return &children;}
int Component::GetComponentType(){return componentType;}
string Component::GetName(){return name;}
int Component::GetId(){return id;}

void Component::SetParent(Component* _parent){parent = _parent;}

int Cache::GetCacheLevel(){return cache_level;}
int Cache::GetCacheSize(){return cache_size;}

Component::Component(int _id, string _name, int _componentType) : id(_id), name(_name), componentType(_componentType){}
Component::Component() :Component(0,"unknown",SYS_TOPO_COMPONENT_NONE){}

Topology::Topology():Component(0, "topology", SYS_TOPO_COMPONENT_TOPOLOGY){}

Node::Node():Component(0, "node", SYS_TOPO_COMPONENT_NODE){}

Chip::Chip(int _id):Component(_id, "chip", SYS_TOPO_COMPONENT_CHIP){}
Chip::Chip():Chip(0){}

Cache::Cache(int _id, int  _cache_level, int _cache_size): Component(_id, "cache", SYS_TOPO_COMPONENT_CACHE), cache_level(_cache_level), cache_size(_cache_size){}
Cache::Cache():Cache(0,0,0){}

Numa::Numa(int _id):Component(_id, "Numa", SYS_TOPO_COMPONENT_NUMA){}
Numa::Numa():Numa(0){}

Core::Core(int _id):Component(_id, "Core", SYS_TOPO_COMPONENT_CORE){}
Core::Core():Core(0){}

Thread::Thread(int _id):Component(_id, "Thread", SYS_TOPO_COMPONENT_THREAD){}
Thread::Thread():Thread(0){}
