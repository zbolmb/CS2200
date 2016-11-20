#include "pagetable.h"
/**
 * This function checks the page table of the current process to find
 * the VPN to PFN mapping.
 *
 * @param vpn The virtual page number that has to be translated
 * @param offset The page offset of the address that is being translated
 * @param rw Specifies if access is read or a write
 * @param stats The struct used for storing statistics
 *
 * @return The page frame number (aka physical frame number) PFN linked to the VPN
 */
uint64_t page_lookup(uint64_t vpn, uint64_t offset, char rw, stats_t *stats)
{
	uint64_t pfn = 0;

	// (1) Check the "current_pagetable" at the param VPN to find the mapping
	// (2) If the mapping does not exist, call the "page_fault_handler" function
	// (3) Make sure to increment stats
	// (4) Make sure to mark the entry dirty if access is a write
	// (5) Make sure to increment the frequency count of the VPN that has been accessed

	/********* TODO ************/
	// stats->accesses++;
	// if (rw == 'r') {
	// 	stats->reads++;
	// } else {
	// 	stats->writes++;
	// }
	// int k;
	// pte_t *cur1;
	// for (k = 0; k < (1 << 6); k++) {
	// 	cur1 = current_pagetable + k;
	// 	printf("pfn %"PRIx64"\n", cur1->pfn);
	// }

	pte_t *cur = current_pagetable + vpn;
	if (cur->valid) {
		cur->frequency++;
		if (rw == 'w') {
			cur->dirty = 1;
		}
		return cur->pfn;
	} else {
		stats->page_faults++;
		if (rw == 'r') {
			stats->reads_from_disk++;
		} else {
			stats->writes_to_disk++;
		}
		pfn = page_fault_handler(vpn, rw, stats);
		return pfn;
	}
}
