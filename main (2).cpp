#include <iostream>
#include <fstream>
#include <cstdlib>
#include "replacement_state.h"

using namespace std;

int main()
{
    UINT32 numset;
    UINT32 numway;
    UINT32 policy;
    UINT32 shctentry;
    UINT32 numinst;


    /* FILE * fp_r = NULL;
    fp_r = fopen("testcases/t8.txt","r");
    ofstream f_w("testcases/output8.txt"); */
    //ofstream out("testcases/output1.txt");
    /* fscanf(fp_r,"%d",&numset);
    fscanf(fp_r,"%d",&numway);
    fscanf(fp_r,"%d",&policy);
    fscanf(fp_r,"%d",&shctentry);
    fscanf(fp_r,"%d",&numinst); */
    scanf("%u",&numset);
    scanf("%u",&numway);
    scanf("%u",&policy);
    scanf("%u",&shctentry);
    scanf("%u",&numinst);
    cout<<numset<<"\n"<<numway<<"\n"<<policy<<"\n"<<shctentry<<"\n"<<numinst<<"\n";


    //cout << "Test creating an CACHE_REPLACEMENT_STATE object" << endl;
    //CACHE_REPLACEMENT_STATE * 
    CACHE_REPLACEMENT_STATE * cacheReplState = new CACHE_REPLACEMENT_STATE( numset, numway, policy );
    cacheReplState->SetSHCT( shctentry );
    cacheReplState->InitSHCTState();

    unsigned int serialnum, pc, set, hitway, replaceway;
    bool hit;
    for (unsigned index = 0; index < numinst; index ++){
        /* fscanf(fp_r,"%d",&serialnum);
        fscanf(fp_r,"%d",&pc);
        fscanf(fp_r,"%d",&set); */
        scanf("%u",&serialnum);
        scanf("%u",&pc);
        scanf("%u",&set);

        cout<<serialnum<<" "<<pc<<" "<<set<<" ";

        scanf("%d",&hit);
        if(hit){
            scanf("%d",&hitway);
            cout<<"Hit "<<hitway<<"\n";
            cacheReplState->UpdateSHiP( set, hitway, hit, pc );
            cacheReplState->PrintResult( set, hitway, replaceway, hit );
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
    /* fclose(fp_r);
    f_w.close(); */

    return 0;
}


