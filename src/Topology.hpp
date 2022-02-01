#ifndef TOPOLOGY
#define TOPOLOGY

#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "DataPath.hpp"

#define SYS_SAGE_COMPONENT_NONE 1
#define SYS_SAGE_COMPONENT_THREAD 2
#define SYS_SAGE_COMPONENT_CORE 4
#define SYS_SAGE_COMPONENT_CACHE 8
#define SYS_SAGE_COMPONENT_SUBDIVISION 16
#define SYS_SAGE_COMPONENT_NUMA 32
#define SYS_SAGE_COMPONENT_CHIP 64
#define SYS_SAGE_COMPONENT_MEMORY 128
#define SYS_SAGE_COMPONENT_STORAGE 256
#define SYS_SAGE_COMPONENT_NODE 512
#define SYS_SAGE_COMPONENT_TOPOLOGY 1024

#define SYS_SAGE_SUBDIVISION_TYPE_NUMA 2048


using namespace std;
class DataPath;

class Component {
public:
    Component();
    Component(int,string,int);
    void InsertChild(Component * child);
    void PrintSubtree(int level);
    void PrintAllDataPathsInSubtree();

    string GetName();
    int GetComponentType();
    string GetComponentTypeStr();
    int GetId();

    vector<Component*>* GetChildren();

    Component* GetParent();
    Component* GetChild(int _id);
    Component* FindSubcomponentById(int id, int componentType);
    Component* FindParentByType(int componentType);
    int GetNumThreads();
    int GetTopoTreeDepth();//0=empty, 1=1element,...
    void GetComponentsNLevelsDeeper(vector<Component*>* outArray, int depth);
    void GetSubcomponentsByType(vector<Component*>* outArray, int componentType);
    void GetSubtreeNodeList(vector<Component*>* outArray);
    vector<DataPath*>* GetDataPaths(int orientation);
    int GetTopologySize(unsigned * out_component_size, unsigned * out_dataPathSize);
    int GetTopologySize(unsigned * out_component_size, unsigned * out_dataPathSize, std::set<DataPath*>* counted_dataPaths);

    void SetParent(Component* parent);

    void AddDataPath(DataPath* p, int orientation);

    map<string,void*> metadata;
protected:
    int id;
    int depth;
    string name;
    const int componentType;
    vector<Component*> children;
    Component* parent;
    vector<DataPath*> dp_incoming;
    vector<DataPath*> dp_outgoing;

private:
};

class Topology : public Component {
public:
    Topology();
private:
};

class Node : public Component {
public:
    Node();
    Node(int _id);

#ifdef CAT_AWARE //defined in CAT_aware.cpp
    int UpdateL3CATCoreCOS();
#endif

private:
};

class Memory : public Component {
public:
    Memory();
private:
};

class Storage : public Component {
public:
    Storage();
private:
};

class Chip : public Component {
public:
    Chip();
    Chip(int _id);
private:
};

class Cache : public Component {
public:
    Cache();
    Cache(int _id, int  _cache_level, unsigned long long _cache_size, int _associativity);
    int GetCacheLevel();
    long long GetCacheSize();
    int GetCacheAssociativityWays();
private:
    int cache_level;
    long long cache_size;
    int cache_associativity_ways;
};

class Subdivision : public Component {
public:
    Subdivision();
    Subdivision(int _id);
    Subdivision(int _id, string _name, int _componentType);
protected:
    int type;
};

class Numa : public Subdivision {
public:
    Numa();
    Numa(int _id);
    Numa(int _id, int _size);
    int GetSize();
private:
    long long size;
};

class Core : public Component {
public:
    Core();
    Core(int _id);
private:
};

class Thread : public Component {
public:
    Thread();
    Thread(int _id);
    //#ifdef CAT_AWARE //defined in CAT_aware.cpp
        long long GetCATAwareL3Size();
    //#endif
private:
};

#endif
