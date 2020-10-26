#include<iostream>
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
#include "rank_support.hpp"

using namespace std;
using namespace boost;



int main(){
	u_long value = int(pow(2,(16))) - 12345;
	dynamic_bitset<> test1(16,150);
    dynamic_bitset<> test2(5,12);
	rank_support r(&test1);
	string mystring;
	boost::to_string(test1, mystring);
    //print bitvector
  	cout << "bit vector (right to left): " << mystring << '\n';
  	cout << "Overhead Size: " << r.overhead() << endl;
    string file = "new_file.txt";
    r.save(file);
    /*
    rank_support new_guy;
    new_guy.load(file);
    map<boost::dynamic_bitset<>,uint32_t*> newmap = new_guy.r_p;
    dynamic_bitset<>d (1);
    uint32_t * ranks = newmap[d];
    cout << "RANK: " << new_guy.rank1(1);
     */
    
	//string file = "select.txt";
	//s.save(file);
	//s.load(file);
}
	
	
