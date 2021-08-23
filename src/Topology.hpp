#ifndef TOPOLOGY
#define TOPOLOGY

#include <iostream>
#include <vector>


using namespace std;

class Node;

//carries the information about the whole system

class Component {
public:
    Component();
    Component(int,string);
    void InsertChild(Component * child);
    void PrintSubtree(int level);

    string GetName();
    vector<Component*>* GetChildren();
    Component* GetChild(int _id);

    void SetParent(Component* parent);

protected:
    int id;
    string name;
    vector<Component*> children;
    Component* parent;
    vector<string> metadata;
private:
};

class Topology : public Component {
public:
    Topology();
    ~Topology();

//    vector<Node*> GetNodes();
    // Node* GetNodeById(int id);
    // Node* GetNodeByIndex(int index);

    void DestroyTopo();

    //string name;
    //vector<string> metadata;
private:
    // int id;
    // vector<Node*> nodes;
};

class Node : Component {
public:
    Node();
private:
};

class Chip : Component {
public:
    Chip();
    Chip(int _id);
private:
};

class Cache : Component {
public:
    Cache();
    Cache(int _id, int  _cache_level, int _cache_size);
private:
    int cache_level;
    int cache_size;
};

class Numa : Component {
public:
    Numa();
    Numa(int _id);
private:
};

class Core : Component {
public:
    Core();
    Core(int _id);
private:
};

class Thread : Component {
public:
    Thread();
    Thread(int _id);
private:
};

#endif
