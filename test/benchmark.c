/*******************************************************************************
            w65c02s.h -- cycle-accurate C emulator of the WDC 65C02S
                         as a single-header library
            by ziplantil 2022 -- under the CC0 license
            version: 2022-10-22

            busdump.c - bus dump program
*******************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define W65C02S_IMPL 1
#define W65C02S_LINK 1
#include "w65c02s.h"

#define INSTRS 0

#if __STDC_VERSION__ >= 201112L
_Alignas(128)
#endif
uint8_t ram[65536];
struct w65c02s_cpu cpu;
unsigned long cycles;

uint8_t w65c02s_read(uint16_t a) {
    return ram[a];
}

void w65c02s_write(uint16_t a, uint8_t v) {
    ram[a] = v;
}

uint16_t vector = 0;

static size_t loadmemfromfile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    size_t size = 0;

    if (!file) {
        perror("fopen");
        return 0;
    }

    size = fread(ram, 1, 0x10000UL, file);
    fclose(file);
    return size;
}

int main(int argc, char *argv[]) {
    double start, end;
    const int tries = 10;
    int i;

    if (argc <= 3) {
#if INSTRS
        printf("%s <file_in> <vector> <instrcount>\n", argv[0]);
#else
        printf("%s <file_in> <vector> <cyclecount>\n", argv[0]);
#endif
        return EXIT_FAILURE;
    }

    if (!loadmemfromfile(argv[1])) {
        return EXIT_FAILURE;
    }

    vector = strtoul(argv[2], NULL, 16);
    cycles = strtoul(argv[3], NULL, 0);
#if INSTRS
    printf("Running %lu instructions\n", cycles);
#else
    printf("Running %lu cycles\n", cycles);
#endif

    w65c02s_init(&cpu, NULL, NULL, NULL);
    
    for (i = 0; i < tries; ++i) {
        unsigned long cycles_run;

        w65c02s_reset(&cpu);
        /* RESET cycles */
        w65c02s_run_instructions(&cpu, 1, false);
        cpu.pc = vector;
        
        start = (double)clock() * 1000 / CLOCKS_PER_SEC;
#if INSTRS
        cycles_run = w65c02s_run_instructions(&cpu, cycles, false);
#else
        cycles_run = w65c02s_run_cycles(&cpu, cycles);
#endif        
        end = (double)clock() * 1000 / CLOCKS_PER_SEC;
#if __STDC_VERSION__ >= 199901L
        printf("%lf ms (%ld cyc)\n", end - start, cycles_run);
#else
        printf("%f ms (%ld cyc)\n", end - start, cycles_run);
#endif
    }

    return EXIT_SUCCESS;
}
