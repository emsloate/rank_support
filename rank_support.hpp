//
//  rank_support.hpp
//  
//
//  Created by Elliott Sloate on 10/22/20.
//

#ifndef rank_support_hpp
#define rank_support_hpp

#include <stdio.h>
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
    uint16_t * blocks;
    std::map<boost::dynamic_bitset<>,uint32_t*> r_p;
    
    rank_support();
    rank_support(boost::dynamic_bitset<> * b);
    void create_rp(int length,std::string str);
    uint64_t rank1(uint64_t i);
    uint64_t rank0(uint64_t i);
    uint64_t overhead();
    void save(std::string& fname);
    void load(std::string& fname);
};
#endif /* rank_support_hpp */
