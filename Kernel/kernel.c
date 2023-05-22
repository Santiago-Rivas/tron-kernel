#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <MemoryManager.h>
#include <syscallManager.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;


static const uint64_t PageSize = 0x1000;

static void *const uCodeModuleAddress = (void *)0x400000;
static void *const uDataModuleAddress = (void *)0x500000;


typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
    return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
            - sizeof(uint64_t)                    // Begin at the top of the stack
            );
}


void *initializeKernelBinary() {

    void *moduleAddresses[] = {
        uCodeModuleAddress,
        uDataModuleAddress
        };

    void * endOfModules = loadModules(&endOfKernelBinary, moduleAddresses);

    void * startOfMem = (void *)(((uint8_t *) endOfModules + PageSize - (uint64_t) endOfModules % PageSize));

    MemoryManagerADT memoryManager = createMemoryManager(startOfMem, startOfMem + sizeof(MemoryManagerADT));

    clearBSS(&bss, &endOfKernel - &bss);

    return getStackBase();
}


int main() {
    load_idt();
    clearScreen();


    ((EntryPoint) uCodeModuleAddress)();

    return 0;
}
