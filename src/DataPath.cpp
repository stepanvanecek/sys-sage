 #include "DataPath.hpp"

 DataPath::DataPath(Component* _source, Component* _target): source(_source), target(_target) {}
 DataPath::DataPath(Component* _source, Component* _target, double _bw, double _latency): source(_source), target(_target), bw(_bw), latency(_latency)
