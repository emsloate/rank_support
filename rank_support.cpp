//
//  rank_support.cpp
//  
//
//  Created by Elliott Sloate on 10/22/20.
//

#include "rank_support.hpp"
#include <iostream>
#include <math.h>
#include <map>
#include <typeinfo>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include <chrono>
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace boost;

rank_support::rank_support(){}
rank_support::rank_support(dynamic_bitset<> * b){
    //set values of our size constants
    NS = b->size();
    SB = int(pow(log2(NS),2) / 2);
    BL = int(log2(NS) / 2);
    RP = int(pow(BL,2));

    int num_sb = int(ceil(NS / float(SB)));
    super_blocks = new uint64_t[num_sb];
    int num_bl = int(ceil(NS / float(BL)));
    blocks = new uint16_t[NS/BL];

    bitvec = b;
    uint16_t rank_count = 0;
    //checl first bit, just so modulo ops make sense
    if (bitvec->test(0) == true){
            rank_count = 1;
    }
    //value is 0 for both first blocks
    super_blocks[0] = uint64_t(0);
    blocks[0] = uint16_t(0);
    //counters for superblocks, blocks arrays
    int j = 0;
    int k = 0;
    //just looping through bitvec to get ranks for the blocks we care about
    //start at 1 since we already checked first bit
    for(size_t i = 1;i<NS;i++){
        //start of a super block, super_blocks[j] is just rank(i)
        if (i % SB == 0){
            // increment j, but only if we have not reached last super block
            if (j < num_sb - 1){
                j += 1;
            }
            super_blocks[j] = uint64_t(rank_count);
            
        }
        //start of a regular block, blocks[k] is rank(i) - super_blocks[j]
        if (i % BL == 0){
            if (k < num_bl - 1){
                k += 1;
            }
            blocks[k] = uint16_t(rank_count - int(super_blocks[j]) );
            // increment k, but only if we have not reached last super block
            
        }
        //if we see a 1, increment our rank
        if (bitvec->test(i) == true){
            rank_count += 1;
        }
    }
    //create our map from bitvectors to arrays
    // have to map 2^b bit vectors of length BL
    // best done recusively?
    create_rp(BL,"");
}
void rank_support::create_rp(int length,string str){
    //inputs: length - BL to start, str - a bitvector (in string form) we build each call
    //output: no output is returned, but map<dyanmic_bitset,int*,bitset> r_p will be filled by the end

    //base case: we have filled out current bitvector, time to create rank array and add it to map
    if(length == 0){
        int rank_count = 0;
        // not sure why new fixes bug
        uint32_t * ranks = new uint32_t[BL];
        //loop through our bitset, add rank at each spot to our array (also length BL)
        for(int i = 0;i < BL;i++){
            if(str[i] == '1'){
                rank_count += 1;
            }
            ranks[i] = uint32_t(rank_count);
        }
        //create and insert out bitset, int[] pair into r_p
        // our string is read from right to left when constructing bv, so have to reverse it to make sense
        reverse(str.begin(),str.end());
        dynamic_bitset<> bv (str);
        r_p[bv] = ranks;
        return;
    }

    //otherwise, decrement length, call create_RPp with current string + "0", create_RPp with current string + "1",
    length = length - 1;
    create_rp(length,str+'0');
    create_rp(length,str+'1');
}
uint64_t rank_support::rank1(uint64_t i){
    //inputs: unsigned 64 bit integer i
    //output: # of 1s in bitvec up to position i (inclusive)

    
    
    //create a bitset that represents the block i resides in
    //this is the slice bitvec[i/BL,i/BL + BL]
    //does not seem like bitset supports slicing, so have to just loop through
    uint64_t block_start = i / BL;
    dynamic_bitset<> rp_val(BL,0);
    uint64_t count = 0;
    bool too_big = false;
    for(int index= int(block_start * BL); index < int(block_start * BL + BL);index++){
        //set the bits of rp_val
        //edge case: near the end of the bit vector, may have blocks that go past the end, just set these values to 0 (they won't matter for rank)
        if (index >= bitvec->size()){
            too_big = true;
            rp_val[count] = 0;
        }
        else{
            rp_val[count] = bitvec->test(size_t(index));
        }
        //cout << "rp_val[" << count << "]: " << int(rp_val[count]) << endl;
        count += 1;
    }
    
    // get the array which holds the ranks for the block we have
    uint32_t * r_p_array = r_p[rp_val];
    // the value we care about is clearly i - start of array = i - i/BL * BL
    uint32_t rank_wanted = uint32_t(i - block_start * BL);

    uint32_t r_p_rank = r_p_array[rank_wanted];
    //cout << "r_p_rank:  " << int(r_p_rank) << endl;
    
    //RP_s[i/s]
    uint64_t sb_rank = uint64_t(super_blocks[i / SB]);
    //cout << "Superblock rank: " << sb_rank << endl;
    
    //RP_b[i/b]s
    uint64_t b_rank = uint64_t(blocks[block_start]);
    //cout << "Block rank: " << b_rank << endl;
    
    uint64_t total_rank = uint64_t(sb_rank + b_rank + r_p_rank);
    //cout << "total rank: " << total_rank;
    return total_rank;

}
uint64_t rank_support::rank0(uint64_t i){
    //input: uint64_t i, position in bitvec
    //output: # of 0s up to position i (inclusive)

    return i - rank1(i) + 1;
}
uint64_t rank_support::overhead(){
    return uint64_t(sizeof(*this));

}
void rank_support::save(string& fname){
    ofstream myfile;
    myfile.open(fname);
    myfile << NS << endl;
    myfile << *bitvec << endl;
    //write superblock array
    myfile << SB << endl;
    int num_sb = int(ceil(NS / float(SB)));
    for(int i = 0; i < num_sb; i++){
        myfile << int(super_blocks[i]) << "," ;
    }
    myfile << endl;
    //write block array
    myfile << BL << endl;
    int num_bl = int(ceil(NS / float(BL)));
    for(int i = 0; i < num_bl; i++){
        myfile << int(blocks[i]) << "," ;
    }
    myfile << endl;
    //write rp table
    //idea: each line pair is key value pair seperated
    // write 2*(2^BL) lines
    map<dynamic_bitset<>,uint32_t*>::iterator it;

    for ( it = r_p.begin(); it != r_p.end(); it++ )
    {
        //int representation of bv
        dynamic_bitset<> bv = it->first;
        // represent array
        uint32_t* val = it->second;   // string's value
        //write key on first line
        myfile << bv << endl;
        //write value on second line
        for(int i = 0;i< BL;i++){
            myfile << int(val[i]) << "," ;
        }
        //end line here
        myfile << endl;
    }
    myfile.close();

}
void rank_support::load(string& fname){
     string line;
     ifstream myfile (fname);
     stringstream buffer;
     //read file
     if (myfile.is_open()){
         buffer << myfile.rdbuf();
         //get bitvector size
         getline(buffer,line);
         NS = stoi(line);
         //get bitvector
         getline(buffer,line);
         //int value = stoi(line);
         bitvec = new dynamic_bitset<>(line);
         //read superblock array
         //get size
         getline(buffer, line);
         SB = stoi(line);
         int num_sb = int(ceil(NS/float(SB)));
         super_blocks = new uint64_t[num_sb];
         //read elements
         getline(buffer, line);
         stringstream linestream;
         linestream.str(line);
         string tok;
         int i = 0;
         while(getline(linestream, tok, ',')){
             if (tok.size() > 0){
                super_blocks[i] = uint64_t(stoi(tok));
                i++;
             }
         }
         //read block array
         //get size
         getline(buffer, line);
         BL = stoi(line);
         int num_bl = int(ceil(NS/float(BL)));
         blocks = new uint16_t[num_bl];
         //get elements
         getline(buffer, line);
         linestream.clear();
         linestream.str(line);
         i = 0;
         while(getline(linestream, tok, ',')){
             if (tok.size() > 0){
                 blocks[i] = uint16_t(stoi(tok));
                 i++;
             }
         }
         //read map
         i = 0;
         //2^BL key-value pairs
         int limit = int(pow(2,BL));
         for(int pair = 0; pair<limit; pair++){

             getline(buffer,line);    //reads bitvector long into line
             dynamic_bitset<> * bv = new dynamic_bitset<>(line);
             getline(buffer,line);     //reads uint8 array into line
             
             linestream.clear();
             linestream.str(line);

             //doing slow version of creating array
             uint32_t * ranks = new uint32_t[BL];
             while(getline(linestream, tok, ',')){
                 if(tok.size() > 0){
                     ranks[i] = uint32_t(stoi(tok));
                     i++;
                 }
             }
             //set i = 0 for next value
             i = 0;
             //add to map
             r_p[*bv] = ranks;
         }
     }
     myfile.close();
     
}

/*
int main(){
    //testing for various sizes
    for(int i = 5;i<26;i++){
        unsigned long long int powval = (unsigned long long int) (pow(2,i));
        unsigned long long int bitval = rand() % powval;
        dynamic_bitset<> bv(powval,bitval);
        rank_support r(&bv);
        uint64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        for (int j = 0; j < 1000000;j++){
            r.rank1(j % powval);
        }
        
        uint64_t end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        uint64_t time = end-start;
        cout << powval << "," << r.overhead() << "," << time << endl;
    }
    
    
}*/
    
