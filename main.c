#include <stdint.h>
#include <unistd.h>
#include "wayland.c"

//linux entrypoint
int main(int argc, char* argv[]) {
    return Wlmain(argc, argv);
}
