//
// Created by 谢宇 on 2019/12/9.
//

#ifndef BCRAWLER_TRIE_H
#define BCRAWLER_TRIE_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

#define URL_LEN 1000
#define M 2

typedef struct x{
    int n;
    struct x * next;
}*LINK,LINKnode;

typedef struct{
    char s[URL_LEN];
    LINK link;
    int num;
}*URL,URLstruct;

typedef struct node{
    int nodeKind;// 1:leaf 0:internal
    union{
        struct a{
            URL record;
        }lf;
        struct b{
            struct node * ptr[M+1];
            int num;
            int start;
        }bh;
    };
}RadixNode,* RadixTree;

struct {
    long mem;
    long num;
}Global_stats;


static void* bupt_malloc(size_t size){
    if (size <= 0) {
        return NULL;
    }
    Global_stats.mem += size;
    return malloc(size);
}

static int string_cmp(char *a, char *b,int flag){
    while((*a)-(*b)==0){
        if((*a)==0) return 0;
        else {
            a++;
            b++;
        }
    }
    return (*a)-(*b);
}

static int cmp_bit(char s,int pos,int flag){
    uint32_t t1,mask;
    unsigned char x=s;
    mask=0x000000FF;
    t1=(x|(1<<pos))&mask;
    int f1=t1-x;
    if(f1==0) return 2;
    else return 1;
}

extern RadixTree insert(char *string,RadixTree T,URL pattern,int pre,int * flag,int *node);
extern void freeTree(RadixTree T);
extern int search(RadixTree T,char *s,int len,int * node);

#endif //BCRAWLER_TRIE_H
