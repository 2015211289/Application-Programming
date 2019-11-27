//
//  main.c
//  strstr
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

int main() {
    // initialization
    Global_stats.mem=0;
    Global_stats.cmpnum=0;
    Global_stats.pattern_num=0;
    char * buf= malloc(sizeof(char)*LEN);
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
    while(fgets(buf,LEN,input)!=NULL){
        int len=strlen(buf);
        for(int i=0;i<len;i++){
            patterns[Global_stats.pattern_num].s[i]=buf[i];
        }
        patterns[Global_stats.pattern_num].s[len-1]=0;
        patterns[Global_stats.pattern_num].compatible=0;
        int str_len=strLen;
        for(int i=0;i<str_len-len+2;i++){
            int j=0;
            for(;j<len-1;j++){
                if(byte_cmp(string[i],buf[j],1)!=0){
                    break;
                }
                i++;
            }
            if(j==len-1){
                patterns[Global_stats.pattern_num].compatible++;
            }
            i=i-j;
        }
        Global_stats.pattern_num++;
        printf("%ld\n",Global_stats.pattern_num);
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