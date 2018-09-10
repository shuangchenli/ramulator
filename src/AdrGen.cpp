/*
 * AdrGen.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: shuangchenli
 */

#include "AdrGen.h"
using namespace std;
using namespace ramulator;

namespace ramulator {

template<>
AdrGen<HMC>::AdrGen(const Config& configs, int _id, Memory<HMC, Controller>& memory) {
		id = _id;
		keepFull = configs.contains("keep_full");
		if(configs.contains("pattern")) {
			//[TODO]: check if valid input
			pattern = Pattern_map[configs["pattern"]];
		}
		if(configs.contains("mapping")) {
			//[TODO]: check if valid input
			mapping = AdrMapType_map[configs["mapping"]];
		}
		bit[int(Level::Clear)] = memory.tx_bits;
		bit[int(Level::Block)] = memory.spec->maxblock_entry.flit_num_bits - memory.tx_bits;
		bit[int(Level::Vault)] = memory.addr_bits[int(HMC::Level::Vault)];
		bit[int(Level::BankGroup)] = memory.addr_bits[int(HMC::Level::BankGroup)];
		bit[int(Level::Bank)] = memory.addr_bits[int(HMC::Level::Bank)];
		bit[int(Level::Row)] = memory.addr_bits[int(HMC::Level::Row)];
		bit[int(Level::Column)] = memory.addr_bits[int(HMC::Level::Column)];
}

}


