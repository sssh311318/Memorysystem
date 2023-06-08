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
    fp_r = fopen("testcases/t8.txt","r"); 
    fscanf(fp_r,"%d",&setnum);
    fscanf(fp_r,"%d",&waynum_perset);
    fscanf(fp_r,"%d",&repl_policy);
    fscanf(fp_r,"%d",&shctentry_num);
    fscanf(fp_r,"%d",&operation_num);    
    
    cout<<setnum<<"\n"<<waynum_perset<<"\n"<<repl_policy<<"\n"<<shctentry_num<<"\n"<<operation_num<<"\n";
    
    cacheReplState->SHCT_size( shctentry_num );
    cacheReplState->InitSHCTState();
    
    
    //cout << "Test creating an CACHE_REPLACEMENT_STATE object" << endl;
    CACHE_REPLACEMENT_STATE * cacheReplState = new CACHE_REPLACEMENT_STATE( setnum, waynum_perset, repl_policy );

    int serialnum, pc, set, hit, hit_way, replace_way;
    
    for (unsigned index = 0; index < numinst; index ++){

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
            fscanf(fp_r, "%d",&hitway);
            cout<<"Hit "<<hitway<<"\n";
            cacheReplState->UpdateSHiP( set, hitway, hit, pc );
            cacheReplState->PrintResult( set, hitway, replaceway, hit );
        //print replace info
        }else{
            replaceway = cacheReplState->Get_SHiP_Victim( set );
            cout<<"Replace "<<replaceway<<"\n";
            cacheReplState->UpdateSHiP( set, replaceway, hit, pc );
            cacheReplState->PrintResult( set, hitway, replaceway, hit );
        }

        if(index!=numinst-1){
            cout<<"\n";
        }

    }

    fclose(fp_r);
    return 0;
}


