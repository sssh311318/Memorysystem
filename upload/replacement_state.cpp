#include "replacement_state.h"
using namespace std;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This file is distributed as part of the Cache Replacement Championship     //
// workshop held in conjunction with ISCA'2010.                               //
//                                                                            //
//                                                                            //
// Everyone is granted permission to copy, modify, and/or re-distribute       //
// this software.                                                             //
//                                                                            //
// Please contact Aamer Jaleel <ajaleel@gmail.com> should you have any        //
// questions                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/*
** This file implements the cache replacement state. Users can enhance the code
** below to develop their cache replacement ideas.
**
*/


////////////////////////////////////////////////////////////////////////////////
// The replacement state constructor:                                         //
// Inputs: number of sets, associativity, and replacement policy to use       //
// Outputs: None                                                              //
//                                                                            //
// DO NOT CHANGE THE CONSTRUCTOR PROTOTYPE                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
CACHE_REPLACEMENT_STATE::CACHE_REPLACEMENT_STATE( UINT32 _sets, UINT32 _assoc, UINT32 _pol )
{

    numsets    = _sets;
    assoc      = _assoc;
    replPolicy = _pol;

    mytimer    = 0;

    InitReplacementState();
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function initializes the replacement policy hardware by creating      //
// storage for the replacement state on a per-line/per-cache basis.           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void CACHE_REPLACEMENT_STATE::InitReplacementState()
{
    // Create the state for sets, then create the state for the ways
    repl  = new LINE_REPLACEMENT_STATE* [ numsets ];

    // ensure that we were able to create replacement state
    assert(repl);

    // Create the state for the sets
    for(UINT32 setIndex=0; setIndex<numsets; setIndex++) 
    {
        repl[ setIndex ]  = new LINE_REPLACEMENT_STATE[ assoc ];

        for(UINT32 way=0; way<assoc; way++) 
        {
            // initialize stack position (for true LRU)
            repl[ setIndex ][ way ].LRUstackposition = way;
        }
    }

    // Contestants:  ADD INITIALIZATION FOR YOUR HARDWARE HERE

    for(UINT32 setIndex=0; setIndex<numsets; setIndex++) 
    {

        for(UINT32 way=0; way<assoc; way++) 
        {
            // initialize  (for true SHiP)
            repl[ setIndex ][ way ].RRPV = 3;
            repl[ setIndex ][ way ].signature_line = 0;
            repl[ setIndex ][ way ].outcome = 0;
        }
    }

}
void CACHE_REPLACEMENT_STATE::InitSHCTState(){
    shct = new SHCT_STATE[ shctentry_num ];
    for(UINT32 shctIndex=0; shctIndex<shctentry_num; shctIndex++) 
    {
        // initialize SHCT(for true SHiP)
        shct[ shctIndex ].shct_value = 1;
    }
}

void CACHE_REPLACEMENT_STATE::SHCT_size( UINT32 shct_in){

    shctentry_num = shct_in;

}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function is called by the cache on every cache miss. The input        //
// arguments are the thread id, set index, pointers to ways in current set    //
// and the associativity.  We are also providing the PC, physical address,    //
// and accesstype should you wish to use them at victim selection time.       //
// The return value is the physical way index for the line being replaced.    //
// Return -1 if you wish to bypass LLC.                                       //
//                                                                            //
// vicSet is the current set. You can access the contents of the set by       //
// indexing using the wayID which ranges from 0 to assoc-1 e.g. vicSet[0]     //
// is the first way and vicSet[4] is the 4th physical way of the cache.       //
// Elements of LINE_STATE are defined in crc_cache_defs.h                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
INT32 CACHE_REPLACEMENT_STATE::GetVictimInSet( UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, UINT32 assoc,
                                               Addr_t PC, Addr_t paddr, UINT32 accessType )
{
    // If no invalid lines, then replace based on replacement policy
    if( replPolicy == CRC_REPL_LRU ) 
    {
        return Get_LRU_Victim( setIndex );
    }
    else if( replPolicy == CRC_REPL_RANDOM )
    {
        return Get_Random_Victim( setIndex );
    }
    else if( replPolicy == CRC_REPL_CONTESTANT )
    {
        // Contestants:  ADD YOUR VICTIM SELECTION FUNCTION HERE
        return SHiP_GetVictimInSet( setIndex );
    }

    // We should never get here
    assert(0);

    return -1; // Returning -1 bypasses the LLC
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function is called by the cache after every cache hit/miss            //
// The arguments are: the set index, the physical way of the cache,           //
// the pointer to the physical line (should contestants need access           //
// to information of the line filled or hit upon), the thread id              //
// of the request, the PC of the request, the accesstype, and finall          //
// whether the line was a cachehit or not (cacheHit=true implies hit)         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void CACHE_REPLACEMENT_STATE::UpdateReplacementState( 
    UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, 
    UINT32 tid, Addr_t PC, UINT32 accessType, bool cacheHit )
{
    // What replacement policy?
    if( replPolicy == CRC_REPL_LRU ) 
    {
        UpdateLRU( setIndex, updateWayID );
    }
    else if( replPolicy == CRC_REPL_RANDOM )
    {
        // Random replacement requires no replacement state update
    }
    else if( replPolicy == CRC_REPL_CONTESTANT )
    {
        // Contestants:  ADD YOUR UPDATE REPLACEMENT STATE FUNCTION HERE
        // Feel free to use any of the input parameters to make
        // updates to your replacement policy
        SHiP_Update( setIndex, updateWayID, cacheHit, PC );
    }
    
    
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//////// HELPER FUNCTIONS FOR REPLACEMENT UPDATE AND VICTIM SELECTION //////////
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function finds the LRU victim in the cache set by returning the       //
// cache block at the bottom of the LRU stack. Top of LRU stack is '0'        //
// while bottom of LRU stack is 'assoc-1'                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
INT32 CACHE_REPLACEMENT_STATE::Get_LRU_Victim( UINT32 setIndex )
{
    // Get pointer to replacement state of current set
    LINE_REPLACEMENT_STATE *replSet = repl[ setIndex ];

    INT32   lruWay   = 0;

    // Search for victim whose stack position is assoc-1
    for(UINT32 way=0; way<assoc; way++) 
    {
        if( replSet[way].LRUstackposition == (assoc-1) ) 
        {
            lruWay = way;
            break;
        }
    }

    // return lru way
    return lruWay;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function finds a random victim in the cache set                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
INT32 CACHE_REPLACEMENT_STATE::Get_Random_Victim( UINT32 setIndex )
{
    INT32 way = (rand() % assoc);
    
    return way;
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//SHiP                                                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
INT32 CACHE_REPLACEMENT_STATE::SHiP_GetVictimInSet( UINT32 setIndex )
{
    // Get pointer to replacement state of current set
    LINE_REPLACEMENT_STATE *replSet = repl[ setIndex ];

    INT32   SHiPWay_88   = 0;

    // Search for victim whose stack position is assoc-1
    findway:
    for(UINT32 way=0; way<assoc; way++) 
    {
        if( replSet[way].RRPV == 3 ) 
        {
            SHiPWay_88 = way;
            return SHiPWay_88;
        }
    }

    for(UINT32 way=0; way<assoc; way++) 
    {
        replSet[way].RRPV++;
    }
    goto findway;
    

    // return SHiP way
    
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This function implements the LRU update routine for the traditional        //
// LRU replacement policy. The arguments to the function are the physical     //
// way and set index.                                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void CACHE_REPLACEMENT_STATE::UpdateLRU( UINT32 setIndex, INT32 updateWayID )
{
    // Determine current LRU stack position
    UINT32 currLRUstackposition = repl[ setIndex ][ updateWayID ].LRUstackposition;

    // Update the stack position of all lines before the current line
    // Update implies incremeting their stack positions by one
    for(UINT32 way=0; way<assoc; way++) 
    {
        if( repl[setIndex][way].LRUstackposition < currLRUstackposition ) 
        {
            repl[setIndex][way].LRUstackposition++;
        }
    }

    // Set the LRU stack position of new line to be zero
    repl[ setIndex ][ updateWayID ].LRUstackposition = 0;
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//SHiP Update                                                                 //
//                                                                            //
//pseudo code
/*
Cache Hit:
cache_line.outcome = true;
Increment SHCT[signature_line];
RRPV = 0; // same as RRIP
Cache Miss:
find an old line to evict
if (evicted_cache_line.outcome == false)
Decrement SHCT[signature_line];
insert the new line
cache_line.outcome = false;
cache_line.signature_line = hash(PC);
if (SHCT[hash(PC)] == 0)
RRPV = 3; // not worth being cached
else
RRPV = 2; // same as RRIP
*/
////////////////////////////////////////////////////////////////////////////////
void CACHE_REPLACEMENT_STATE::SHiP_Update( UINT32 setIndex, INT32 updateWayID, bool hit, UINT32 pc )
{
    UINT32 hash;
    hash = pc % shctentry_num;
    if(hit){
        repl[ setIndex ][ updateWayID ].RRPV = 0;
        repl[ setIndex ][ updateWayID ].outcome = 1;
        pre_signature_line = repl[ setIndex ][ updateWayID ].signature_line;
        if(shct[repl[ setIndex ][ updateWayID ].signature_line].shct_value != 3){
            shct[repl[ setIndex ][ updateWayID ].signature_line].shct_value ++;
        }
        change_pre_sig = true;
    }else{
        if(repl[ setIndex ][ updateWayID ].outcome == 0){
            if(shct[repl[ setIndex ][ updateWayID ].signature_line].shct_value!=0){
                shct[repl[ setIndex ][ updateWayID ].signature_line].shct_value--;
            }
            pre_signature_line = repl[ setIndex ][ updateWayID ].signature_line;
            change_pre_sig = true;
        }else{
            change_pre_sig = false;
        }
        repl[ setIndex ][ updateWayID ].outcome = 0;
        repl[ setIndex ][ updateWayID ].signature_line = hash;
        if(shct[hash].shct_value == 0){
            repl[ setIndex ][ updateWayID ].RRPV = 3;
        }else{
            repl[ setIndex ][ updateWayID ].RRPV = 2;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// The function prints the statistics for the cache                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
ostream & CACHE_REPLACEMENT_STATE::PrintStats(ostream &out)
{

    out<<"=========================================================="<<endl;
    out<<"=========== Replacement Policy Statistics ================"<<endl;
    out<<"=========================================================="<<endl;

    // CONTESTANTS:  Insert your statistics printing here

    return out;
    
}

void  CACHE_REPLACEMENT_STATE::PrintAll( UINT32 setIndex , UINT32 hit_way, UINT32 replace_way, int hit )
{
    cout<<"RRPV:      ";

    for (UINT32 i = 0; i < assoc; i++){
        if(hit){
            if(i == hit_way)cout<<"(";
        }else{
            if(i == replace_way)cout<<"(";
        }
        
        cout<<repl[setIndex][i].RRPV;

        if(hit){
            if(i == hit_way)cout<<")";
        }else{
            if(i == replace_way)cout<<")";
        }

        if(i == assoc - 1){
            cout<<"\n";
        }else{
            cout<<" ";
        }
    }

    cout<<"Signature: ";
    for (UINT32 i = 0; i < assoc; i++){
        if(hit == 0){
            if(i == replace_way)cout<<"(";
        }
        
        cout<<repl[setIndex][i].signature_line;

        if(hit == 0 ){
            if(i == replace_way)cout<<")";
        }

        if(i == assoc - 1){
            cout<<"\n";
        }else{
            cout<<" ";
        }
    }

    cout<<"outcome:   ";
    for (UINT32 i = 0; i < assoc; i++){
        if(hit){
            if(i == hit_way)cout<<"(";
        }else{
            if(i == replace_way)cout<<"(";
        }
        
        cout<<repl[setIndex][i].outcome;

        if(hit){
            if(i == hit_way)cout<<")";
        }else{
            if(i == replace_way)cout<<")";
        }

        if(i == assoc - 1){
            cout<<"\n";
        }else{
            cout<<" ";
        }
    }

    cout<<"SHCT:      ";
    for (UINT32 i = 0; i < shctentry_num; i++){

        if((i == pre_signature_line) && change_pre_sig)cout<<"(";
             
        cout<<shct[i].shct_value;

        if((i == pre_signature_line) && change_pre_sig)cout<<")";
        

        if(i == shctentry_num - 1){
            cout<<"\n";
        }else{
            cout<<" ";
        }
    }
    
}
