#include <iostream>
#include <fstream>
#include <cstdlib>
#include "replacement_state.h"
using namespace std;

int main()
{
    UINT32 setnum;
    UINT32 waynum_perset;
    UINT32 repl_policy;
    UINT32 shctentry_num;
    UINT32 operation_num;

    FILE * fp_r = NULL;
    fp_r = fopen("testcases/t8.txt","r"); 

    fscanf(fp_r,"%d",&setnum);
    fscanf(fp_r,"%d",&waynum_perset);
    fscanf(fp_r,"%d",&repl_policy);
    fscanf(fp_r,"%d",&shctentry_num);
    fscanf(fp_r,"%d",&operation_num);    
    
    cout<<setnum<<"\n"<<waynum_perset<<"\n"<<repl_policy<<"\n"<<shctentry_num<<"\n"<<operation_num<<"\n";
    CACHE_REPLACEMENT_STATE * cacheReplState = new CACHE_REPLACEMENT_STATE( setnum, waynum_perset, repl_policy );
    cacheReplState->SHCT_size( shctentry_num );
    cacheReplState->InitSHCTState();
    
    
    //cout << "Test creating an CACHE_REPLACEMENT_STATE object" << endl;

    int serialnum, pc, set, hit, hit_way, replace_way;
    
    for (unsigned index = 0; index < operation_num; index ++){

        fscanf(fp_r,"%d",&serialnum);
        fscanf(fp_r,"%d",&pc);
        fscanf(fp_r,"%d",&set); 
        /*
        scanf("%u",&serialnum);
        scanf("%u",&pc);
        scanf("%u",&set);
        */
        cout<<serialnum<<" "<<pc<<" "<<set<<" ";

        fscanf(fp_r, "%d",&hit);
        //print hit's info
        if(hit){
            fscanf(fp_r, "%d",&hit_way);
            cout<<"Hit "<<hit_way<<"\n";
            cacheReplState->SHiP_Update( set, hit_way, hit, pc );
            cacheReplState->PrintAll( set, hit_way, replace_way, hit );
        //print replace info
        }else{
            replace_way = cacheReplState->SHiP_GetVictimInSet( set );
            cout<<"Replace "<<replace_way<<"\n";
            cacheReplState->SHiP_Update( set, replace_way, hit, pc );
            cacheReplState->PrintAll( set, hit_way, replace_way, hit );
        }

        if(index!=operation_num-1){
            cout<<"\n";
        }

    }

    fclose(fp_r);
    return 0;
}


