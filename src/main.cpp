#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <array>
#include <cstring>
#include <cstdlib>

extern "C" {
#ifndef NO_PCI
    #include <pci/pci.h>
    #include <pci/types.h>
#endif
}

#include "utils.hpp" 

using std::string;
using std::ifstream;

/*enum InfoLineType {
    distro,
    pretty_name,
    kernel,
    CPU,
    GPU,
    memory,
    shell,
    sh,
    str
};

struct InfoLine {
    InfoLineType type;
    string line;
};*/

struct Config {
    string path;
    std::map<string, string> ascii_art = {
        {
            "unknown",
            "     #####\n    ##O#O##\n    #######\n  ###########\n #############\n###############\n #############\n  ###########\n"
        },
        {
            "arch",
            "      /\\\n     /  \\\n    /\\   \\\n   /      \\\n  /   __   \\\n /   |  |  -\\\n/_-''    ''-_\\\n"
        },
        {
            "debian",
            "  _____\n /  ___ \\\n|  /     |\n|  \\____/\n -_\n   -._\n"
        },
        {
            "ubuntu",
            "         _\n     ---(_)\n _/  ---  \\\n(_) |   |\n  \\  --- _/\n     ---(_)\n"
        },
        {
            "linuxmint",
            " ______________\n|__             \\\n   | | _______   |\n   | |  |  |  |  |\n   | |  |  |  |  |\n   | \\________/  |\n   \\____________/\n"
        },
        {
            "pop",
            "______\n\\   _ \\        __\n \\ \\ \\ \\      / /\n  \\ \\_\\ \\    / /\n   \\  ___\\  /_/\n    \\ \\    _ \n   __\\_\\__(_)_ \n  (___________)`\n"
        },
        {
            "gentoo",
            " _-----_\n(       \\\n\\    0   \\\n \\        )\n /      _/\n(     _-\n\\____-\n"
        }
    };
    std::vector<string> lines;
};

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

#ifndef NO_PCI
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
#else
void get_gpu(std::vector<string> &GPUs) {}
#endif

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

//TODO add proper config parsing
void read_conf(struct Config &conf) {
    conf.lines = {
        " OS     ->  %NAME%",
        " Kernel ->  %KERNEL%",
        " Shell  ->  %SHELL%",
        " CPU    ->  %CPU%",
        " GPU    ->  %GPU%",
        " Memory ->  %MEMUSED%MiB/%MEMTOTAL%MiB (%MEMPERCENT%%)",
        " SH$(printf \"$((5 + 5))\")"};
}

std::array<std::size_t, 2> print_art(struct Config conf, string distro) {
    string art = conf.ascii_art["unknown"];
    if (conf.ascii_art.count(distro) > 0) {
        art = conf.ascii_art[distro];
    }

    string buf;
    std::size_t max_length = 0;
    std::size_t lines = 0;
    std::istringstream art_stream(art);
    while (getline(art_stream, buf)) {
        lines++;
        std::size_t len = buf.length();
        if (max_length < len)
            max_length = len;
        std::cout << buf << "\n";
    }
    return {lines, max_length};
}

/*void print_info(struct Config conf, std::array<std::size_t, 2> art_size) {
}*/

int main(int argc, char** argv) {
    // config
    struct Config conf;
    read_conf(conf);

    // info variables
    string distro, pretty_name, kernel, kernel_release, cpu, shell;
    std::vector<string> GPUs;
    int mem[2] = {0,0}; // 0:memtotal 1:memused

    // get info
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

    // print art
    std::array<std::size_t, 2> art_size = print_art(conf, distro);
    // print info
    MOVE_CUR_UP(art_size[0]);

    int total_info_lines;
    for(int i = 0; i < conf.lines.size(); i++) {
        MOVE_CUR_RIGHT(art_size[1]);
        string line = conf.lines[i];
        replaceAll(line, "%DISTRO%", distro);
        replaceAll(line, "%NAME%", pretty_name);
        replaceAll(line, "%KERNEL%", kernel + " " + kernel_release);
        replaceAll(line, "%KERNEL_TYPE%", kernel);
        replaceAll(line, "%KERNEL_RELEASE%", kernel_release);
        replaceAll(line, "%CPU%", cpu);
        replaceAll(line, "%MEMTOTAL%", std::to_string(mem[0]));
        replaceAll(line, "%MEMUSED%", std::to_string(mem[1]));
        replaceAll(line, "%MEMPERCENT%", std::to_string((mem[1] * 100) / mem[0]));
        replaceAll(line, "%SHELL%", shell);
        if (line.find("%GPU%") != string::npos) {
            for (string gpu : GPUs) {
                total_info_lines++;
                replaceAll(line, "%GPU%", gpu);
                std::cout << line << "\n";
            }
            continue;
        }

        std::cout << line << "\n";
        total_info_lines++;
    }

    int lines_left = art_size[0] - total_info_lines;
    if (lines_left > 0)
        MOVE_CUR_DOWN(lines_left + 1);
    std::cout << COL_RESET;
}
