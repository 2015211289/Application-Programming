//
//  main.c
//  Multikmp
//
//  Created by 谢宇 on 2019/11/20.
//  Copyright © 2019 谢宇. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 100
#define MAX 20000
#define STR_LEN 1000000000

struct {
    long long cmpnum;
    long mem;
    long pattern_num;
}Global_stats;

typedef struct {
    char s[LEN];
    int compatible;
}Patterns;

int byte_cmp(char a, char b,int flag){
    if(flag==1) Global_stats.cmpnum++;// 1:search 0:build
    return a-b;
}

void* bupt_malloc(size_t size){
    if (size <= 0) {
        return NULL;
    }
    Global_stats.mem += size;
    return malloc(size);
}
void QuickSort(Patterns * a, long left, long right)
{
    if(left >= right)
    {
        return;
    }
    long i = left;
    long j = right;
    Patterns  key = a[left];

    while(i < j)
    {
        while(i < j && key.compatible<=a[j].compatible)
        {
            j--;
        }
        a[i] = a[j];
        while(i < j && key.compatible>=a[i].compatible)
        {
            i++;
        }
        a[j] = a[i];
    }
    a[i] = key;
    QuickSort(a, left, i - 1);
    QuickSort(a, i + 1, right);
}

void getNext(char * t, int * next){
    int i=0,j=-1;
    next[0]=-1;
    int len=(int)strlen(t);
    while(i<len){
        if(j==-1 || t[i]==t[j])
        {
            i++;
            j++;
            if(t[i]!=t[j]) next[i]=j;
            else next[i]=next[j];
        }
        else{
            j=next[j];
        }
    }
}

int main() {
    // initialization
    Global_stats.mem=0;
    Global_stats.cmpnum=0;
    Global_stats.pattern_num=0;
    int * next = (int *)bupt_malloc(sizeof(int) *LEN);
    Patterns * patterns=(Patterns*)bupt_malloc(sizeof(Patterns)*MAX);
    char * string = bupt_malloc(sizeof(char)*STR_LEN);
    FILE * input = fopen("pattern_bf_kmp.txt","r");
    FILE * stringInput = fopen("../string.txt","r");
    // read patterns
    int strLen=0;
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
    for(long k=0;k<Global_stats.pattern_num;k++){
        int len = strlen(patterns[k].s);
        getNext(patterns[k].s,next);
        int i=0,j=0;
        while(i<strLen){
            if(j==-1||byte_cmp(string[i],patterns[k].s[j],1)==0){
                i++;
                j++;
                if(j>=len){
                    patterns[k].compatible++;
                    j=next[j];
                }
            }
            else{
                j=next[j];
            }
        }
        printf("%ld\n",k);
    }

    fclose(input);
    fclose(stringInput);

    // print results
    QuickSort(patterns,0,Global_stats.pattern_num-1);
    FILE * out = fopen("result.txt","w");
    for(long i=Global_stats.pattern_num-1;i>=0;i--){
        fprintf(out,"%s %d\n",patterns[i].s,patterns[i].compatible);
    }
    fprintf(out,"%lld %ld",Global_stats.cmpnum/1000,Global_stats.mem/1024);
    fclose(out);

    // free memory
    free(patterns);
    free(string);
    return 0;
}