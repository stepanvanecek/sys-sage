#include "DataPath.hpp"

DataPath::DataPath(Component* _source, Component* _target): source(_source), target(_target) {}
DataPath::DataPath(Component* _source, Component* _target, double _bw, double _latency): source(_source), target(_target), bw(_bw), latency(_latency) {}

DataPath::DataPath(Component* _source, Component* _target, int _oriented): source(_source), target(_target), oriented(_oriented) {}
DataPath::DataPath(Component* _source, Component* _target, int _oriented, double _bw, double _latency): source(_source), target(_target), oriented(_oriented), bw(_bw), latency(_latency) {}

void DataPath::Print()
{
    cout << "DataPath src: (" << source->GetComponentTypeStr() << ") id " << source->GetId() << ", target: (" << target->GetComponentTypeStr() << ") id " << target->GetId() << " - bw: " << bw << ", latency: " << latency << endl;
}
