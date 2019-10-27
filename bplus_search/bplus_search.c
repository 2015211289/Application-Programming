//
//  main.c
//  bplus_search
//
//  Created by 谢宇 on 2019/10/23.
//  Copyright © 2019 谢宇. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define LEN 100
#define M 10
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
    int leaf;
    char * s[M];
    int num;
    struct node * p[M];
    struct node * next;
}Bpnode,* Bptree;

int search_array(char * string[],int num, char s[],int f){
    int i=-1;
    for(i=num-1;i>=0;i--){
        int flag=string_cmp(string[i],s,f);
        if(flag==0){
            return -1;
        }else if(flag<0){
            return i+1;
        }
        else continue;
    }
    return 0;
}

int search_tree(char *s,Bptree T){
    int pos=search_array(T->s,T->num,s,1);
    if(T->leaf==1){
        if(pos==-1) return 1;
        else return 0;
    }
    else{
        if(pos==-1) return 1;
        else if(pos==T->num) return 0;
        else return search_tree(s,T->p[pos]);
    }
}

char * max(char *s[],int n){
    char *t=s[0];
    for(int i=1;i<n;i++){
        if(string_cmp(t,s[i],0)<0) t=s[i];
    }
    return t;
}

void seperate_leaf(Bptree p,Bptree T,char *s,int pos,int k1){
    if(pos<k1){
        for(int i=k1-1,j=0;i<M;j++,i++){
            p->s[j]=T->s[i];
        }
        for(int i=k1-2;i>=pos;i--){
            T->s[i+1]=T->s[i];
        }
        T->s[pos]=s;
    }
    else{
        int k=0;
        for(int i=k1,j=0;i<=pos-1;j++,i++,k++){
            p->s[j]=T->s[i];
            k=j;
        }
        p->s[k]=s;
        k++;
        for(int i=pos;i<M;i++,k++){
            p->s[k]=T->s[i];
        }
    }
}
void seperate_node(Bptree p,Bptree T,char *s,Bptree x,int pos,int k1){
    if(pos<k1){
        for(int i=k1-1,j=0;i<M;j++,i++){
            p->s[j]=T->s[i];
            p->p[j]=T->p[i];
        }
        for(int i=k1-2;i>=pos;i--){
            T->s[i+1]=T->s[i];
            T->p[i+1]=T->p[i];
        }
        T->s[pos]=s;
        T->p[pos]=x;
    }
    else{
        int k=0;
        for(int i=k1,j=0;i<=pos-1;j++,i++,k++){
            p->s[j]=T->s[i];
            p->p[j]=T->p[i];
        }
        p->s[k]=s;
        p->p[k]=x;
        k++;
        for(int i=pos;i<M;i++,k++){
            p->s[k]=T->s[i];
            p->p[k]=T->p[i];
        }
    }
}
Bptree insert(char * s,Bptree T,int parent){
    int pos=search_array(T->s,T->num,s,0);
    if(T->leaf==1){// leaf node
        if(pos!=-1){
            if(T->num<M){// no separate
                for(int i=T->num-1;i>=pos;i--){
                    T->s[i+1]=T->s[i];
                }
                T->num++;
                T->s[pos]=s;
                return T;
            }
            else{// separate
                Bptree p=(Bptree)bupt_malloc(sizeof(Bpnode));
                Global_stats.node_num++;
                int k1=ceil((double)(M+1)/2);
                seperate_leaf(p,T,s,pos,k1);
                T->num=k1;
                p->num=ceil((double)M/2);
                T->next=p;
                p->leaf=1;
                if(parent==1){
                    return p;
                }
                else{
                    Bptree pbptree = (Bptree)bupt_malloc(sizeof(Bpnode));
                    pbptree->leaf=0;
                    pbptree->num=2;
                    pbptree->s[0]=T->s[T->num-1];
                    pbptree->p[0]=T;
                    pbptree->s[1]=p->s[p->num-1];
                    pbptree->p[1]=p;
                    Global_stats.node_num++;
                    return pbptree;
                }
            }
        }
        else return T;
    }
    else{
        if(pos!=-1){
            int flag=0;// max key
            if(pos==T->num) {
                flag=1;
                pos--;
            }
            Bptree child;
            child=insert(s,T->p[pos],1);
            if(child!=T->p[pos]){// child node separate
                char * max1 = max(child->s,child->num);
                char * max2 = max(T->p[pos]->s,T->p[pos]->num);
                if(T->num<M){// parent no separate
                    //if(flag==1) pos++;
                    for(int i=T->num-1;i>=pos+1;i--){
                        T->s[i+1]=T->s[i];
                        T->p[i+1]=T->p[i];
                    }
                    T->num++;
                    T->s[pos]=max2;
                    T->s[pos+1]=max1;
                    T->p[pos+1]=child;
                    return T;
                }
                else{// parent separate
                    Bptree bptree=(Bptree)bupt_malloc(sizeof(Bpnode));
                    Global_stats.node_num++;
                    int k1=ceil((double)(M+1)/2);
                    T->s[pos]=max2;
                    seperate_node(bptree,T,max1,child,pos+1,k1);
                    T->num=k1;
                    bptree->num=ceil((double)M/2);
                    bptree->leaf=0;
                    if(parent==1) return bptree;
                    else{
                        Bptree ptr=(Bptree)bupt_malloc(sizeof(Bpnode));
                        ptr->leaf=0;
                        ptr->num=2;
                        ptr->s[0]=T->s[T->num-1];
                        ptr->p[0]=T;
                        ptr->s[1]=bptree->s[bptree->num-1];
                        ptr->p[1]=bptree;
                        Global_stats.node_num++;
                        return ptr;
                    }
                }
            }
            else {//child no separate
                T->s[pos]=max(T->p[pos]->s,T->p[pos]->num);
                return T;
            }
        }
        else return T;
    }
}

void freeTree(Bptree T){
    if(T->leaf==1) free(T);
    else{
        for(int i=0;i<T->num;i++){
            freeTree(T->p[i]);
        }
        free(T);
    }
}

void printTree(Bptree T){
    if(T!=NULL){
        for(int i=0;i<T->num;i++){
            printf("%s ",T->s[i]);
        }
        printf("\n");
        for(int i=0;i<T->num;i++){
            printTree(T->p[i]);
        }

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
    Bptree T = (Bptree) bupt_malloc(sizeof(Bpnode));
    T->num=0;
    T->leaf=1;
    T->next=NULL;
    Global_stats.node_num++;
    char ** pattern = (char **)malloc(sizeof(char *) * MAX);
    // read patterns into array
    FILE * fptr = fopen("patterns.txt","r");
    if(fptr==NULL) {return 0;}
    unsigned long len;
    while((fscanf(fptr,"%s",buf))!=EOF){
        char * t =(char *)malloc(sizeof(char) * LEN);
        len=strlen(buf);
        for(int i=0;i<len;i++){
           t[i]=buf[i];
        }
        t[len]=0;
        pattern[Global_stats.pattern_num]=t;
        Global_stats.pattern_num++;
    };
    fclose(fptr);
    // build B+tree
    for(int i=0;i<Global_stats.pattern_num;i++){
        T=insert(pattern[i],T,0);
    }
    // read words and search
    FILE * readptr =fopen("words-98w.txt","r");
    FILE * writeptr = fopen("result.txt","w");
    if(readptr == NULL || writeptr == NULL) return 0;
    while((fscanf(readptr,"%s",buf))!=EOF){
        Global_stats.word_num++;
        int len=strlen(buf);
        buf[len]=0;
        int flag=search_tree(buf,T);
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
    //printTree(T);
    free(buf);
    freeTree(T);
    for(int i=0;i<Global_stats.pattern_num;i++){
        free(pattern[i]);
    }
    free(pattern);
    return 0;
}
