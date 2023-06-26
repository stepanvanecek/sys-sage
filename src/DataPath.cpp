#include "DataPath.hpp"

DataPath* NewDataPath(Component* _source, Component* _target, int _oriented, int _type){
    return NewDataPath(_source,_target,_oriented,_type,(double)-1,(double)-1);
}
DataPath* NewDataPath(Component* _source, Component* _target, int _oriented, double _bw, double _latency){
    return NewDataPath(_source,_target,_oriented,SYS_SAGE_DATAPATH_TYPE_NONE,_bw,_latency);
}
DataPath* NewDataPath(Component* _source, Component* _target, int _oriented, int _type, double _bw, double _latency)
{
    DataPath* p = new DataPath(_source, _target, _oriented, _type, _bw, _latency);
    return p;
}

Component * DataPath::GetSource() {return source;}
Component * DataPath::GetTarget() {return target;}
double DataPath::GetBw() {return bw;}
double DataPath::GetLatency() {return latency;}
int DataPath::GetDpType() {return dp_type;}
int DataPath::GetOriented() {return oriented;}

DataPath::DataPath(Component* _source, Component* _target, int _oriented, int _type): DataPath(_source, _target, _oriented, _type, -1, -1) {}
DataPath::DataPath(Component* _source, Component* _target, int _oriented, double _bw, double _latency): DataPath(_source, _target, _oriented, SYS_SAGE_DATAPATH_TYPE_NONE, _bw, _latency) {}
DataPath::DataPath(Component* _source, Component* _target, int _oriented, int _type, double _bw, double _latency): source(_source), target(_target), oriented(_oriented), dp_type(_type), bw(_bw), latency(_latency)
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
