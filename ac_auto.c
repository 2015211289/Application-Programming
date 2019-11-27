//
//  main.c
//  ac_auto
//
//  Created by 谢宇 on 2019/11/21.
//  Copyright © 2019 谢宇. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 100
#define MAX 2300000
#define STR_LEN 1000000000
#define M 2

struct {
    long long cmpnum;
    long long mem;
    long pattern_num;
}Global_stats;

typedef struct {
    char s[LEN];
    int compatible;
}Patterns;

typedef struct a{
    int n;
    struct a * next;
}*Out,Outnode;

typedef struct node{
    struct node * ptr[M];
    Out output;
    struct node * fail;
    int start;
}AutoNode,* AutoTree;

typedef struct b {
    AutoTree p;
    struct b * next;
}element;

struct {
    struct b * front;
    struct b * rear;
    int size;
}Queue;

void queue_push(AutoTree a){
    element *t=(element*)malloc(sizeof(element));
    t->p=a;
    t->next=NULL;
    Queue.size++;
    if(Queue.size==1){
        Queue.front=t;
        Queue.rear=t;
    }
    else{
        Queue.rear->next=t;
        Queue.rear=t;
    }
}

AutoTree queue_pop(){
    element * t=Queue.front;
    Queue.front=t->next;
    Queue.size--;
    if(Queue.size==0){
        Queue.rear=NULL;
        Queue.front=NULL;
    }
    AutoTree e=t->p;
    free(t);
    return e;
}

int queue_empty(){
    if(Queue.size==0) return 1;
    else return 0;
}

int byte_cmp(char a, char b,int flag){
    if(flag==1) Global_stats.cmpnum++;// 1:search 0:build
    return a-b;
}

int cmp_bit(char s,int pos,int flag){
    uint32_t t1,mask;
    unsigned char x=s;
    mask=0x000000FF;
    t1=(x|((unsigned int)1<<(unsigned int)(7-pos)))&mask;
    int f1=byte_cmp(t1,(char)x,flag);
    if(f1==0) return 1;
    else return 0;
}

void* bupt_malloc(size_t size){
    if (size <= 0) {
        return NULL;
    }
    Global_stats.mem += size;
    return malloc(size);
}

AutoTree insert(Patterns * patterns,int n,AutoTree T,int pre){
    int len=strlen(patterns[n].s);
    if(T==NULL){
        AutoTree A=(AutoTree)bupt_malloc(sizeof(AutoNode));
        for(int i=0;i<M;i++){
            A->ptr[i]=NULL;
        }
        A->output=NULL;
        A->start=pre+1;
        int f=A->start;
        int pos=f/8;
        int t=f%8;
        if(len<=pos){// pattern end
            Out O=(Out)bupt_malloc(sizeof(Outnode));
            O->n=n;
            O->next=NULL;
            A->output=O;
            return A;
        }
        else{// pattern not end
            int ans=cmp_bit(patterns[n].s[pos],t,0);
            A->ptr[ans]=insert(patterns,n,A->ptr[ans],A->start);
            return A;
        }
    }
    else{
        int f=T->start;
        int pos=f/8;
        int t=f%8;
        if(len<=pos){// pattern end
            Out O=(Out)bupt_malloc(sizeof(Outnode));
            O->n=n;
            Out p=T->output;
            T->output=O;
            O->next=p;
            return T;
        }
        else{// pattern not end
            int ans=cmp_bit(patterns[n].s[pos],t,0);
            T->ptr[ans]=insert(patterns,n,T->ptr[ans],T->start);
            return T;
        }
    }
}

void setFail(AutoTree rout){
    queue_push(rout);
    while(queue_empty()!=1){
        AutoTree T=queue_pop();
        for(int i=0;i<M;i++){
            if(T->ptr[i]!=NULL){
                AutoTree t=T->ptr[i];
                if(t->start==1) t->fail=rout;
                else{
                    AutoTree x=T->fail;
                    while(x->ptr[i]==NULL){
                        x=x->fail;
                    }
                    t->fail=x->ptr[i];
                }
                queue_push(t);
            }
        }
    }
}
void HeapAdjust(Patterns a[],int s,int m)
{
    int j;
    Patterns rc;
    rc=a[s];
    for(j=2*s;j<=m;j=j*2)
    {
        if(j<m&&a[j].compatible<a[j+1].compatible) j++;
        if(rc.compatible>a[j].compatible) break;
        a[s]=a[j];
        s=j;
    }
    a[s]=rc;
}
void HeapSort(Patterns a[],int n) {
    int  i, j;
    Patterns temp;
    for (i = n / 2; i > 0; i--)
    {
        HeapAdjust(a, i, n);
    }
    for (i = n; i >0; i--) {
        temp = a[1];
        a[1] = a[i];
        a[i] = temp;
        HeapAdjust(a, 1, i - 1);
    }
}

void freeTree(AutoTree T){
    for(int i=0;i<M;i++){
        if(T->ptr[i]!=NULL){
            freeTree(T->ptr[i]);
        }
    }
    Out out=T->output;
    while(out!=NULL){
        Out pre=out;
        out=out->next;
        free(pre);
    }
    free(T);
}

int main() {
    // initialization
    Global_stats.mem=0;
    Global_stats.cmpnum=0;
    Global_stats.pattern_num=1;
    Patterns * patterns=(Patterns*)bupt_malloc(sizeof(Patterns)*MAX);
    char * string = bupt_malloc(sizeof(char)*STR_LEN);
    FILE * input = fopen("pattern.txt","r");
    FILE * stringInput = fopen("../string.txt","r");
    AutoTree T=(AutoTree)bupt_malloc(sizeof(AutoNode));
    T->output=NULL;
    T->start=0;
    for(int i=0;i<M;i++){
        T->ptr[i]=NULL;
    }
    T->fail=T;
    Queue.size=0;
    Queue.front=NULL;
    Queue.rear=NULL;
    // read patterns
    long long strLen=0;
    while(!feof(stringInput)){
        int c=fgetc(stringInput);
        string[strLen]=c;
        strLen++;
    }
    printf("string done\n");
    while(fgets(patterns[Global_stats.pattern_num].s,LEN,input)!=NULL) {
        int len = strlen(patterns[Global_stats.pattern_num].s) - 1;
        patterns[Global_stats.pattern_num].s[len] = 0;
        patterns[Global_stats.pattern_num].compatible = 0;
        Global_stats.pattern_num++;
    }
    printf("patterns done\n");
    // build ac_auto
    for(long k=1;k<Global_stats.pattern_num;k++) {
        T = insert(patterns, k, T, -1);
    }
    setFail(T);
    printf("ac_auto done\n");
    // search ac_auto
    long long i=0;
    long long S=strLen*8;
    while(i<S) {
        long long f = i;
        long long pos = f / 8;
        int t = f % 8;
        int ans=cmp_bit(string[pos],t,1);
        if(T->ptr[ans]!=NULL) {
            T=T->ptr[ans];
            i++;
            Out O=T->output;
            AutoTree temp=T;
            while(temp->start!=0){
                while(O!=NULL){
                    patterns[O->n].compatible++;
                    O=O->next;
                }
                temp=temp->fail;
                O=temp->output;
            }
        }
        else if(T->start==0){
            i++;
        }
        else {
            T=T->fail;
        }
    }

    // print results
    free(string);
    freeTree(T);
    HeapSort(patterns,Global_stats.pattern_num-1);
    FILE * out = fopen("result.txt","w");
    for(long i=Global_stats.pattern_num-1;i>=1;i--){
        fprintf(out,"%s %d\n",patterns[i].s,patterns[i].compatible);
    }
    fprintf(out,"%lld %lld",Global_stats.cmpnum/1000,Global_stats.mem/1024);
    fclose(out);

    // free memory
    fclose(input);
    fclose(stringInput);
    free(patterns);
    return 0;
}