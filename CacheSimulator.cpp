#include<iostream>
#include<fstream>
#include<cstring>
#include<stdint.h>
using namespace std;

enum rep_policy {LRU, LFU, RR};

int log_two (int a) {
    if ( a == 1 ) return 0;
    else return (1 + log_two(a/2));
}

class Cache {

public:
    //parameters
    int size; //in KB
    int assoc; //number of slots per set
    int block_size; //in bytes
    int hit_latency;
    rep_policy policy;

    //derived parameters
    int total_capacity; //number of slots the cache has
    int n_sets; //number of sets
    int offset_width;
    int set_width;

    //stats
    int total_accesses;
    int hits;
    int misses;
    
    //the stored addresses
    uint64_t **addresses;
    bool **dirty;
    bool **valid;

    Cache() {
        total_accesses = 0;
        hits = 0;
        misses = 0;
    }

    void setFields ( int i_size, int i_assoc, int i_bsize, int i_hl, rep_policy i_policy ) {
        size = i_size;
        assoc = i_assoc;
        block_size = i_bsize;
        hit_latency = i_hl;
        policy = i_policy;

        //Derive parameters
        total_capacity = 1024*size/block_size;
        n_sets = total_capacity/assoc;
        offset_width = log_two(block_size);
        set_width = log_two(n_sets);

        addresses = new uint64_t*[n_sets];
        dirty = new bool*[n_sets];
        valid = new bool*[n_sets];
        for ( int i = 0 ; i < n_sets ; i++ ) {
            addresses[i] = new uint64_t[assoc];
            dirty[i] = new bool[assoc];
            valid[i] = new bool[assoc];
        }
    }

    void printFields() {
        cout<<"Size: "<<size<<"\nAssociativity: "<<assoc<<"\nBlock_size: "<<block_size<<"\nHit_Latency: "<<hit_latency<<"\nRep_Policy: "<<policy<<"\nTotal capacity: "<<total_capacity<<"\nNumber of sets: "<<n_sets<<endl;
    }

    void addAccess () {
        total_accesses++;
    }

    void addHit () {
        hits++;
    }

    void addMiss() {
        misses++;
    }

    bool containsAddress( uint64_t addr ) {
        int set_number = getSetNumber(addr);
        int tag_number = getTagNumber(addr);

        for ( int slot = 0 ; slot < assoc ; slot++ ) {
            if ( valid[set_number][slot] ) {
                if ( getTagNumber(addresses[set_number][slot]) == tag_number)
                    return true;
            }
        }
        return false;

    }
    
    int getSetNumber (uint64_t addr ) {
        uint64_t set_number = addr>>offset_width;
        set_number &= (uint64_t)(n_sets-1);
        return set_number;
    }

    int getTagNumber (uint64_t addr ) {
        uint64_t tag_number = addr>>(offset_width+set_width);
        return tag_number;
    }

    //returns from 0 to (assoc-1). Which slot to replace.
    int index_to_evict (int set_number) {
        return 0 ; //TODO
    }

    bool invalidate(uint64_t addr ) {
        int set_number = getSetNumber(addr);
        int tag_number = getTagNumber(addr);

        for ( int slot = 0 ; slot < assoc ; slot++ ) {
            if ( valid[set_number][slot] ) {
                if ( getTagNumber(addresses[set_number][slot]) == tag_number) {
                    addAccess();
                    valid[set_number][slot] = false;
                    return dirty[set_number][slot];
                }
            }
        }
        return false;
    }

    void setDirtyBit ( uint64_t addr, bool newval ) {
        int set_number = getSetNumber(addr);
        int tag_number = getTagNumber(addr);

        for ( int slot = 0 ; slot < assoc ; slot++ ) {
            if ( valid[set_number][slot] ) {
                if ( getTagNumber(addresses[set_number][slot]) == tag_number) {
                    dirty[set_number][slot] = newval;
                }
            }
        }
    }

}; 

//Globals here.
int N_LEVELS;
int MAINMEM_HL;
int main_mem_accesses = 0;
Cache* caches;

//Returns true if the dirty bit is true for addr at any level i <= int level
bool invalidate(uint64_t addr, int level ) {

    for (int i = level ; i >= 0 ; i-- ) {
        if(caches[i].invalidate(addr)) return true;
    }
    return false;

}
//For writing down from L(N+1) to LN after fetching.
void mem_read_write (uint64_t addr, int level) {

    caches[level].addAccess();
    int set_number = caches[level].getSetNumber(addr);
//    int tag_number = caches[level].getTagNumber(addr);


    for ( int slot = 0 ; slot < caches[level].assoc; slot++ ) {
        if ( !caches[level].valid[set_number][slot] ) {
            caches[level].valid[set_number][slot] = true;
            caches[level].addresses[set_number][slot] = addr;
            caches[level].dirty[set_number][slot] = false;
            return;
        }
    }

    int i_evict = caches[level].index_to_evict(set_number);

    //Invalidate evicted stuff
    uint64_t old_addr = caches[level].addresses[set_number][i_evict];
    if(invalidate(old_addr,level)) {
        if ((level+1)< N_LEVELS) {
         caches[level+1].addAccess();
         caches[level+1].setDirtyBit(old_addr,true);
        }
        else {
            main_mem_accesses++;
        }
    }

    //Write new address
    caches[level].valid[set_number][i_evict] = true;
    caches[level].addresses[set_number][i_evict] = addr;
    caches[level].dirty[set_number][i_evict] = false;

}



void mem_read (uint64_t addr, int level) {
    if ( level == N_LEVELS ) main_mem_accesses++;
    else {
        caches[level].addAccess();
        if (caches[level].containsAddress(addr)) {
            caches[level].addHit();
        }
        else {
            caches[level].addMiss();
            mem_read(addr,level+1);
            mem_read_write(addr,level);
        }
    }
}

bool mem_write (uint64_t addr, int level) {

    if ( level == N_LEVELS ) { 
        main_mem_accesses++;
        return true;
    }
    else {
        bool returned_from_mainmem = false;
        caches[level].addAccess();
        if ( caches[level].containsAddress(addr)) {
            caches[level].addHit();
        }
        else {
            caches[level].addMiss();
            returned_from_mainmem = mem_write(addr,level+1);
            mem_read_write(addr,level);
        }
        if ( level == 0 && !returned_from_mainmem ) caches[level].setDirtyBit(addr,true);
        return returned_from_mainmem;
    }

}


//Reads the config file and creates corresponding Caches.
void read_inputs() {

    ifstream ifile;
    ifile.open("config.txt");
    string tempstr;
    int tempint;
    ifile>>tempstr>>tempstr>>tempint;
    N_LEVELS = tempint;
    caches = new Cache[N_LEVELS];
    for ( int i = 0 ; i < N_LEVELS ; i++ ) {
        ifile>>tempstr>>tempstr>>tempstr>>tempstr>>tempint;
        int csize = tempint;
        ifile>>tempstr>>tempstr>>tempstr>>tempint;
        int cassoc = tempint;
        ifile>>tempstr>>tempstr>>tempint;
        int cbsize = tempint;
        ifile>>tempstr>>tempstr>>tempstr>>tempint;
        int chl = tempint;
        ifile>>tempstr>>tempstr>>tempstr;
        rep_policy cpol;
        if (!strcmp(tempstr.c_str(),"LRU")) cpol = LRU;
        else if (!strcmp(tempstr.c_str(),"LFU")) cpol = LFU;
        else cpol = RR;
        caches[i].setFields(csize,cassoc,cbsize,chl,cpol);
        caches[i].printFields();
    }
    ifile>>tempstr>>tempstr>>tempstr>>tempstr>>MAINMEM_HL;
    ifile.close();
}

/*
int main() {
    read_inputs();
}*/
