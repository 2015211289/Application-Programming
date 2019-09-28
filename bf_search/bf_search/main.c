//
//  main.c
//  bf_search
//
//  Created by 谢宇 on 2019/9/27.
//  Copyright © 2019 谢宇. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 43000000 // f=0.00001% n=1270000
#define K 23 // hash number
#define NUM 5375000 // char array length
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

void* bupt_malloc(size_t size){
    if (size <= 0) {
        return NULL;
    }
    Global_stats.mem += size;
    return malloc(size);
}

uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed) {
    uint32_t h = seed;
    if (len > 3) {
        const uint32_t* key_x4 = (const uint32_t*) key;
        size_t i = len >> 2;
        do {
            uint32_t k = *key_x4++;
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;
            h ^= k;
            h = (h << 13) | (h >> 19);
            h = (h * 5) + 0xe6546b64;
        } while (--i);
        key = (const uint8_t*) key_x4;
    }
    if (len & 3) {
        size_t i = len & 3;
        uint32_t k = 0;
        key = &key[i - 1];
        do {
            k <<= 8;
            k |= *key--;
        } while (--i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k; }
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

void fill_vector(char * a, size_t len, char * vector){
    for(int i=0;i<K;i++){
        uint32_t result= murmur3_32((uint8_t*)a, len, i)%M;
        uint32_t position=result/8;
        uint32_t t=result%8;
        vector[position] |= (1<<t);
    }
}

int word_filter(char * a, size_t len, char * vector){
    int flag=-1;
    for(int i=0;i<K;i++){
        uint32_t result = murmur3_32(a, len, i)%M;
        uint32_t position=result/8;
        uint32_t t=result%8;
        if(byte_cmp(vector[position] | (1<<t),vector[position])!=0) return flag;
    }
    return 1;
}

int main(int argc, const char * argv[]) {
    // initial operation
    Global_stats.cmpnum=0;
    Global_stats.mem=0;
    Global_stats.pattern_num=0;
    Global_stats.word_num=0;
    Global_stats.compatible=0;
    char * buf = bupt_malloc(sizeof(char)*LEN);
    char * vector = (char *)bupt_malloc(sizeof(char) * NUM);
    for(int i=0;i<NUM;i++){
        vector[i]=0;
    }
    // read patterns into Bloom Filter
    FILE * fptr = fopen("patterns.txt","r");
    if(fptr==NULL) {return 0;}
    while((fscanf(fptr,"%s",buf))!=EOF){
        fill_vector(buf, strlen(buf), vector);
        Global_stats.pattern_num++;
    }
    fclose(fptr);
    // read words
    FILE * readptr =fopen("words-98w.txt","r");
    FILE * writeptr = fopen("result.txt","w");
    if(readptr == NULL || writeptr == NULL) return 0;
    while((fscanf(readptr,"%s",buf))!=EOF){
        Global_stats.word_num++;
        if(word_filter(buf, strlen(buf), vector)==1){
            fprintf(writeptr,"%s yes\n",buf);
            Global_stats.compatible++;
        }
        else{
            fprintf(writeptr,"%s no\n",buf);
        }
    }
    fprintf(writeptr,"%ld %ld %ld %ld",Global_stats.mem/1024,Global_stats.cmpnum,Global_stats.word_num,Global_stats.compatible);
    fclose(readptr);
    fclose(writeptr);
    // free memory
    free(buf);
    free(vector);
    return 0;
}
