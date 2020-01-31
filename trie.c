//
//  tire.c
//  tire tree to store URLs and check repeated URL
//
//  Created by 谢宇 on 2019/12/7.
//  Copyright © 2019 谢宇. All rights reserved.
//

#include "trie.h"

RadixTree insert(char *string,RadixTree T,URL pattern,int pre,int * flag,int * node){
    if(T==NULL){
        RadixTree P=(RadixTree)bupt_malloc(sizeof(RadixNode));
        P->nodeKind=1;
        P->lf.record=pattern;
        *flag=1;//a new URL
        return P;
    }
    else if(T->nodeKind==1){
        RadixTree P = (RadixTree)bupt_malloc(sizeof(RadixNode));
        for(int i=0;i<=M;i++){
            P->bh.ptr[i]=NULL;
        }
        P->nodeKind=0;
        P->bh.start=pre+1;
        P->bh.num=1;
        int f=P->bh.start;
        int pos=f/8;
        int t=f%8;
        if(strlen(T->lf.record->s)<=pos){// leaf end
            P->bh.ptr[0]=T;
            P=insert(string,P,pattern,pre,flag,node);
            return P;
        }
        else{// leaf not end
            int ans2=cmp_bit(T->lf.record->s[pos],t,0);
            P->bh.ptr[ans2]=T;
            P=insert(string,P,pattern,pre,flag,node);
            return P;
        }
    }
    else{
        int f=T->bh.start;
        int pos=f/8;
        int t=f%8;
        if(strlen(pattern->s)<=pos){ // string end
            if(T->bh.ptr[0]==NULL) {
                T->bh.num++;
                T->bh.ptr[0]=insert(string,T->bh.ptr[0],pattern,T->bh.start,flag,node);
            }
            else {// a repeated URL
                *flag=0;
                *node=T->bh.ptr[0]->lf.record->num;
            }
        }
        else{// string not end
            int ans=cmp_bit(string[pos],t,0);
            if(T->bh.ptr[ans]==NULL) T->bh.num++;
            T->bh.ptr[ans]=insert(string,T->bh.ptr[ans],pattern,T->bh.start,flag,node);
        }
        return T;
    }
}

void freeTree(RadixTree T){
    if(T!=NULL){
        if(T->nodeKind==0){
            for(int i=0;i<=M;i++){
                freeTree(T->bh.ptr[i]);
            }
            free(T);
        }
        else{
            free(T);
        }
    }
}
int search(RadixTree T,char *s,int len,int *node){
    if(T==NULL) return 0;
    else{
        if(T->nodeKind==1){
            int ans=string_cmp(T->lf.record->s,s,0);
            if(ans==0) {
                *node=T->lf.record->num;
                return 1;
            }
            else return 0;
        }
        else {
            int f = T->bh.start;
            int pos = f / 8;
            int t = f % 8;
            if (pos >= len) return search(T->bh.ptr[0], s, len,node);
            else {
                int ans = cmp_bit(s[pos], t,1);
                return search(T->bh.ptr[ans], s, len,node);
            }
        }
    }
}
