#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#ifndef NO_PCI
#include <pci/pci.h>
#include <pci/types.h>
#endif
#include "config.h"   // config
#include "language.h" // language

#define VERSION "1.0.1-Bugfix"

int starts_with(const char *prefix, const char *str)
{
    size_t prefix_len = strlen(prefix);
    size_t str_len = strlen(str);
    return str_len < prefix_len ? 0 : memcmp(prefix, str, prefix_len) == 0;
}

char *get_distro()
{
    // allocate memory
    char *distro = (char *)calloc(32, 32);

    // Reading /etc/os-release
    FILE *os_release_fp = fopen("/etc/os-release", "r");
    if (os_release_fp == NULL)
        abort();

    char *os_release_buf = (char *)calloc(512, 512);
    // read /etc/os-release line by line
    while (fgets(os_release_buf, 1024, os_release_fp) != NULL)
    {
        // check if string starts with ID=
        if (starts_with("ID=", os_release_buf))
        {
            // cut out ID
            strcpy(distro, &os_release_buf[3]);
            size_t distro_len = strlen(distro);
            if (distro[distro_len - 1] == '\n')
                distro[distro_len - 1] = '\0';
            break;
        }
    }
    fclose(os_release_fp);
    free(os_release_buf);
    return distro;
}

char *get_os(char *distro)
{
    // allocate memory
    char *os = (char *)calloc(32, 32);
    strcpy(os, LANGUAGE_UNKNOWN);

    // parse distro/os name
    if (distro == NULL || !distro || strlen(distro) == 0)
    {
        return os;
    }
    else
    {
        if (strcmp(distro, "arch") == 0)
        {
            strcpy(os, "Arch Linux");
        }
        else if (strcmp(distro, "debian") == 0)
        {
            strcpy(os, "Debian Linux");
        }
        else if (strcmp(distro, "ubuntu") == 0)
        {
            strcpy(os, "Ubuntu Linux");
        }
        else if (strcmp(distro, "linuxmint") == 0)
        {
            strcpy(os, "Linux Mint");
        }
        else if (strcmp(distro, "linuxlite") == 0)
        {
            strcpy(os, "Linux Lite");
        }
        else if (strcmp(distro, "pop") == 0)
        {
            strcpy(os, "Pop!_OS");
        }
    }

    return os;
}

char *get_kernel()
{
    // allocate memory
    char *kernel = (char *)calloc(32, 32);

    // parse /proc/sys/kernel/ostype
    FILE *ostype_fp = fopen("/proc/sys/kernel/ostype", "r");
    if (ostype_fp == NULL)
        abort();
    char *ostype_buf = (char *)calloc(32, 32);
    if (fgets(ostype_buf, 32, ostype_fp) != NULL)
    {
        size_t ostype_buf_len = strlen(ostype_buf);
        if (ostype_buf[ostype_buf_len - 1] == '\n')
            ostype_buf[ostype_buf_len - 1] = '\0';
        strcpy(kernel, ostype_buf);
        fclose(ostype_fp);

        // parse /proc/sys/kernel/osrelease
        FILE *osrelease_fp = fopen("/proc/sys/kernel/osrelease", "r");
        if (osrelease_fp == NULL)
            abort();
        if (fgets(ostype_buf, 31, osrelease_fp) != NULL)
        {
            size_t osrelease_len = strlen(ostype_buf);
            if (ostype_buf[osrelease_len - 1] == '\n')
                ostype_buf[osrelease_len - 1] = '\0';
            strcpy(&kernel[ostype_buf_len - 1], " ");
            strcpy(&kernel[ostype_buf_len], ostype_buf);
        }
        else
        {
            strcpy(kernel, LANGUAGE_UNKNOWN);
        }
    }
    else
    {
        strcpy(kernel, LANGUAGE_UNKNOWN);
    }
    free(ostype_buf);
    return kernel;
}

char *get_cpu()
{
    // allocate memory
    char *cpu = (char *)malloc(128);

    // parse /proc/cpuinfo
    FILE *cpuinfo_fp = fopen("/proc/cpuinfo", "r");
    if (cpuinfo_fp == NULL)
        abort();
    char *cpuinfo_buf = (char *)malloc(1024);
    memset(cpuinfo_buf, 0, 1);
    while (fgets(cpuinfo_buf, 1024, cpuinfo_fp) != NULL)
    {
        if (starts_with("model name", cpuinfo_buf))
        {
            size_t cpuinfo_len = strlen(&cpuinfo_buf[13]);
            strcpy(cpu, &cpuinfo_buf[13]);
            if (cpu[cpuinfo_len - 1] == '\n')
                cpu[cpuinfo_len - 1] = '\0';
            break;
        }
    }
    fclose(cpuinfo_fp);
    free(cpuinfo_buf);
    return cpu;
}

#ifndef NO_PCI
char *get_gpu()
{
    // allocate memory
    char *gpu = (char *)calloc(128, 128);
    memset(gpu, '?', 5);

    // parse PCI devices
    struct pci_access *pciaccess = pci_alloc();
    pci_init(pciaccess);
    pci_scan_bus(pciaccess);

    for (struct pci_dev *dev = pciaccess->devices; dev; dev = dev->next)
    {
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_CLASS | PCI_FILL_LABEL);
        switch (dev->device_class)
        {
        case 0x0380:
        case 0x0301:
        case 0x0302:
        case 0x0300:
            pci_lookup_name(pciaccess, gpu, 63,
                            PCI_LOOKUP_VENDOR, dev->vendor_id);
            size_t gpu_len = strlen(gpu);
            gpu[gpu_len] = ' ';
            pci_lookup_name(pciaccess, &gpu[gpu_len + 1], 64,
                            PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
            break;
        default:
            break;
        }
        if (strlen(gpu) > 5 || strlen(gpu) < 5)
            break;
    }
    pci_cleanup(pciaccess);
    return gpu;
}
#endif

void get_mem(int *mem_total, int *mem_used)
{
    // parse /proc/meminfo
    char *mem_total_str = (char *)calloc(64, 64);
    char *mem_available_str = (char *)calloc(64, 64);
    FILE *meminfo_fp = fopen("/proc/meminfo", "r");
    if (meminfo_fp == NULL)
        abort();
    char *meminfo_buf = (char *)calloc(1024, 1024);
    while (fgets(meminfo_buf, 1024, meminfo_fp) != NULL)
    {
        if (starts_with("MemTotal", meminfo_buf))
        {
            char *line = &meminfo_buf[9];
            for (int i = 0; i < strlen(line); i++)
            {
                if (strlen(mem_total_str) != 0)
                {
                    break;
                }
                switch (line[i])
                {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '0':
                    strcpy(mem_total_str, &line[i]);
                    break;
                default:
                    break;
                }
            }
        }
        else if (starts_with("MemAvailable", meminfo_buf))
        {
            char *line = &meminfo_buf[13];
            for (int i = 0; i < strlen(line); i++)
            {
                if (strlen(mem_available_str) != 0)
                {
                    break;
                }
                switch (line[i])
                {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '0':
                    strcpy(mem_available_str, &line[i]);
                    break;
                default:
                    break;
                }
            }
        }
        if (strlen(mem_total_str) != 0 && strlen(mem_available_str) != 0)
        {
            break;
        }
    }
    int mem_available = atoi(mem_available_str) / 1024;
    int total = atoi(mem_total_str) / 1024;
    memcpy(mem_total, &total, sizeof(int));
    int used = total - mem_available;
    memcpy(mem_used, &used, sizeof(int));
    fclose(meminfo_fp);
    free(meminfo_buf);
    free(mem_total_str);
    free(mem_available_str);
}

void print_distro(char *distro)
{
    printf(DISTRO_COLOR_CODE);
    if (strcmp(distro, "arch") == 0)
    {
        printf(ARCH);
    }
    else if (strcmp(distro, "debian") == 0)
    {
        printf(DEBIAN);
    }
    else if (strcmp(distro, "ubuntu") == 0)
    {
        printf(UBUNTU);
    }
    else if (strcmp(distro, "linuxmint") == 0)
    {
        printf(LINUXMINT);
    }
    else if (strcmp(distro, "linuxlite") == 0)
    {
        printf(LINUXLITE);
    }
    else if (strcmp(distro, "pop") == 0)
    {
        printf(POPOS);
    }
    else
    {
        printf(UNKNOWN);
    }
}

#ifdef NO_PCI
void print_info(char *os, char *kernel, char *CPU, int mem_used, int mem_total)
{
    printf("\x1b[10F");
    printf(TEMPLATE, os, kernel, CPU, mem_used, mem_total);
    printf("\x1b[4E");
}
#else
void print_info(char *os, char *kernel, char *CPU, char *GPU, int mem_used, int mem_total)
{
    printf("\x1b[10F");
    printf(TEMPLATE, os, kernel, CPU, GPU, mem_used, mem_total);
    printf("\x1b[4E");
}
#endif

int main(int argc, char **argv)
{
    // create distro variable
    char *distro = NULL;
    int print_all = -1;

    // parse command line args
    for (int i = 1; i < argc; i++)
    {
        // print just the info specified
        if (strcmp(argv[i], "os") == 0)
        {
            distro = get_distro();
            char *os = get_os(distro);
            printf("%s\n", os);
            free(os);
            free(distro);
            print_all = 0;
        }
        else if (strcmp(argv[i], "kernel") == 0)
        {
            char *kernel = get_kernel();
            printf("%s\n", kernel);
            free(kernel);
            print_all = 0;
        }
        else if (strcmp(argv[i], "cpu") == 0)
        {
            char *CPU = get_cpu();
            printf("%s\n", CPU);
            free(CPU);
            print_all = 0;
        }
        else if (strcmp(argv[i], "gpu") == 0)
        {
#ifndef NO_PCI
            char *GPU = get_gpu();
            printf("%s\n", GPU);
            free(GPU);
            print_all = 0;
#else
            printf(LANGUAGE_GPU_UNSUPPORTED);
            return 1;
#endif
        }
        else if (strcmp(argv[i], "mem") == 0 || strcmp(argv[i], "memory") == 0)
        {
            int mem_total, mem_used;
            get_mem(&mem_total, &mem_used);
            printf("%dMiB/%dMiB\n", mem_used, mem_total);
            print_all = 0;
        }

        // parse other arguments
        else if (strcmp("-d", argv[i]) == 0 || strcmp("--distro", argv[i]) == 0)
        {
            if ((i + 1) < argc)
            {
                distro = (char *)calloc(512, 512);
                strcpy(distro, argv[i + 1]);
                i++;
            }
        }
        else if (strcmp("-v", argv[i]) == 0 || strcmp("--version", argv[i]) == 0)
        {
            puts("fatch v" VERSION);
            puts("Compiled at " __DATE__ " " __TIME__);
            return 0;
        }
        else if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0)
        {
            printf(LANGUAGE_USAGE, argv[0]);
            return 0;
        }

        // print usage otherwise
        else
        {
            printf(LANGUAGE_INVALID_ARGUMENT, argv[i]);
            printf(LANGUAGE_USAGE, argv[0]);
            return 1;
        }
    }

    if (print_all == 0)
    {
        return 0;
    }

    // memory stuff
    char *os = NULL;
    char *kernel = NULL;
    char *cpu = NULL;
#ifndef NO_PCI
    char *gpu = NULL;
#endif

    // get distro
    if (distro == NULL)
    {
        distro = get_distro();
    }
    // get OS
    os = get_os(distro);
    // get kernel
    kernel = get_kernel();
    // get CPU
    cpu = get_cpu();

#ifndef NO_PCI
    // get GPU
    gpu = get_gpu();
#endif

    // get meminfo
    int mem_total;
    int mem_used;
    get_mem(&mem_total, &mem_used);

    // print distro
    print_distro(distro);

// print info
#ifdef NO_PCI
    print_info(os, kernel, cpu, mem_used, mem_total);
#else
    print_info(os, kernel, cpu, gpu, mem_used, mem_total);
#endif

    // cleanup
    free(os);
    free(distro);
    free(kernel);
    free(cpu);
#ifndef NO_PCI
    free(gpu);
#endif
}