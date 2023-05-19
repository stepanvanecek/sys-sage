//#ifdef NVIDIA_MIG

#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <nvml.h>


#include "Topology.hpp"

// std::string exec(const char* cmd) 
// {
//     std::array<char, 128> buffer;
//     std::string result;
//     std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
//     if (!pipe) {
//         throw std::runtime_error("popen() failed!");
//     }
//     while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
//         result += buffer.data();
//     }
//     return result;
// }

// std::string get_Ith_word(int index, std::string s, char delim)
// {
//     istringstream iss(s);
//     //std::string word;
//     int i = 1;
//     for (std::string word; std::getline(iss, word, delim); )
//     {
//         std::cout << "--word " << i << "-" << word << '\n';
//         if(i == index){
//             std::cout << "ret:--word " << i << "-" << word << '\n';
//             return word;
//         }
//         i++;
//     }
//     return "";    
// }
// std::string get_Ith_word(int index, std::string s) //trims multiple spaces together
// {
//     istringstream iss(s);
//     std::string word;
//     int i = 1;
//     while(iss >> word) {
//         if(i == index)
//             return word;
//         i++;
//     }
//     return "";
// }

//nvmlReturn_t nvmlDeviceGetMigDeviceHandleByIndex ( nvmlDevice_t device, unsigned int  index, nvmlDevice_t* migDevice ) --> look for all mig devices and add/update them
int Chip::UpdateMIGSettings(string uuid)
{
    if(uuid.empty()){
        std::cout << "Chip::UpdateMIGSettings: " << uuid << " is empty!" << std::endl;
        return 1;
    }
	
    nvmlReturn_t ret = a ();
    if(ret != NVML_SUCCESS){std::cout << "Chip::UpdateMIGSettings: Couldn't initialize nvml. nvmlInit_v2 returns " << ret << ". Returning without updating the MIG settings." << std::endl; return 1;}
    nvmlDevice_t device;
    ret = nvmlDeviceGetHandleByUUID (uuid.c_str(), &device );
    if(ret != NVML_SUCCESS){std::cout << "Chip::UpdateMIGSettings: nvmlDeviceGetHandleByUUID returns " << ret << std::endl; return 1;}
    nvmlDeviceAttributes_t attributes;
    ret = nvmlDeviceGetAttributes_v2 ( device, &attributes );
    if(ret != NVML_SUCCESS){std::cout << "Chip::UpdateMIGSettings: nvmlDeviceGetAttributes_v2 returns " << ret << std::endl; return 1;}

    cout << "...........multiprocessorCount " << attributes.multiprocessorCount << " gpuInstanceSliceCount=" << attributes.gpuInstanceSliceCount << "  computeInstanceSliceCount=" << attributes.computeInstanceSliceCount << "    memorySizeMB=" << attributes.memorySizeMB << endl;
    //main memory
    Memory* m = (Memory*)GetChildByType(SYS_SAGE_COMPONENT_MEMORY);
    long long* mig_size;
    if(m != NULL){
        DataPath * d = new DataPath(this, m, SYS_SAGE_DATAPATH_ORIENTED, SYS_SAGE_DATAPATH_TYPE_MIG);
        string* mig_uuid = new string(uuid);
        mig_size = new long long(attributes.memorySizeMB*1000000);
        d->attrib.insert({"mig_uuid",(void*)mig_uuid});
        d->attrib.insert({"mig_size",(void*)mig_size});
    }

    // //L2 cache(s)
    size_t L2_segment_size = overallResults[L2].CacheSize.CacheSize;
    size_t L2_total_sz = cudaInfo.L2CacheSize;
    unsigned int L2_fraction = 1; //which fraction of L2 is in MIG partition (the same fraction as the fraction of main memory)
    if(m->GetSize() > *mig_size){
        L2_fraction = (m->GetSize() + (*mig_size/2)) / *mig_size; //divide and round up or down
    }
    vector<Component*> caches = GetAllChildrenByType(SYS_SAGE_COMPONENT_CACHE);
    vector<Cache*> L2_caches;
    for(Component* c : caches){
        if(((Cache*)c)->GetCacheLevel() == 2)
            L2_caches.push_back((Cache*)c);
    }
    int num_caches = L2_caches.size();
    if(num_caches > 0){
        int cache_id = 0;
        for(Cache* c : L2_caches){
            DataPath * d = new DataPath(this, c, SYS_SAGE_DATAPATH_ORIENTED, SYS_SAGE_DATAPATH_TYPE_MIG);
            string* mig_uuid = new string(uuid);
            mig_size = new long long();
            *mig_size = c->GetCacheSize()*(num_caches/L2_fraction-cache_id/num_caches);
            if(*mig_size <0)
                *mig_size=0;
            d->attrib.insert({"mig_uuid",(void*)mig_uuid});
            d->attrib.insert({"mig_size",(void*)mig_size});
            cache_id++;
        }
    }

    //sm  attributes.multiprocessorCount
    vector<Component*> subdivisions = GetAllChildrenByType(SYS_SAGE_COMPONENT_SUBDIVISION);
    vector<Subdivision*> sms;
    for(Component* sm : subdivisions){
        if(((Subdivision*)sm)->GetSubdivisionType() == SYS_SAGE_SUBDIVISION_TYPE_GPU_SM)
            sms.push_back((Subdivision*)sm);
    }
    for(Subdivision* sm: sms){
        if(sm->GetId() < attributes.multiprocessorCount){
            DataPath * d = new DataPath(this, sm, SYS_SAGE_DATAPATH_ORIENTED, SYS_SAGE_DATAPATH_TYPE_MIG);
            string* mig_uuid = new string(uuid);
            d->attrib.insert({"mig_uuid",(void*)mig_uuid});
        }
    }


    // num_sm
    // memory_gb
    // placement_start ->
    // placement_size ->
    // gpu
    // ci
    // gi
    // uuid
ret =  nvmlShutdown ( );
//     typedef struct nvmlDeviceAttributes_st
// {
//     unsigned int multiprocessorCount;       //!< Streaming Multiprocessor count
//     unsigned int sharedCopyEngineCount;     //!< Shared Copy Engine count
//     unsigned int sharedDecoderCount;        //!< Shared Decoder Engine count
//     unsigned int sharedEncoderCount;        //!< Shared Encoder Engine count
//     unsigned int sharedJpegCount;           //!< Shared JPEG Engine count
//     unsigned int sharedOfaCount;            //!< Shared OFA Engine count
//     unsigned int gpuInstanceSliceCount;     //!< GPU instance slice count
//     unsigned int computeInstanceSliceCount; //!< Compute instance slice count
//     unsigned long long memorySizeMB;        //!< Device memory size (in MiB)
// } nvmlDeviceAttributes_t;


    //std::string cmd = "nvidia-smi -L | grep " + uuid; 
    //string exec_result = exec(cmd.c_str());
    //std::string exec_result = "  MIG 2c.4g.20gb  Device  0: (UUID: MIG-d99e3a01-becd-5d26-82e0-35ace55b590c)";
    //std::cout << "exec_result --" << exec_result << "==" << std::endl;


//     std::string cmd = "nvidia-smi -L"; 
//     //string exec_result = exec(cmd.c_str());
//     std::string exec_result = "GPU 0: NVIDIA A100-PCIE-40GB (UUID: GPU-5b6ec351-6067-def5-6148-17bbe9ac6a64)\n  MIG 4g.20gb     Device  0: (UUID: MIG-d99e3a01-becd-5d26-82e0-35ace55b590c)\n  MIG 2c.3g.20gb  Device  1: (UUID: MIG-92fbbc2c-ce2e-52e1-aab4-b63b1b3fc254)\nGPU 1: NVIDIA A100-PCIE-40GB (UUID: GPU-536ec351-6067-def5-6148-17bbe9ac6a64)\n  MIG 4g.20gb     Device  0: (UUID: MIG-d99e3401-becd-5d26-82e0-35ace55b590c)\n  MIG 2c.3g.20gb  Device  1: (UUID: MIG-92f5bc2c-ce2e-52e1-aab4-b63b1b3fc254)\n";

//     std::istringstream is_exec(exec_result);
//     std::string line;
//     int gpu=-1;
//     std::string ci;
//     std::string gi;
//     while (std::getline(is_exec, line)) 
//     {
//         if(!get_Ith_word(1, line).compare("GPU")){ //gpu id entry
//             gpu = stoi(get_Ith_word(2, line));
//             std::cout << "gpu: " << gpu << std::endl;
//         }
//         if(line.find(uuid) != std::string::npos) {
//             if(gpu == -1){
//                 std::cout << "Chip::UpdateMIGSettings: " << uuid << " no belonging GPU found in nvidia-smi -L." << std::endl;
//                 return 1;
//             }
//             std::cout << "uuid found!" << '\n';
//             string mig = get_Ith_word(1, line);
//             if(mig.compare("MIG")){
//                 std::cout << "Chip::UpdateMIGSettings: " << uuid << " is not a MIG instance!" << std::endl;
//                 return 1;
//             }
//             mig = get_Ith_word(2, line);
//             std::cout << "ci+gi: " << mig << std::endl;
            
//             ci = get_Ith_word(1, mig, '.');
//             if (ci.find('c') == std::string::npos){//no CI, only GI
//                 ci="";
//                 gi=mig;
//             }
//             else {
//                 size_t gi_pos = mig.find('.');
//                 gi=mig.substr(gi_pos+1);//take the rest after the first '.'
//             }
//             std::cout << "ci: -" << ci << "-  gi: -" << gi << std::endl;
           
//             break;
//         }
//     }

//     if(gi.empty()){
//         std::cout << "Chip::UpdateMIGSettings: " << uuid << " not founf in nvidia-smi -L." << std::endl;
//         return 1;
//     }
//     cmd = "nvidia-smi mig -lgi | grep " + gi;
//     //exec_result = exec(cmd.c_str());
//     exec_result = "|   0  MIG 4g.20gb          5        1          0:4     |\n    |   0  MIG 4g.20gb          5        1          0:4     |";
//     //std::cout << "exec_result --" << exec_result << "==" << std::endl;
//     std::istringstream is_exec2(exec_result);
//     int profile_id = -1;
//     int placement_start = -1;
//     int placement_size = -1;
//     while (std::getline(is_exec2, line))
//     {
//         if(!get_Ith_word(2, line).compare(std::to_string(gpu))){ //second word is the GPU ID
//             profile_id = stoi(get_Ith_word(5, line));
//             std::string placement = get_Ith_word(7, line);
//             size_t div_pos = placement.find(':');
//             if(div_pos == string::npos){
//                 placement_start =100000;
//             }
//             else{
//                 placement_start = stoi(placement.substr(0,div_pos));
//                 placement_size = stoi(placement.substr(div_pos+1));    
//             }
//             break;
//         }
//     }

//     if(profile_id == -1){
//         std::cout << "Chip::UpdateMIGSettings: GI " << gi << " not found in nvidia-smi mig -lgi for this GPU." << std::endl;
//         return 1;
//     }
//     std::cout << "lgi << " << gi << " : profile_id=" << profile_id << ", placement_start=" << placement_start << std::endl;

//     cmd = "nvidia-smi mig -lgip | grep " + gi;
//    //exec_result = exec(cmd.c_str());
//     exec_result = "|   0  MIG 4g.20gb        5     1/1        19.50      No 56     2     0   |";
//     std::istringstream is_exec3(exec_result);
//     double memory_gb = -1;
//     int num_sm = -1;
//     while (std::getline(is_exec3, line))
//     {
//         if(!get_Ith_word(2, line).compare(std::to_string(gpu)) && !get_Ith_word(5, line).compare(std::to_string(profile_id))){ //second word is the GPU ID, fifth is the gi-profile-id
//             memory_gb = stod(get_Ith_word(7, line));
//             num_sm = stoi(get_Ith_word(9, line));
//             break;
//         }
//     }
//     if(memory_gb == -1 || num_sm == -1){
//         std::cout << "Chip::UpdateMIGSettings: GIP " << gi << " not correctly parsed number of SMs and memory size in nvidia-smi mig -lgip for this GPU." << std::endl;
//         return 1;
//     }
//     std::cout << "lgip << " << gi << " : memory_gb=" << memory_gb << ", num_sm=" << num_sm << std::endl;

//     ////
//     int first_sm_index = (num_sm/placement_size) * placement_start;



    return 0;
}
int Chip::GetMIGNumSMs()
{
    int num_sm = 0;
    for(DataPath* dp: dp_outgoing){
        if(dp->GetDpType() == SYS_SAGE_DATAPATH_TYPE_MIG){
            Component* target = dp->GetTarget();
            if(target->GetComponentType() == SYS_SAGE_COMPONENT_SUBDIVISION && ((Subdivision*)target)->GetSubdivisionType() == SYS_SAGE_SUBDIVISION_TYPE_GPU_SM ){
                num_sm++;
            }
        }
    }
    if(num_sm == 0){ //if no mig found, return number of all SMs
        vector<Component*> subdivisions;
        FindAllSubcomponentsByType(&subdivisions, SYS_SAGE_COMPONENT_SUBDIVISION);
        vector<Subdivision*> sms;
        for(Component* sm : subdivisions){
            if(((Subdivision*)sm)->GetSubdivisionType() == SYS_SAGE_SUBDIVISION_TYPE_GPU_SM)
                num_sm++;
        }   
    }
    return num_sm;
}

int Chip::GetMIGNumCores()
{
    int num_cores = 0;
    vector<Subdivision*> sms;
    vector<Component*> cores;
    for(DataPath* dp: dp_outgoing){
        if(dp->GetDpType() == SYS_SAGE_DATAPATH_TYPE_MIG){
            Component* target = dp->GetTarget();
            if(target->GetComponentType() == SYS_SAGE_COMPONENT_SUBDIVISION && ((Subdivision*)target)->GetSubdivisionType() == SYS_SAGE_SUBDIVISION_TYPE_GPU_SM ){
                sms.push_back((Subdivision*)target);
            }
        }
    }
    if(sms.size() == 0){ //if no mig found, return number of all SMs
        vector<Component*> subdivisions;
        FindAllSubcomponentsByType(&subdivisions, SYS_SAGE_COMPONENT_SUBDIVISION);
        for(Component* sm : subdivisions){
            if(((Subdivision*)sm)->GetSubdivisionType() == SYS_SAGE_SUBDIVISION_TYPE_GPU_SM)
                sms.push_back((Subdivision*)sm);
        }   
    }

    for(sm: sms)
    {
        FindAllSubcomponentsByType(&cores, SYS_SAGE_COMPONENT_THREAD);
    }
    
    return cores.size();
}

long long Memory::GetMIGSize()
{
    for(DataPath* dp: dp_incoming){
        if(dp->GetDpType() == SYS_SAGE_DATAPATH_TYPE_MIG){
            if (d->attrib.count("mig_size")){
                long long r = *(Long long*)d->attrib["mig_size"];
                return r;
            }
        }
    }
    return cache_size;
}

long long Cache::GetMIGSize()
{
    if(GetCacheLevel() == 2){
        for(DataPath* dp: dp_incoming){
            if(dp->GetDpType() == SYS_SAGE_DATAPATH_TYPE_MIG){
                if (d->attrib.count("mig_size")){
                    long long r = *(Long long*)d->attrib["mig_size"];
                    return r;
                }
            }
        }
    }
    return cache_size;
}

/*
1) nvidia-smi -L | grep ..UUID
-> read out MIG 2c.4g.20gb => CI=2c, GI=4g.20gb
2) nvidia-smi mig -lgi | grep GI
=> read out profile_ID, placement+size (which L2/SM-ids to use)
3) nvidia-smi mig -lgip | grep GI, grep profile_ID
-> read out num SMs, memory size, extrapolate L2 size


nvidia-smi mig -lgi
+-------------------------------------------------------+
| GPU instances:                                        |
| GPU   Name             Profile  Instance   Placement  |
|                          ID       ID       Start:Size |
|=======================================================|
|   0  MIG 3g.20gb          9        2          4:4     |
+-------------------------------------------------------+
|   0  MIG 4g.20gb          5        1          0:4     |
+-------------------------------------------------------+

nvidia-smi mig -lgip
+-----------------------------------------------------------------------------+
| GPU instance profiles:                                                      |
| GPU   Name             ID    Instances   Memory     P2P    SM DEC   ENC  |
|                              Free/Total   GiB              CE JPEG  OFA  |
|=============================================================================|
|   0  MIG 1g.5gb        19     7/7        4.75       No 14     0     0   |
| 1     0     0   |
+-----------------------------------------------------------------------------+
|   0  MIG 1g.5gb+me     20     1/1        4.75       No 14     1     0   |
| 1     1     1   |
+-----------------------------------------------------------------------------+
|   0  MIG 2g.10gb       14     3/3        9.62       No 28     1     0   |
| 2     0     0   |
+-----------------------------------------------------------------------------+
|   0  MIG 3g.20gb        9     2/2        19.50      No 42     2     0   |
| 3     0     0   |
+-----------------------------------------------------------------------------+
|   0  MIG 4g.20gb        5     1/1        19.50      No 56     2     0   |
| 4     0     0   |
+-----------------------------------------------------------------------------+
|   0  MIG 7g.40gb        0     1/1        39.25      No 98     5     0   |
| 7     1     1   |
+-----------------------------------------------------------------------------+


nvidia-smi mig -gi 1 -lcip
+--------------------------------------------------------------------------------------+
| Compute instance profiles: |
| GPU     GPU       Name             Profile  Instances Exclusive       Shared       |
|       Instance                       ID     Free/Total SM       DEC   ENC   OFA  |
| ID                                                          CE JPEG       |
|======================================================================================|
|   0      1       MIG 1c.4g.20gb       0      4/4 14        2     0     0   |
| 4     0         |
+--------------------------------------------------------------------------------------+
|   0      1       MIG 2c.4g.20gb       1      2/2 28        2     0     0   |
| 4     0         |
+--------------------------------------------------------------------------------------+
|   0      1       MIG 4g.20gb          3*     1/1 56        2     0     0   |
| 4     0         |
+--------------------------------------------------------------------------------------+
*/
//#endif