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

private:

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


public:

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

    }

};


//Globals here.
int N_LEVELS;
int MAINMEM_HL;
int main_mem_accesses = 0;
Cache* caches;

void mem_read (uint64_t addr) {

}

void mem_write (uint64_t addr) {

}


//Reads the config file and creates corresponding Caches.
int read_inputs() {

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

int main() {
    read_inputs();
}
