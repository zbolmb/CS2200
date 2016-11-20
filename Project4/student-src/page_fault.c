#include "reverselookup.h"
#include "pagetable.h"
#include "stats.h"

static uint64_t find_free_frame(stats_t *stats);

/**
 * This function is used to resolve a page fault. It finds a free frame, and returns
 * the PFN after doing some book keeping
 *
 * @param vpn The virtual page number that has to be looked up
 * @param rw Specifies if an access is a read or a write
 * @param stats The struct used to store stats
 *
 */
uint64_t page_fault_handler(uint64_t vpn, char rw, stats_t *stats)
{
	/* DO NOT MODIFY BELOW LINE */
	uint64_t victim_pfn = find_free_frame(stats);
	/****************************/

	// The above call finds a free frame using the below function
	// Implement that first

	// Now that you have a free frame you can use:

	// (a) Update RLT
	// 		(1) Set the PCB of the RLT at victim_pfn
	// 		(2) Set the RLT at victim_pfn as valid
	// 		(3) Set the VPN in the RLT entry at victim_pfn
	// (b) Update the page table of the page faulting procees using the current_pagetable
	//		(1) Set it valid
	// 		(2) Set the correct VPN to PFN mapping


	/********* TODO ************/
	rlt[victim_pfn].valid = 1;
	current_pagetable[rlt[victim_pfn].vpn].valid = 0;
	if (rw == 'w' && current_pagetable[rlt[victim_pfn].vpn].dirty == 1) {
		stats->writes_to_disk++;
	}
	rlt[victim_pfn].vpn = vpn;
	current_pagetable[vpn].valid = 1;
	current_pagetable[vpn].pfn = victim_pfn;
	if (rw == 'w') {
		current_pagetable[vpn].dirty = 1;
	}
	rlt[victim_pfn].task_struct = current_process;



	// return victim page frame number
	return victim_pfn;
}

/**
 * This functions finds a free frame by using the Least Frequently Used algorithm
 *
 * @param stats The struct used for keeping track of statistics
 *
 * @return The physical frame number that the page fault handler can use
 */
static uint64_t find_free_frame(stats_t *stats)
{
	// (1) Look for an invalid frame in the RLT - Return that frame
	// (2) Use Least frequently used to identify victim frame
	//		(a) Update the victim page table
	// 			(i) Use the RLT to find the page table of the victim process
	//			(ii) Mark it invalid
	//			(iii) Increment writebacks if the victim frame is dirty
	//		(b) Return the victim frame


	/********* TODO ************/
	int i;
	for (i = 0; i < sizeof(rlt) / sizeof(*rlt); i++) {
		if (rlt[i].valid == 0) {
			return i;
		}
	}
	int min = current_pagetable[rlt[0].vpn].frequency;
	int minFrame = 0;
	int curFreq;
	rlte_t curFrame;
	for (i = 0; i < sizeof(rlt) / sizeof(*rlt); i++) {
		curFrame = rlt[i];
		curFreq = current_pagetable[curFrame.vpn].frequency;
		if (curFreq < min) {
			min = curFreq;
			minFrame = i;
		}
	}
	return minFrame;
}
