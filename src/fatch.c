#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#ifdef __unix__
#include <sys/ioctl.h>
#else
#include <fcntl.h>
#endif
#ifndef NO_PCI
#include <pci/pci.h>
#include <pci/types.h>
#endif
#include "config.h"   // config
#include "language.h" // language

#define VERSION "3.0.0-Dev"

#ifndef NO_PCI
struct gpu_buffer {
	size_t count;
	char *buffer;
};
#endif

int starts_with(const char *prefix, const char *str)
{
	size_t prefix_len = strlen(prefix);
	size_t str_len = strlen(str);
	return str_len < prefix_len ? 0 : memcmp(prefix, str, prefix_len) == 0;
}

char *get_distro()
{
	// allocate memory
	char *distro = (char *)calloc(1, 4096);

	// Reading /etc/os-release
	FILE *os_release_fp = fopen("/etc/os-release", "r");
	if (os_release_fp == NULL)
		abort();

	char *os_release_buf = (char *)calloc(1, 4096);
	// read /etc/os-release line by line
	while (fgets(os_release_buf, 4096, os_release_fp) != NULL)
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

// TODO add distro enum to optimise get_os and print_distro 
//      from 14 strcmp calls to 7 with the enum and a switch
char *get_os(char *distro)
{
	// allocate memory
	char *os = (char *)calloc(1, 4096);
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
		else if (strcmp(distro, "gentoo") == 0)
		{
			strcpy(os, "Gentoo Linux");
		}
	}

	return os;
}

char *get_kernel()
{
	// allocate memory
	char *kernel = (char *)calloc(1, 4096);

	// parse /proc/sys/kernel/ostype
	FILE *ostype_fp = fopen("/proc/sys/kernel/ostype", "r");
	if (ostype_fp == NULL)
		abort();
	char *ostype_buf = (char *)calloc(1, 4096);
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
		fclose(osrelease_fp);
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
	char *cpu = (char *)malloc(4096);

	// parse /proc/cpuinfo
	FILE *cpuinfo_fp = fopen("/proc/cpuinfo", "r");
	if (cpuinfo_fp == NULL)
		abort();
	char *cpuinfo_buf = (char *)malloc(4096);
	memset(cpuinfo_buf, 0, 1);
	while (fgets(cpuinfo_buf, 4096, cpuinfo_fp) != NULL)
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
struct gpu_buffer *get_gpu()
{
	// allocate memory
	char *gpu_buf = (char *)calloc(1, 1024);
	memset(gpu_buf, '?', 5);

	// GPU count
	size_t gpu_count = 0;

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
			char *gpu;
			if (gpu_count > 0) 
			{
				// alloc more mem when more than 1 GPU
				//gpu_buf = realloc(gpu_buf, (gpu_count * 256) + 256);
				gpu = &gpu_buf[gpu_count * 256];
			}
			else
			{
				gpu = gpu_buf;
			}
			// read PCI device vendor
			pci_lookup_name(pciaccess, gpu, 127,
							PCI_LOOKUP_VENDOR, dev->vendor_id);
			size_t gpu_len = strlen(gpu);
			// add space after vendor
			gpu[gpu_len] = ' ';
			// read PCI device name
			pci_lookup_name(pciaccess, &gpu[gpu_len + 1], 127,
							PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
			gpu_count++;
			break;
		default:
			break;
		}
	}
	pci_cleanup(pciaccess);
	char *gpu = gpu_buf;
#ifdef DEBUG
	strcpy(&gpu_buf[256], "test uwu");
	gpu_count++;
	for(int i = 0; i < gpu_count; i++) 
	{
		printf("GPU%d: %s\n", i, gpu);
		gpu = gpu + 256;
	}
#endif
	struct gpu_buffer *result = (struct gpu_buffer *)malloc(sizeof(struct gpu_buffer));
	result->count = gpu_count;
	result->buffer = gpu_buf;
	return result; 
}
#endif

void get_mem(int *mem_total, int *mem_used)
{
	// parse /proc/meminfo
	char *mem_total_str = (char *)calloc(1, 4096);
	char *mem_available_str = (char *)calloc(1, 4096);
	FILE *meminfo_fp = fopen("/proc/meminfo", "r");
	if (meminfo_fp == NULL)
		abort();
	char *meminfo_buf = (char *)calloc(1, 4096);
	while (fgets(meminfo_buf, 4096, meminfo_fp) != NULL)
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

char *get_shell()
{
	char *shell = calloc(1, 4096);
	char *env = getenv("SHELL");
	if (env)
	{
		strcpy(shell, env);
	}
	else
	{
		strcpy(shell, "Unknown");
	}
	return shell;
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
	else if (strcmp(distro, "gentoo") == 0)
	{
		printf(GENTOO);
	}
	else
	{
		printf(UNKNOWN);
	}
}

int get_term_width()
{
	int width = -1;

#ifdef __unix__
	struct winsize max;
	ioctl(0, TIOCGWINSZ, &max);
	width = max.ws_col;
#else
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int tmpret;
	tmpret = GetConsoleScreenBufferInfo(hConsole, &csbi);
	(void)tmpret;
	width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#endif

	return width;
}

void print_info(char *os, char *shell, char *kernel, char *cpu, struct gpu_buffer *gpu, int mem_used, int mem_total)
{
	// allocate a string for every line
	char *os_line = calloc(1, 4096);
	char *shell_line = calloc(1, 4096);
	char *kernel_line = calloc(1, 4096);
	char *cpu_line = calloc(1, 4096);
#ifndef NO_PCI
	char *gpu_line = calloc(gpu->count, 1024);
#endif
	char *mem_line = calloc(1, 4096);
	// copy and format
	sprintf(os_line, TEMPLATE_OS, os);
	sprintf(shell_line, TEMPLATE_SHELL, shell);
	sprintf(kernel_line, TEMPLATE_KERNEL, kernel);
	sprintf(cpu_line, TEMPLATE_CPU, cpu);
#ifndef NO_PCI
    char *gpu_ptr = gpu->buffer;
    char *gpu_line_ptr = gpu_line;
    for(int i = 0; i < gpu->count; i++) {
        if (i >= 4) break;
	    sprintf(gpu_line_ptr, TEMPLATE_GPU, gpu_ptr);
        int len = strlen(gpu_line_ptr);
        gpu_line_ptr[len] = '\n';
        gpu_ptr = gpu_ptr + 256;
        gpu_line_ptr = &gpu_line_ptr[len + 1];
    }
#endif
	sprintf(mem_line, TEMPLATE_MEMORY, mem_used, mem_total);

	// get terminal size
	int term_width = get_term_width();
	if (term_width > 0 && term_width < 4077)
	{
		// check string size and cut accordingly
		os_line[term_width + 18] = '\0';
		shell_line[term_width + 18] = '\0';
		kernel_line[term_width + 18] = '\0';
		cpu_line[term_width + 18] = '\0';
#ifndef NO_PCI
		gpu_line[term_width + 18] = '\0';
#endif
		mem_line[term_width + 18] = '\0';
	}

	// print the lines
	printf("\x1b[10F");
#ifdef NO_PCI
	printf("%s\n%s\n%s\n%s\n%s\n", os_line, shell_line, kernel_line, cpu_line, mem_line);
#else
	printf("%s\n%s\n%s\n%s\n%s%s\n", os_line, shell_line, kernel_line, cpu_line, gpu_line, mem_line);
#endif
	printf("\x1b[4E");

	// free memory
	free(os_line);
	free(shell_line);
	free(kernel_line);
	free(cpu_line);
#ifndef NO_PCI
	free(gpu_line);
#endif
	free(mem_line);
}

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
			struct gpu_buffer *GPU = get_gpu();
			printf("%s\n", GPU->buffer);
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
				distro = (char *)calloc(1, 4096);
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
		else if (strcmp("-l", argv[i]) == 0 || strcmp("--legal", argv[i]) == 0)
		{
			puts(LANGUAGE_LICENSE);
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
	char *shell = NULL;
	char *kernel = NULL;
	char *cpu = NULL;
#ifndef NO_PCI
	struct gpu_buffer *gpu = NULL;
#endif

	// get distro
	if (distro == NULL)
	{
		distro = get_distro();
	}
	// get OS
	os = get_os(distro);
	// get shell
	shell = get_shell();
	// get kernel
	kernel = get_kernel();
	// get CPU
	cpu = get_cpu();

#ifndef NO_PCI
	// get GPU info
	struct gpu_buffer *gpuinfo = get_gpu();
	gpu = gpuinfo;

    /*gpu = (char *)calloc(gpuinfo->count, 256);
	for(int i = 0; i < gpuinfo->count; i++) {
		strcpy(gpu, &(gpuinfo->buffer)[i * 256]);
	}*/
    //free(gpuinfo->buffer);
	//free(gpuinfo);
#endif

	// get meminfo
	int mem_total;
	int mem_used;
	get_mem(&mem_total, &mem_used);

	// print distro
	print_distro(distro);

// print info
#ifdef NO_PCI
	print_info(os, shell, kernel, cpu, NULL, mem_used, mem_total);
#else
	print_info(os, shell, kernel, cpu, gpu, mem_used, mem_total);
#endif

	// cleanup
	free(os);
	free(shell);
	free(distro);
	free(kernel);
	free(cpu);
#ifndef NO_PCI
	free(gpuinfo->buffer);
	free(gpuinfo);
#endif
}
