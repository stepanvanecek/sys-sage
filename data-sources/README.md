# sys-sage Data Sources

## hwloc

//TODO

## mt4g

Retrieves compute and memory topology of GPUs (NVidia only for now). See the project's [git repository](https://github.com/caps-tum/mt4g), especially the Readme, for more information on how to use it.

To enable the **mt4g** data source, add `-DDS_MT4G=ON` to the cmake script.
You may also need to specify the nvcc compiler path `-DCMAKE_CUDA_COMPILER=/path/to/nvcc`.
Next, it is necessary to specify the target microarchitecture `-DNVIDIA_UARCH=<arch>`.
NVIDIA_UARCH options are: KEPLER, MAXWELL, PASCAL, VOLTA, TURING, AMPERE

Example cmake command to build sys-sage with mt4g data source:
```
cmake -DCMAKE_INSTALL_PREFIX=../inst-dir -DDS_MT4G=ON -DNVIDIA_UARCH="volta" -DCMAKE_CUDA_COMPILER=/usr/local/cuda-11.7/bin/nvcc ..
```

## caps-numa-benchmark

//TODO
