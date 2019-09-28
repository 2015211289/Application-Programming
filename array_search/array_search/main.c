//
//  main.c
//  array_search
//
//  Created by 谢宇 on 2019/9/26.
//  Copyright © 2019 谢宇. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1500000
#define LEN 30

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
};

struct word{
    char s[LEN];
};

void QuickSort(struct pattern ** a, long left, long right)
{
    if(left >= right)
    {
        return;
    }
    long i = left;
    long j = right;
    struct pattern * key = a[left];
     
    while(i < j)
    {
        while(i < j && string_cmp(key->s, a[j]->s)<=0)
        {
            j--;
        }
        a[i] = a[j];
        while(i < j && string_cmp(key->s, a[i]->s)>=0)
        {
            i++;
        }
        a[j] = a[i];
    }
    a[i] = key;
    QuickSort(a, left, i - 1);
    QuickSort(a, i + 1, right);
}

int binary_search(char * key,struct pattern ** a,long n)
{
    long low,high,mid;
    int flag=-1;
    low=0;
    high=n-1;
    while(low<=high)
    {
        mid=(low+high)/2;
        if(string_cmp(key, a[mid]->s)<0)
            high=mid-1;
        else if(string_cmp(key, a[mid]->s)>0)
            low=mid+1;
        else if(string_cmp(key, a[mid]->s)==0)
        {
            flag=1;
            break;
        }
    }
    return flag;
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
    unsigned long len;
    while((fscanf(fptr,"%s",buf))!=EOF){
        struct pattern * p = (struct pattern *)bupt_malloc(sizeof(struct pattern));
        len=strlen(buf);
        for(int i=0;i<len;i++){
            p->s[i]=buf[i];
        }
        p->s[len]=0;
        patterns[Global_stats.pattern_num]=p;
        Global_stats.pattern_num++;
    };
    fclose(fptr);
    // sort the array
    QuickSort(patterns, 0, Global_stats.pattern_num-1);
    // read words
    FILE * readptr =fopen("words-98w.txt","r");
    FILE * writeptr = fopen("result.txt","w");
    if(readptr == NULL || writeptr == NULL) return 0;
    while((fscanf(readptr,"%s",buf))!=EOF){
        Global_stats.word_num++;
        int flag=binary_search(buf, patterns, Global_stats.pattern_num);
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
    for(int i=0;i<Global_stats.pattern_num;i++){
        free(patterns[i]);
    }
    free(patterns);
    free(buf);
    
    return 0;
}
