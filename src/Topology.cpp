 #include "Topology.hpp"

Topology::~Topology(){DestroyTopo();}
void Topology::DestroyTopo(){}//TODO

void Component::PrintSubtree(int level)
{
    //cout << "---PrintSubtree---" << endl;
    for (int i = 0; i < level; ++i)
        cout << " " ;
    cout << name << " " << id << " - children: " << children.size() <<  endl;

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
vector<Component*>* Component::GetChildren(){return &children;}
string Component::GetName(){return name;}
void Component::SetParent(Component* _parent){parent = _parent;}

Component::Component(int _id, string _name) : id(_id), name(_name){}
Component::Component() :Component(0,"unknown"){}
Topology::Topology():Component(0, "topology"){}
Node::Node():Component(0, "node"){}
Chip::Chip():Component(0, "chip"){}
Chip::Chip(int _id):Component(_id, "chip"){}
Cache::Cache():Component(0, "cache"){}
Cache::Cache(int _id, int  _cache_level, int _cache_size):cache_level(_cache_level), cache_size(_cache_size), Component(_id, "cache"){}
Numa::Numa():Component(0, "Numa"){}
Numa::Numa(int _id):Component(_id, "Numa"){}
Core::Core():Component(0, "Core"){}
Core::Core(int _id):Component(_id, "Core"){}
Thread::Thread():Component(0, "Thread"){}
Thread::Thread(int _id):Component(_id, "Thread"){}
