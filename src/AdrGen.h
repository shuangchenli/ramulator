/*
 * AdrGen.h
 *
 *  Created on: Sep 8, 2018
 *      Author: shuangchenli
 */

#ifndef SRC_ADRGEN_H_
#define SRC_ADRGEN_H_

#include <vector>
#include "Config.h"
#include "Memory.h"
#include "HMC_Memory.h"
#include "HMC_Controller.h"
#include "Controller.h"

using namespace std;
namespace ramulator
{

template<typename T>
class AdrGen{
public:
	long adr = 0;
  bool keepFull = true;
  int id = 0;
  enum class Level: int {
    	Channel, Rank, Vault, BankGroup, Bank, Row, Column, Clear, Block, MAX
  };
  int bit[(int)Level::MAX] = {0, 0, 0, 0, 0, 0, 0, 0};
  enum class Pattern: int {
  	Random, Sequential, Linklist, MAX
  } pattern = Pattern::Sequential;
  string pattern_name[int(Pattern::MAX)] = {
      "Random", "Sequential", "Linklist"
  };
  enum class AdrMapType {
      RoCoBaVa, // XXX The specification doesn't define row/column addressing
      RoBaCoVa,
      RoCoBaBgVa,
      ChRaBaRoCo,
      RoBaRaCoCh,
      MAX,
  } mapping = AdrMapType::RoCoBaVa;
  string map_name[int(AdrMapType::MAX)] = {
      "RoCoBaVa", "RoBaCoVa", "RoCoBaBgVa", "ChRaBaRoCo", "RoBaRaCoCh"
  };

  map<string, enum Pattern> Pattern_map = {
      {"Random", Pattern::Random},
      {"Sequential", Pattern::Sequential},
      {"Linklist", Pattern::Linklist}
  };

  map<string, enum AdrMapType> AdrMapType_map = {
      {"RoCoBaVa", AdrMapType::RoCoBaVa},
      {"RoBaCoVa", AdrMapType::RoBaCoVa},
      {"RoCoBaBgVa", AdrMapType::RoCoBaBgVa},
      {"ChRaBaRoCo", AdrMapType::ChRaBaRoCo},
      {"RoBaRaCoCh", AdrMapType::RoBaRaCoCh}
  };
	AdrGen(const Config& configs, int _id, Memory<T, Controller>& memory) {
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
		bit[int(Level::Channel)] = memory.addr_bits[int(T::Level::Channel)];
		bit[int(Level::Rank)] = memory.addr_bits[int(T::Level::Rank)];
		bit[int(Level::Bank)] = memory.addr_bits[int(T::Level::Bank)];
		bit[int(Level::Row)] = memory.addr_bits[int(T::Level::Row)];
		bit[int(Level::Column)] = memory.addr_bits[int(T::Level::Column)];
		if(int(T::Level::MAX) == 6) //[FIXME]: damn it template
			bit[int(Level::BankGroup)] = memory.addr_bits[2];
	}
  ~AdrGen();
 	long gen(){
 		long out_adr = 0, block = 0;
 		adr++;
 		switch(int(mapping)) {
 			case int(AdrMapType::RoCoBaVa):
 			case int(AdrMapType::RoBaCoVa):
 			case int(AdrMapType::RoCoBaBgVa): {
 				block = adr & ((1<<bit[int(Level::Block)]) - 1);
 				out_adr = adr >> bit[int(Level::Block)];
 				out_adr = out_adr << bit[int(Level::Vault)];
 				out_adr += id;
 				out_adr = out_adr << bit[int(Level::Block)];
 				out_adr += block;
 				out_adr = out_adr << bit[int(Level::Clear)];
 				break;
 			}
 			case int(AdrMapType::ChRaBaRoCo): {
 				out_adr = id << (bit[int(Level::Rank)] + bit[int(Level::BankGroup)] + bit[int(Level::Bank)]
 			                       + bit[int(Level::Row)] + bit[int(Level::Column)]);
 				out_adr += adr;
 				out_adr = out_adr << bit[int(Level::Clear)];
 				break;
 			}
 			case int(AdrMapType::RoBaRaCoCh): {
 				out_adr = adr << bit[int(Level::Channel)];
 				out_adr += id;
 				out_adr = out_adr << bit[int(Level::Clear)];
 				break;
 			}
 			default:;
 		}
 		return out_adr;
 	}
 	void rollback(){
 		adr--;
 	}

};

template <>
AdrGen<HMC>::AdrGen(const Config& configs, int _id, Memory<HMC, Controller>& memory);
}


#endif /* SRC_ADRGEN_H_ */
