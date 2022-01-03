#ifndef TOPOLOGY
#define TOPOLOGY

#include <iostream>
#include <vector>
#include <map>

#include "DataPath.hpp"

#define SYS_TOPO_COMPONENT_NONE 1
#define SYS_TOPO_COMPONENT_THREAD 2
#define SYS_TOPO_COMPONENT_CORE 4
#define SYS_TOPO_COMPONENT_CACHE 8
#define SYS_TOPO_COMPONENT_NUMA 16
#define SYS_TOPO_COMPONENT_CHIP 32
#define SYS_TOPO_COMPONENT_NODE 64
#define SYS_TOPO_COMPONENT_TOPOLOGY 128

using namespace std;
class DataPath;

class Component {
public:
    Component();
    Component(int,string,int);
    void InsertChild(Component * child);
    void PrintSubtree(int level);

    string GetName();
    int GetComponentType();
    string GetComponentTypeStr();
    int GetId();

    vector<Component*>* GetChildren();

    Component* GetParent();
    Component* GetChild(int _id);
    Component* FindSubcomponentById(int id, int componentType);
    int GetNumThreads();
    int GetTopoTreeDepth();//0=empty, 1=1element,...
    void GetComponentsNLevelsDeeper(vector<Component*>* outArray, int depth);
    void GetSubtreeNodeList(vector<Component*>* outArray);
    vector<DataPath*>* GetDataPaths(int orientation);

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
    Cache(int _id, int  _cache_level, int _cache_size);
    int GetCacheLevel();
    int GetCacheSize();
private:
    int cache_level;
    long long cache_size;
};

class Numa : public Component {
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
private:
};

#endif
