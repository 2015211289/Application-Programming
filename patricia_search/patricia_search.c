//
//  main.c
//  patricia_search
//
//  Created by 谢宇 on 2019/10/27.
//  Copyright © 2019 谢宇. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1500000
#define LEN 100
#define M 2

struct {
    long node_num;
    long cmpnum;
    long mem;
    long pattern_num;
    long word_num;
    long compatible;
}Global_stats;

int byte_cmp(char a, char b,int flag){
    if(flag==1) Global_stats.cmpnum++;// 1:search 0:build
    return a-b;
}

int string_cmp(char *a, char *b,int flag){
    while(byte_cmp(*a, *b,flag)==0){
        if(byte_cmp(*a,0,flag)==0) return 0;
        else {
            a++;
            b++;
        }
    }
    return byte_cmp(*a, *b,flag);
}


void* bupt_malloc(size_t size){
    if (size <= 0) {
        return NULL;
    }
    Global_stats.mem += size;
    return malloc(size);
}

typedef struct {
    char s[LEN];
    int len;
}Pattern;

typedef struct node{
    int nodeKind;// 1:leaf 0:internal
    union{
        struct a{
            char * key;
            Pattern * record;
        }lf;
        struct b{
            struct node * ptr[M+1];
            int num;
            int start;
        }bh;
    };
}RadixNode,* RadixTree;

int cmp_bit(char s,int pos,int flag){
    uint32_t t1,mask;
    unsigned char x=s;
    mask=0x000000FF;
    t1=(x|(1<<pos))&mask;
    int f1=byte_cmp(t1,x,flag);
    if(f1==0) return 2;
    else return 1;
}

RadixTree insert(char *string,RadixTree T,Pattern * pattern,int pre){
    if(T==NULL){
        RadixTree P=(RadixTree)bupt_malloc(sizeof(RadixNode));
        P->nodeKind=1;
        P->lf.key=pattern->s;
        P->lf.record=pattern;
        Global_stats.node_num++;
        return P;
    }
    else if(T->nodeKind==1){
        RadixTree P = (RadixTree)bupt_malloc(sizeof(RadixNode));
        Global_stats.node_num++;
        for(int i=0;i<=M;i++){
            P->bh.ptr[i]=NULL;
        }
        P->nodeKind=0;
        P->bh.start=pre+1;
        P->bh.num=1;
        int f=P->bh.start;
        int pos=f/8;
        int t=f%8;
        if(T->lf.record->len<=pos){// leaf end
            P->bh.ptr[0]=T;
            P=insert(string,P,pattern,pre);
            return P;
        }
        else{// leaf not end
            int ans2=cmp_bit(T->lf.key[pos],t,0);
            P->bh.ptr[ans2]=T;
            P=insert(string,P,pattern,pre);
            return P;
        }
    }
    else{
        int f=T->bh.start;
        int pos=f/8;
        int t=f%8;
        if(pattern->len<=pos){ // string end
            if(T->bh.ptr[0]==NULL) {
                T->bh.num++;
                T->bh.ptr[0]=insert(string,T->bh.ptr[0],pattern,T->bh.start);
            }
        }
        else{// string not end
            int ans=cmp_bit(string[pos],t,0);
            if(T->bh.ptr[ans]==NULL) T->bh.num++;
            T->bh.ptr[ans]=insert(string,T->bh.ptr[ans],pattern,T->bh.start);
        }
        return T;
    }
}

RadixTree compress(RadixTree T){
    if(T!=NULL){
        if(T->nodeKind==0){ // internal node
            if(T->bh.num==1){ // only child
                RadixTree t;
                for(int i=0;i<=M;i++){
                    if(T->bh.ptr[i]!=NULL) {
                        t=T->bh.ptr[i];
                        break;
                    }
                }
                t=compress(t);
                free(T);
                Global_stats.node_num--;
                Global_stats.mem-= sizeof(T);
                return t;
            }
            else{// more than one child
                for(int i=0;i<=M;i++){
                    if(T->bh.ptr[i]!=NULL) T->bh.ptr[i]=compress(T->bh.ptr[i]);
                }
                return T;
            }
        }
        else return T;// leaf node
    }
    else return T;
}

int search(RadixTree T,char *s,int len){
    if(T==NULL) return 0;
    else{
        if(T->nodeKind==1){
            int ans=string_cmp(T->lf.key,s,1);
            if(ans==0) return 1;
            else return 0;
        }
        else {
            int f = T->bh.start;
            int pos = f / 8;
            int t = f % 8;
            if (pos >= len) return search(T->bh.ptr[0], s, len);
            else {
                int ans = cmp_bit(s[pos], t,1);
                return search(T->bh.ptr[ans], s, len);
            }
        }
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

int main() {
    // initial operation
    Global_stats.cmpnum=0;
    Global_stats.mem=0;
    Global_stats.pattern_num=0;
    Global_stats.word_num=0;
    Global_stats.compatible=0;
    Global_stats.node_num=1;
    char * buf = malloc(sizeof(char) * LEN);
    Pattern * pattern = (Pattern *)malloc(sizeof(Pattern) * MAX);
    RadixTree T=(RadixTree)bupt_malloc(sizeof(RadixNode));
    T->nodeKind=0;
    for(int i=0;i<=M;i++){
        T->bh.ptr[i]=NULL;
    }
    T->bh.start=0;
    T->bh.num=0;
    // read patterns into pattern array and insert into Radix tree
    FILE * fptr = fopen("patterns.txt","r");
    if(fptr==NULL) {return 0;}
    while((fscanf(fptr,"%s",buf))!=EOF){
        int len=strlen(buf);
        for(int i=0;i<=len;i++){
            pattern[Global_stats.pattern_num].s[i]=buf[i];
        }
        pattern[Global_stats.pattern_num].len=len;
        T=insert(pattern[Global_stats.pattern_num].s,T,&pattern[Global_stats.pattern_num],-1);
        Global_stats.pattern_num++;
    }
    fclose(fptr);
    // compress the tree to minimize the memory
    T=compress(T);
    // read words
    FILE * readptr =fopen("words-98w.txt","r");
    FILE * writeptr = fopen("result.txt","w");
    if(readptr == NULL || writeptr == NULL) return 0;
    while((fscanf(readptr,"%s",buf))!=EOF){
        Global_stats.word_num++;
        if(search(T,buf,strlen(buf))==1){
            fprintf(writeptr,"%s yes\n",buf);
            Global_stats.compatible++;
        }
        else{
            fprintf(writeptr,"%s no\n",buf);
        }
    }
    fprintf(writeptr,"%ld %ld %ld %ld %ld",Global_stats.node_num,Global_stats.mem/1024,
            Global_stats.cmpnum/1024,Global_stats.word_num,Global_stats.compatible);
    fclose(readptr);
    fclose(writeptr);
    // free memory
    freeTree(T);
    free(buf);
    free(pattern);
    return 0;
}