#include "DataPath.hpp"

DataPath* NewDataPath(Component* _source, Component* _target, int _oriented, int _type){
    return NewDataPath(_source,_target,_oriented,_type,(double)0,(double)0);
}
DataPath* NewDataPath(Component* _source, Component* _target, int _oriented, double _bw, double _latency){
    return NewDataPath(_source,_target,_oriented,SYS_SAGE_DATAPATH_TYPE_NONE,_bw,_latency);
}
DataPath* NewDataPath(Component* _source, Component* _target, int _oriented, int _type, double _bw, double _latency)
{
    //cout << "inserting datapath bw " << _bw << "src id " << _source->GetId() << " target id " << _target->GetId() <<  endl;
    DataPath* p = new DataPath(_source, _target, _oriented, _type, _bw, _latency);
    if(_oriented == SYS_SAGE_DATAPATH_BIDIRECTIONAL)
    {
        _source->AddDataPath(p, SYS_SAGE_DATAPATH_OUTGOING);
        _target->AddDataPath(p, SYS_SAGE_DATAPATH_OUTGOING);
        _source->AddDataPath(p, SYS_SAGE_DATAPATH_INCOMING);
        _target->AddDataPath(p, SYS_SAGE_DATAPATH_INCOMING);
        return p;//ok
    }
    else if(_oriented == SYS_SAGE_DATAPATH_ORIENTED)
    {
        _source->AddDataPath(p, SYS_SAGE_DATAPATH_OUTGOING);
        _target->AddDataPath(p, SYS_SAGE_DATAPATH_INCOMING);
        return p;//ok
    }
    else
    {
        delete p;
        return NULL;//error
    }
}

Component * DataPath::GetSource() {return source;}
Component * DataPath::GetTarget() {return target;}
double Component::GetLatency() {return latency;}
double Component::GetBw() {return bw;}

DataPath::DataPath(Component* _source, Component* _target, int _oriented, int _type): source(_source), target(_target), oriented(_oriented), dp_type(_type) {}
DataPath::DataPath(Component* _source, Component* _target, int _oriented, double _bw, double _latency): source(_source), target(_target), oriented(_oriented), dp_type(SYS_SAGE_DATAPATH_TYPE_NONE), bw(_bw), latency(_latency) {}
DataPath::DataPath(Component* _source, Component* _target, int _oriented, int _type, double _bw, double _latency): source(_source), target(_target), oriented(_oriented), dp_type(_type), bw(_bw), latency(_latency) {}
void DataPath::Print()
{
    cout << "DataPath src: (" << source->GetComponentTypeStr() << ") id " << source->GetId() << ", target: (" << target->GetComponentTypeStr() << ") id " << target->GetId() << " - bw: " << bw << ", latency: " << latency;
    if(!metadata.empty())
    {
        cout << " - metadata: ";
        for (const auto& n : metadata) {
            uint64_t* val = (uint64_t*)n.second;
            std::cout << n.first << " = " << *val << "; ";
        }
    }
    cout << endl;
}
