
#define CPUINFO
#ifdef CPUINFO

#include "Topology.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sstream>

int Node::RefreshCpuCoreFrequency()
{
    //retrieve from /proc/cpuinfo
    int fd = open("/proc/cpuinfo", O_RDONLY);
    if(fd == -1)
        return -1;

    /* Advise the kernel of our access pattern.  */
    posix_fadvise(fd, 0, 0, 1);  // FDADVICE_SEQUENTIAL

    vector<Component*> sockets = this->GetAllChildrenByType(SYS_SAGE_COMPONENT_CHIP);
    vector<Component*> cpu_hw_threads;
    for(Component * socket : sockets)
    {
        if(((Chip*)socket)->GetChipType() == SYS_SAGE_CHIP_TYPE_CPU_SOCKET || ((Chip*)socket)->GetChipType() == SYS_SAGE_CHIP_TYPE_CPU)
            socket->FindAllSubcomponentsByType(&cpu_hw_threads, SYS_SAGE_COMPONENT_THREAD);
    }

    static const auto BUFFER_SIZE = 1024*16;
    char buf[BUFFER_SIZE + 1];
    buf[BUFFER_SIZE] = '\0';
    std::stringstream file;
    string file_contents;
    while(size_t bytes_read = read(fd, buf, BUFFER_SIZE))
    {
        if (!bytes_read)
            break;
        file << buf;
    }

    string line;
    int current_thread = -1;
    size_t pos = 0;
    while (std::getline(file, line))
    {
        if (line.rfind("processor", 0) == 0)
        {
            if((pos = line.find(":")) != std::string::npos)
            {
                current_thread = stoi(line.substr(pos + 1));
            }
        }
        else if (line.rfind("cpu MHz", 0) == 0)
        {
            if((pos = line.find(":")) != std::string::npos)
            {
                bool hw_thread_found = false;
                for(Component * thread : cpu_hw_threads)
                {
                    if(thread->GetId() == current_thread)
                    {
                        Core* c = (Core*)thread->FindParentByType(SYS_SAGE_COMPONENT_CORE);
                        if(c != NULL)
                        {
                            ((Core*)c)->SetFreq(stod(line.substr(pos + 1)));
                            //cout << "setting hw thread " << current_thread << " to " << c->GetFreq() << endl;
                            hw_thread_found = true;
                            break;
                        }
                    }
                }
                if(!hw_thread_found)
                {
                    cout << "HW thread " << current_thread << " not found for refreshing the current frequency" << std::endl;
                    close(fd);
                    return 1;
                }
            }
        }
    }
    close(fd);
    return 0;
}

int Core::RefreshFreq()
{
    //retrieve from /proc/cpuinfo
    int fd = open("/proc/cpuinfo", O_RDONLY);
    if(fd == -1)
        return 1;

    /* Advise the kernel of our access pattern.  */
    posix_fadvise(fd, 0, 0, 1);  // FDADVICE_SEQUENTIAL

    static const auto BUFFER_SIZE = 1024*16;
    char buf[BUFFER_SIZE + 1];
    buf[BUFFER_SIZE] = '\0';
    std::stringstream file;
    string file_contents;
    while(size_t bytes_read = read(fd, buf, BUFFER_SIZE))
    {
        if (!bytes_read)
            break;
        file << buf;
    }

    string line;
    int current_thread = -1;
    size_t pos = 0;
    while (std::getline(file, line))
    {
        if (line.rfind("processor", 0) == 0)
        {
            if((pos = line.find(":")) != std::string::npos)
            {
                current_thread = stoi(line.substr(pos + 1));
            }
        }
        else if (current_thread == id && line.rfind("cpu MHz", 0) == 0)
        {
            if((pos = line.find(":")) != std::string::npos)
            {
                freq = stod(line.substr(pos + 1));
                //std::cout << "--   "<<  current_thread << " - freq " << stod(line.substr(pos + 1)) << std::endl;
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    std::cout << "Frequency for Processor id " << this->id << " (HW_thread) not found " << std::endl;
    return 1;
}
double Core::GetFreq() {return freq;}
void Core::SetFreq(double _freq) {freq = _freq;}
int Thread::RefreshFreq()
{
    Core * c = (Core*)this->FindParentByType(SYS_SAGE_COMPONENT_CORE);
    if(c == NULL)
        return 1;
    return c->RefreshFreq();
}
double Thread::GetFreq()
{
    Core * c = (Core*)this->FindParentByType(SYS_SAGE_COMPONENT_CORE);
    if(c == NULL)
        return -1;
    return c->GetFreq();
}
#endif
