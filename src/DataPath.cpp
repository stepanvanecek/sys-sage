#include "DataPath.hpp"

DataPath* NewDataPath(Component* _source, Component* _target, int _oriented, int _type){
    return NewDataPath(_source,_target,_oriented,_type,(double)0,(double)0);
}
DataPath* NewDataPath(Component* _source, Component* _target, int _oriented, double _bw, double _latency){
    return NewDataPath(_source,_target,_oriented,SYS_SAGE_DATAPATH_TYPE_NONE,_bw,_latency);
}
DataPath* NewDataPath(Component* _source, Component* _target, int _oriented, int _type, double _bw, double _latency)
{
    DataPath* p = new DataPath(_source, _target, _oriented, _type, _bw, _latency);
}

Component * DataPath::GetSource() {return source;}
Component * DataPath::GetTarget() {return target;}
double DataPath::GetBw() {return bw;}
double DataPath::GetLatency() {return latency;}
int DataPath::GetDpType() {return dp_type;}
int DataPath::GetOriented() {return oriented;}

DataPath::DataPath(Component* _source, Component* _target, int _oriented, int _type): source(_source), target(_target), oriented(_oriented), dp_type(_type)
{
    if(_oriented == SYS_SAGE_DATAPATH_BIDIRECTIONAL)
    {
        _source->AddDataPath(this, SYS_SAGE_DATAPATH_OUTGOING);
        _target->AddDataPath(this, SYS_SAGE_DATAPATH_OUTGOING);
        _source->AddDataPath(this, SYS_SAGE_DATAPATH_INCOMING);
        _target->AddDataPath(this, SYS_SAGE_DATAPATH_INCOMING);
    }
    else if(_oriented == SYS_SAGE_DATAPATH_ORIENTED)
    {
        _source->AddDataPath(this, SYS_SAGE_DATAPATH_OUTGOING);
        _target->AddDataPath(this, SYS_SAGE_DATAPATH_INCOMING);
    }
    else
    {
        delete this;
        return;//error
    }
}
DataPath::DataPath(Component* _source, Component* _target, int _oriented, double _bw, double _latency): DataPath(_source, _target, _oriented, SYS_SAGE_DATAPATH_TYPE_NONE), bw(_bw), latency(_latency) {}
DataPath::DataPath(Component* _source, Component* _target, int _oriented, int _type, double _bw, double _latency): DataPath(_source, _target, _oriented, _type), bw(_bw), latency(_latency) {}

void DataPath::Print()
{
    cout << "DataPath src: (" << source->GetComponentTypeStr() << ") id " << source->GetId() << ", target: (" << target->GetComponentTypeStr() << ") id " << target->GetId() << " - bw: " << bw << ", latency: " << latency;
    if(!attrib.empty())
    {
        cout << " - attrib: ";
        for (const auto& n : attrib) {
            uint64_t* val = (uint64_t*)n.second;
            std::cout << n.first << " = " << *val << "; ";
        }
    }
    cout << endl;
}
