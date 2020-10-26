#ifndef BIT_VEC_H
#define BIT_VEC_H
 

#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <typeinfo>
#include <fstream>
#include <math.h>
#include <boost/dynamic_bitset.hpp>

class rank_support
{
 
public:
	int NS;
	int SB;
	int BL;
	int RP;
	boost::dynamic_bitset<> * bitvec;
	uint64_t * super_blocks;
	uint8_t * blocks;
	std::map<boost::dynamic_bitset<>,uint32_t*> r_p;

    rank_support(boost::dynamic_bitset<> * b);
 	void create_rp(int length,std::string str);
 	uint64_t rank1(uint64_t i);
 	uint64_t rank0(uint64_t i);
 	uint64_t overhead();
 	void save(std::string& fname);
 	void load(std::string& fname);
};

class select_support
{
 
public:
	rank_support * ranker;
	
    select_support(rank_support * r);
 	uint64_t select1(uint64_t i);
 	uint64_t select0(uint64_t i);
 	uint64_t overhead();
 	void save(std::string& fname);
 	void load(std::string& fname);
};
 
#endif
