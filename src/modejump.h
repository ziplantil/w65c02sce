/*******************************************************************************
            w65c02sce -- cycle-accurate C emulator of the WDC 65C02S
            by ziplantil 2022 -- under the CC0 license
            version: 2022-10-18

            mjump.h - addressing mode jump table implementation
*******************************************************************************/

#ifndef W65C02SCE_MJUMP_H
#define W65C02SCE_MJUMP_H

#define W65C02SCE
#include "w65c02s.h"

#if W65C02SCE_COARSE
#if W65C02SCE_COARSE_CYCLE_COUNTER
#define CYCLE_TOTAL_INC         
#else
#define CYCLE_TOTAL_INC         ++cpu->total_cycles;
#endif

#define SKIP_REST               return cyc
#define SKIP_TO_NEXT(n)         goto cycle_##n;
#define BEGIN_INSTRUCTION       unsigned cyc = 1;
#define CYCLE_END               ++cyc; CYCLE_TOTAL_INC
#define CYCLE_LABEL_1(n)        
#define CYCLE_LABEL_X(n)        goto cycle_##n; cycle_##n:
#define END_INSTRUCTION             CYCLE_END                                  \
                                SKIP_REST;
#else /* !W65C02SCE_COARSE */

#if W65C02SCE_COARSE_CYCLE_COUNTER
#define CYCLE_CONDITION         !--cpu->left_cycles
#else
#define CYCLE_CONDITION         ++cpu->total_cycles == cpu->target_cycles
#endif

#define SKIP_REST               return 0
#define SKIP_TO_NEXT(n)         do { ++cpu->cycl; goto cycle_##n; } while (0)
#define BEGIN_INSTRUCTION       if (LIKELY(!cont)) goto cycle_1;               \
                                switch (cpu->cycl) {
#define CYCLE_END               if (UNLIKELY(CYCLE_CONDITION)) return 1;
#define CYCLE_END_LAST          cpu->cycl = 0; CYCLE_END
                              
#define CYCLE_LABEL_1(n)        cycle_##n: case n:
#define CYCLE_LABEL_X(n)        goto cycle_##n; cycle_##n: case n:
#define END_INSTRUCTION             CYCLE_END_LAST                             \
                                    SKIP_REST;                                 \
                                }                                              \
                                unreachable();                                 \
                                SKIP_REST;
#endif

#define CYCLE_1                           CYCLE_LABEL_1(1)
#define CYCLE_2                 CYCLE_END CYCLE_LABEL_X(2)
#define CYCLE_3                 CYCLE_END CYCLE_LABEL_X(3)
#define CYCLE_4                 CYCLE_END CYCLE_LABEL_X(4)
#define CYCLE_5                 CYCLE_END CYCLE_LABEL_X(5)
#define CYCLE_6                 CYCLE_END CYCLE_LABEL_X(6)
#define CYCLE_7                 CYCLE_END CYCLE_LABEL_X(7)

#define CYCLE(n)                CYCLE_##n

/* example instruction:

    BEGIN_INSTRUCTION
        CYCLE(1)
            foo();
        CYCLE(2)
            bar();
    END_INSTRUCTION

becomes

    switch (cpu->cycl) {
        case 1:
            foo();
            if (!--cpu->left_cycles) return 1;
        case 2:
            bar();
            if (!--cpu->left_cycles) {
                cpu->cycl = 0;
                return 1;
            }
    }
    return 0;

*/

#endif /* W65C02SCE_MJUMP_H */
