#define NVIDIA_MIG
#ifdef NVIDIA_MIG

#include <iostream>
#include <sstream>
#include <string>
#include <array>

#include "Topology.hpp"

std::string exec(const char* cmd) 
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string get_Ith_word(int index, std::string s)
{
    istringstream iss(s);
    std::string word;
    int i = 1;
    while(iss >> word) {
        if(i == index)
            return word;
        /* do stuff with word */
    }
    return "";
}

int Chip::UpdateMIGSettings(string uuid)
{
    std::string cmd = "nvidia-smi -L | grep " + uuid;
    //string exec_result = exec(cmd.c_str());
    std::string exec_result = "  MIG 2c.4g.20gb  Device  0: (UUID: MIG-d99e3a01-becd-5d26-82e0-35ace55b590c)";

    std::string s = get_Ith_word(2, exec_result);
    std::cout << "2nd: " << s << std::endl;
    return 0;
}
int Chip::GetMIGNumSMs(){return 0;}
int Chip::GetMIGNumCores(){return 0;}
int Memory::GetMIGSize(){return 0;}
int Cache::GetMIGSize(){return 0;}

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
#endif