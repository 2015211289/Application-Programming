//
//  main.c
//  hashtable_search
//
//  Created by 谢宇 on 2019/9/27.
//  Copyright © 2019 谢宇. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GeneralHashFunctions.h"

#define MAX 1270000
#define LEN 30
hash_function hash = PJWHash;

struct {
    long cmpnum;
    long mem;
    long pattern_num;
    long word_num;
    long compatible;
}Global_stats;

int byte_cmp(char a, char b){
    Global_stats.cmpnum++;
    return a-b;
}

int string_cmp(char *a, char *b){
    while(byte_cmp(*a, *b)==0){
        if(byte_cmp(*a,0)==0) return 0;
        else {
            a++;
            b++;
        }
    }
    return byte_cmp(*a, *b);
}

void* bupt_malloc(size_t size){
    if (size <= 0) {
            return NULL;
    }
    Global_stats.mem += size;
    return malloc(size);
}

struct pattern{
    char s[LEN];
    struct pattern * next;
};

int search(char * s,int len, struct pattern ** a){
    int position = hash(s,len) % MAX;
    struct pattern * p = a[position];
    while(p!=NULL){
        if(string_cmp(p->s,s)==0){
            return 0;
        }
        else p=p->next;
    }
    return -1;
}

void free_vector(struct pattern * p){
    if(p==NULL) return;
    else{
        free_vector(p->next);
        free(p);
    }
}

int main(int argc, const char * argv[]) {
    // initial operation
    Global_stats.cmpnum=0;
    Global_stats.mem=0;
    Global_stats.pattern_num=0;
    Global_stats.word_num=0;
    Global_stats.compatible=0;
    char * buf = bupt_malloc(sizeof(char)*LEN);
    // read patterns into array
    FILE * fptr = fopen("patterns.txt","r");
    if(fptr==NULL) {return 0;}
    struct pattern ** patterns = (struct pattern **)bupt_malloc(sizeof(struct pattern *) * MAX);
    for(int i=0;i<MAX;i++){
        patterns[i]=NULL;
    }
    int len,position;
    while((fscanf(fptr,"%s",buf))!=EOF){
        len=strlen(buf);
        struct pattern * p =(struct pattern *)bupt_malloc(sizeof(struct pattern));
        for(int i=0;i<len;i++){
            p->s[i]=buf[i];
        }
        p->s[len]=0;
        p->next=NULL;
        position = hash(buf,len) % MAX;
        struct pattern * r=patterns[position];
        patterns[position]=p;
        p->next=r;
        Global_stats.pattern_num++;
    };
    fclose(fptr);
    // read words to match patterns
    FILE * readptr =fopen("words-98w.txt","r");
    FILE * writeptr = fopen("result.txt","w");
    if(readptr == NULL || writeptr == NULL) return 0;
    while((fscanf(readptr,"%s",buf))!=EOF){
        Global_stats.word_num++;
        int len=strlen(buf);
        int flag=search(buf, len, patterns);
        if(flag==-1){
            fprintf(writeptr,"%s no\n",buf);
        }
        else {
            fprintf(writeptr,"%s yes\n",buf);
            Global_stats.compatible++;
        }
    }
    fprintf(writeptr,"%ld %ld %ld %ld",Global_stats.mem/1024,Global_stats.cmpnum,Global_stats.word_num,Global_stats.compatible);
    fclose(readptr);
    fclose(writeptr);
    // free memory
    free(buf);
    for(int i=0;i<MAX;i++){
        struct pattern * p=patterns[i];
        free_vector(p);
    }
    free(patterns);
    return 0;
}
