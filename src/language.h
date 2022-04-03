// English

#define LANGUAGE_USAGE                                                             \
    "Usage: %s [INFO] [OPTION]...\n"                                               \
    "\n"                                                                           \
    "Options:\n"                                                                   \
    "  -h, --help               Prints this message\n"                             \
    "  -v, --version            Prints version information\n"                      \
    "  -d, --distro <distro>    Prints the specified distribution logo and name\n" \
    "  -l, --legal              Prints license information\n"                      \
    "\n"                                                                           \
    "Infos:\n"                                                                     \
    "  memory, mem              Prints the amount of memory used and installed\n"  \
    "  cpu                      Prints the CPU model\n"                            \
    "  gpu                      Prints the GPU model*\n"                           \
    "  kernel                   Prints the kernel version\n"                       \
    "  os                       Prints the OS name\n"                              \
    "\n"                                                                           \
    "*: Only works if supported!"                                                  \
    "\n"

#define LANGUAGE_INVALID_ARGUMENT "Invalid argument: %s\n"

#define LANGUAGE_UNKNOWN "Unknown"

#define LANGUAGE_GPU_UNSUPPORTED "GPU Unsupported\n"

#define LANGUAGE_LICENSE                                                  \
    "This program provides no warranty whatsoever.\n"                     \
    "See the GNU General Public License version 2 for more details.\n"    \
    "You should have received a copy of the GNU General Public License\n" \
    "along with this program. If not, see <https://www.gnu.org/licenses/>.\n"
