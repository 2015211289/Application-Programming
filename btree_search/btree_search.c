//
//  main.c
//  btree_search
//
//  Created by 谢宇 on 2019/10/22.
//  Copyright © 2019 谢宇. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 100
#define MAX 1500000

struct {
    long cmpnum;
    long mem;
    long pattern_num;
    long word_num;
    long compatible;
    long node_num;
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

typedef struct node{
    char * s;
    struct node * lc;
    struct node * rc;
    int height;
}*Btree,Bnode;

int get_height(Btree a){
    if(a==NULL) return 0;
    else return a->height;
}

int max(int a,int b)
{
    return a>b?a:b;
}

Btree SingleRotateWithLeft(Btree x)
{
    Btree left = x->lc;
    x->lc=left->rc;
    left->rc=x;
    x->height=max(get_height(x->lc),get_height(x->rc))+1;
    left->height=max(get_height(left->lc),get_height(left->rc))+1;
    return left;
}

Btree SingleRotateWithRight(Btree x)
{
    Btree right;
    right=x->rc;
    x->rc=right->lc;
    right->lc=x;
    x->height=max(get_height(x->lc),get_height(x->rc))+1;
    right->height=max(get_height(right->rc),x->height)+1;
    return right;
}

Btree DoubleRotateWithLeft(Btree x)
{
    x->lc=SingleRotateWithRight(x->lc);
    return SingleRotateWithLeft(x);
}

Btree DoubleRotateWithRight(Btree x)
{
    x->rc=SingleRotateWithLeft(x->rc);
    return SingleRotateWithRight(x);
}

Btree insert(char *s,Btree x)
{
    if(x==NULL)
    {
        x=bupt_malloc(sizeof(Bnode));
        x->s=s;
        x->height=0;
        x->lc=x->rc=0;
        Global_stats.node_num++;
    }
    else if(string_cmp(s,x->s,0)<0)
    {
        x->lc=insert(s,x->lc);
        if(get_height(x->lc)-get_height(x->rc)==2){
            if(string_cmp(s,x->lc->s,0)<0)
                x=SingleRotateWithLeft(x);
            else{
                x=DoubleRotateWithLeft(x);
            }
        }
    }
    else if(string_cmp(s,x->s,0)>0)
    {
        x->rc=insert(s,x->rc);
        if(get_height(x->rc)-get_height(x->lc)==2){
            if(string_cmp(s,x->rc->s,0)>0)
                x=SingleRotateWithRight(x);
            else
                x=DoubleRotateWithRight(x);
        }
    }
    x->height=max(get_height(x->lc),get_height(x->rc))+1;
    return x;
}

int search(char s[],Btree T){
    if(T==NULL) return 0;
    else{
        if(string_cmp(s,T->s,1)==0) return 1;
        else if(string_cmp(s,T->s,1)<0) return search(s,T->lc);
        else return search(s,T->rc);
    }
}

void freeTree(Btree T){
    if(T!=NULL){
        freeTree(T->lc);
        freeTree(T->rc);
        free(T);
    }
}

int main() {
    // initialization
    Global_stats.cmpnum = 0;
    Global_stats.mem=0;
    Global_stats.pattern_num=0;
    Global_stats.word_num=0;
    Global_stats.compatible=0;
    Global_stats.node_num=0;
    char * buf = malloc(sizeof(char)*LEN);
    char ** pattern = (char **)malloc(sizeof(char *) * MAX);
    Btree T=NULL;
    // read patterns into array
    FILE * fptr = fopen("patterns.txt","r");
    if(fptr==NULL) {return 0;}
    unsigned long len;
    while((fscanf(fptr,"%s",buf))!=EOF){
        len=strlen(buf);
        char * t =(char *)malloc(sizeof(char) * LEN);
        for(int i=0;i<=len;i++){
            t[i]=buf[i];
        }
        pattern[Global_stats.pattern_num]=t;
        T=insert(t,T);
        Global_stats.pattern_num++;
    };
    fclose(fptr);
    // read words and search
    FILE * readptr =fopen("words-98w.txt","r");
    FILE * writeptr = fopen("result.txt","w");
    if(readptr == NULL || writeptr == NULL) return 0;
    while((fscanf(readptr,"%s",buf))!=EOF){
        Global_stats.word_num++;
        int flag=search(buf,T);
        if(flag==0){
            fprintf(writeptr,"%s no\n",buf);
        }
        else {
            fprintf(writeptr,"%s yes\n",buf);
            Global_stats.compatible++;
        }
    }
    fprintf(writeptr,"%ld %ld %ld %ld %ld",Global_stats.node_num,Global_stats.mem/1024,Global_stats.cmpnum/1000,Global_stats.word_num,Global_stats.compatible);
    fclose(readptr);
    fclose(writeptr);
    // free memory
    free(buf);
    freeTree(T);
    for(int i=0;i<Global_stats.pattern_num;i++){
        free(pattern[i]);
    }
    free(pattern);
    return 0;
}