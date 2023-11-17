#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

extern "C" {
    #include <pci/pci.h>
    #include <pci/types.h>
}

#include "utils.hpp" 

using std::string;
using std::ifstream;

void get_osrelease(string &id, string &pretty_name) {
    string buf;
    ifstream os_release_file;

    id = "";
    pretty_name = "";

    os_release_file.open("/etc/os-release");
    if (os_release_file.fail())
        return;

    // parse os-release
    while (getline(os_release_file, buf)) {
        // exit when both has been found
        if (!id.empty() && !pretty_name.empty())
            break;
        
        if (buf.rfind("ID=", 0) == 0) {
            id = buf.substr(3);
            continue;
        }
        else if (buf.rfind("PRETTY_NAME=", 0) == 0) {
            pretty_name = buf.substr(12);
            continue;
        }
        else if (buf.rfind("NAME=", 0) == 0) {
            pretty_name = buf.substr(5);
            continue;
        }
    }
    os_release_file.close();

    erase_all(pretty_name, '"');
    if (pretty_name.empty())
        pretty_name = id;
}

void get_kernel(string &kernel, string &release) {
    kernel = "unknown";
    release = "unknown";

    ifstream kernel_file;
    kernel_file.open("/proc/sys/kernel/ostype");
    if (!kernel_file.fail())
        getline(kernel_file, kernel);
    kernel_file.close();
    kernel_file.open("/proc/sys/kernel/osrelease");
    if (!kernel_file.fail())
        getline(kernel_file, release);
    kernel_file.close();
}

void get_cpu(string &cpu) {
    string buf;

    cpu = "unknown";

    ifstream cpuinfo_file;
    cpuinfo_file.open("/proc/cpuinfo");
    if (cpuinfo_file.fail())
        return;

    while (getline(cpuinfo_file, buf)) {
        if (buf.rfind("model name", 0) == 0) {
            std::size_t colon_pos = buf.find(":") + 2;
            cpu = buf.substr(colon_pos);
            break;
        }
    }
    cpuinfo_file.close();
}

void get_gpu(std::vector<string> &GPUs) {
    struct pci_access *pciaccess = pci_alloc();
    pci_init(pciaccess);
    pci_scan_bus(pciaccess);

    char gpu[1024] = {0}; 
    std::size_t gpu_len; 
    for (struct pci_dev *dev = pciaccess->devices; dev; dev = dev->next) {
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_CLASS | PCI_FILL_LABEL);
        switch(dev->device_class) {
            case 0x0380:
            case 0x0301:
            case 0x0302:
            case 0x0300:
                // read PCI device vendor name
                pci_lookup_name(pciaccess, gpu, 512, PCI_LOOKUP_VENDOR, dev->vendor_id);
                // add space after vendor
                gpu_len = strlen(gpu);
                gpu[gpu_len] = ' ';
                // read PCI device name
                pci_lookup_name(pciaccess, &gpu[gpu_len + 1], 510, PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
                GPUs.push_back(gpu);
                break;
            default:
                break;
        }
    }
}

void get_mem(int(&mem)[2]) {
    string buf, memtotal, memavailable;

    ifstream meminfo_file;
    meminfo_file.open("/proc/meminfo");
    if (meminfo_file.fail()) {
        mem[0] = 0;
        mem[1] = 0;
        return;
    }

    while (getline(meminfo_file, buf)) {
        if (!memtotal.empty() && !memavailable.empty()) {
            meminfo_file.close();
            break;
        }
        else if (buf.rfind("MemTotal:", 0) == 0) {
            memtotal = &buf.c_str()[9];
            continue;
        }
        else if (buf.rfind("MemAvailable:", 0) == 0) {
            memavailable = &buf.c_str()[14];
            continue;
        }
    }

    try {
        mem[0] = stoi(memtotal, nullptr, 10) / 1024;
        mem[1] = stoi(memavailable, nullptr, 10) / 1024;
        mem[1] = mem[0] - mem[1];
    } catch (std::exception ex) {
        ERR("There was a error reading meminfo");
        mem[0] = 0;
        mem[1] = 0;
    }
}

void get_shell(string &shell) {
    char *env = getenv("SHELL");
    if (env == nullptr)
        shell = "unknown";
    else
        shell = env;
}

int main(int argc, char** argv) {
    string distro, pretty_name, kernel, kernel_release, cpu, shell;
    std::vector<string> GPUs;
    // 0:memtotal 1:memused
    int mem[2] = {0,0};

    get_osrelease(distro, pretty_name);
    DBG("distro: " << distro);
    DBG("pretty name: " << pretty_name);

    get_kernel(kernel, kernel_release);
    DBG("kernel: " << kernel << " " << kernel_release);

    get_cpu(cpu);
    DBG("CPU: " << cpu);

    get_gpu(GPUs);
    for (string &gpu : GPUs)
        DBG("GPU: " << gpu);

    get_mem(mem);
    DBG("mem: " << mem[1] << "MiB/" << mem[0] << "MiB");

    get_shell(shell);
    DBG("shell: " << shell);
}
