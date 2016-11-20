#include "tlb.h"

tlbe_t *tlb;

void tlb_clear(void)
{
    for (uint64_t i = 0; i < (1 << tlb_size); i++) {
        tlb[i].valid = 0;
        tlb[i].dirty = 0;
        tlb[i].used = 0;
    }
}

void tlb_clearOne(uint64_t vpn)
{
    for (uint64_t i = 0; i < (1 << tlb_size); i++) {
        if (tlb[i].valid && tlb[i].vpn == vpn) {
            tlb[i].valid = 0;
            break;
        }
    }
}

void tlb_init(void)
{
    tlb = calloc(sizeof(tlbe_t), (1 << tlb_size));
    if (!tlb) {
        perror_exit("tlb: Could not allocate memory for TLB");
    }
}

void tlb_free(void)
{
    free(tlb);
}
