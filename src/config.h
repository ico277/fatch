// -- config start --

// The separator used between the value key pair
#define SEPARATOR "->"

// Colors
// These colors are the 256 ANSI escape code colors
#define KEY_COLOR "255"
#define VALUE_COLOR "255"
#define SEPARATOR_COLOR "105"
#define DISTRO_COLOR "105"

// -- ASCII art start --
#define UNKNOWN           \
    "       #####\n"      \
    "      ##O#O##\n"     \
    "      #######\n"     \
    "    ###########\n"   \
    "   #############\n"  \
    "  ###############\n" \
    "   #############\n"  \
    "    ###########\n"   \
    "\n"                  \
    "\n"

#define ARCH               \
    "         /\\\n"       \
    "        /  \\\n"      \
    "       /\\   \\\n"    \
    "      /      \\\n"    \
    "     /   __   \\\n"   \
    "    /   |  |  -\\\n"  \
    "   /_-''    ''-_\\\n" \
    "\n"                   \
    "\n"                   \
    "\n"

#define DEBIAN          \
    "       _____\n"    \
    "      /  ___ \\\n" \
    "     |  /     |\n" \
    "     |  \\____/\n" \
    "      -_\n"        \
    "        -._\n"     \
    "\n"                \
    "\n"                \
    "\n"                \
    "\n"

#define UBUNTU           \
    "             _\n"   \
    "         ---(_)\n"  \
    "     _/  ---  \\\n" \
    "    (_) |   |\n"    \
    "      \\  --- _/\n" \
    "         ---(_)\n"  \
    "\n"                 \
    "\n"                 \
    "\n"                 \
    "\n"

#define LINUXMINT           \
    " ______________\n"     \
    "|__             \\\n"  \
    "   | | _______   |\n"  \
    "   | |  |  |  |  |\n"  \
    "   | |  |  |  |  |\n"  \
    "   | \\________/  |\n" \
    "   \\____________/\n"  \
    "\n"                    \
    "\n"                    \
    "\n"

#define LINUXLITE       \
    "         /\\\n"    \
    "        /  \\\n"   \
    "       / / /\n"    \
    "      > / /\n"     \
    "      \\ \\ \\\n"  \
    "       \\_\\_\\\n" \
    ">          \\\n"   \
    "\n"                \
    "\n"                \
    "\n"

#define POPOS                 \
    "______\n"                \
    "\\   _ \\        __\n"   \
    " \\ \\ \\ \\      / /\n" \
    "  \\ \\_\\ \\    / /\n"  \
    "   \\  ___\\  /_/\n"     \
    "    \\ \\    _ \n"       \
    "   __\\_\\__(_)_ \n"     \
    "  (___________)`\n"      \
    "\n"                      \
    "\n"

#define WINDOWS11         \
    "######## ########\n" \
    "######## ########\n" \
    "######## ########\n" \
    "######## ########\n" \
    "\n"                  \
    "######## ########\n" \
    "######## ########\n" \
    "######## ########\n" \
    "######## ########\n" \
    "\n"

#define GENTOO            \
    "      _-----_\n"     \
    "     (       \\\n"   \
    "     \\    0   \\\n" \
    "      \\        )\n" \
    "      /      _/\n"   \
    "     (     _-\n"     \
    "     \\____-\n"      \
    "\n"                  \
    "\n"                  \
    "\n"
// -- ASCII art end --

// -- config end --

#define KEY_COLOR_CODE "\x1b[38;5;" KEY_COLOR "m"
#define VALUE_COLOR_CODE "\x1b[38;5;" VALUE_COLOR "m"
#define SEPARATOR_COLOR_CODE "\x1b[38;5;" SEPARATOR_COLOR "m"
#define DISTRO_COLOR_CODE "\x1b[38;5;" DISTRO_COLOR "m"
#define RESET "\x1b[0m"
#define MOVE_CUR "\x1b[20C"

#define TEMPLATE_OS     KEY_COLOR_CODE MOVE_CUR "OS      " SEPARATOR_COLOR_CODE SEPARATOR VALUE_COLOR_CODE "  %s"
#define TEMPLATE_SHELL  KEY_COLOR_CODE MOVE_CUR "Shell   " SEPARATOR_COLOR_CODE SEPARATOR VALUE_COLOR_CODE "  %s"
#define TEMPLATE_KERNEL KEY_COLOR_CODE MOVE_CUR "Kernel  " SEPARATOR_COLOR_CODE SEPARATOR VALUE_COLOR_CODE "  %s"
#define TEMPLATE_CPU    KEY_COLOR_CODE MOVE_CUR "CPU     " SEPARATOR_COLOR_CODE SEPARATOR VALUE_COLOR_CODE "  %s"
#ifndef NO_PCI
#define TEMPLATE_GPU    KEY_COLOR_CODE MOVE_CUR "GPU     " SEPARATOR_COLOR_CODE SEPARATOR VALUE_COLOR_CODE "  %s"
#endif
#define TEMPLATE_MEMORY KEY_COLOR_CODE MOVE_CUR "Memory  " SEPARATOR_COLOR_CODE SEPARATOR VALUE_COLOR_CODE "  %dMiB/%dMiB"
