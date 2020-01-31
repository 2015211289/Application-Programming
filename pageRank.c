#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_SIZE 700000
#define N 150000
#define A 0.15

typedef struct b{
    char s[500];
    double value;
}*URL,URLnode;

typedef struct a{
    int row;
    int col;
    double value;
}*Matrix,Matrixnode;

struct {
    long mem;
    long num;
    long size;
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

double distance(double * v1,double * v2){
    double num=0;
    for(int i=0;i<Global_stats.size;i++){
        double t=(pow(v1[i]-v2[i],2));
        num+=t;
    }
    return num;
}

void QuickSort(URL a, long left, long right)
{
    if(left >= right)
    {
        return;
    }
    long i = left;
    long j = right;
    URLnode  key = a[left];

    while(i < j)
    {
        while(i < j && key.value<=a[j].value)
        {
            j--;
        }
        a[i] = a[j];
        while(i < j && key.value>=a[i].value)
        {
            i++;
        }
        a[j] = a[i];
    }
    a[i] = key;
    QuickSort(a, left, i - 1);
    QuickSort(a, i + 1, right);
}

int main(int argc, char *argv[]) {
    Global_stats.num=0;
    Global_stats.mem=0;
    Global_stats.size=0;
    if(argc!=3) exit(1);
    FILE * input;
    FILE * output;
    input=fopen(argv[1],"r");
    output=fopen(argv[2],"w");
    Matrix matrix=(Matrix)bupt_malloc(sizeof(Matrixnode)*MAX_SIZE);
    double *vector1=(double*)bupt_malloc(sizeof(double)*N);
    double *vector2=(double*)bupt_malloc(sizeof(double)*N);
    double *temp=(double*)bupt_malloc(sizeof(double)*N);
    double *temp2=(double*)bupt_malloc(sizeof(double)*N);
    URL url=(URL)bupt_malloc(sizeof(URLnode)*N);
    for(int i=0;i<N;i++){
        vector1[i]=0;
        vector2[i]=0;
        temp[i]=0;
        temp2[i]=0;
    }

    int v;
    char buf[1000]="1";
    fgets(buf,1000,input);
    while(string_cmp("\n",buf,1)!=0){
        sscanf(buf,"%s %d",url[Global_stats.size].s,&v);
        Global_stats.size++;
        fgets(buf,1000,input);
    }
    printf("url done:%ld\n",Global_stats.size);

    int row,col,repeat;
    while(!feof(input)){
        if(fscanf(input,"%d %d",&col,&row)==2){
            matrix[Global_stats.num].col=col;
            matrix[Global_stats.num].row=row;
            matrix[Global_stats.num].value=1;
            Global_stats.num++;
        }
        else break;
    }
    printf("link done:%ld\n",Global_stats.num);

    for(int i=0;i<Global_stats.num;i++){
        temp[matrix[i].col]++;
        temp2[matrix[i].row]++;
    }
    for(int i=0;i<Global_stats.num;i++){
        matrix[i].value= matrix[i].value/temp[matrix[i].col]*(1-A);
    }
    for(int i=0;i<Global_stats.size;i++){
        vector1[i]=temp2[i]/(double)Global_stats.num;
    }
    printf("initiate done\n");

    double e=0.0001;
    int i=0;
    double temp3=0;
    for(int k=0;k<Global_stats.size;k++){
        temp3+=vector1[k];
    }
    temp3=temp3*A/(double)Global_stats.size;
    for(int i=0;i<Global_stats.num;i++){
        vector2[matrix[i].row]+=matrix[i].value*vector1[matrix[i].col];
    }
    for(int i=0;i<Global_stats.size;i++){
        vector2[i]+=temp3;
    }

    while(distance(vector2,vector1)>e*e){
        printf("repeat:%d\n",i++);
        double * t=vector1;
        vector1=vector2;
        vector2=t;
        temp3=0;
        for(int k=0;k<Global_stats.size;k++){
            temp3+=vector1[k];
            vector2[k]=0;
        }
        temp3=temp3*A/(double)Global_stats.size;
        for(int i=0;i<Global_stats.num;i++){
            vector2[matrix[i].row]+=matrix[i].value*vector1[matrix[i].col];
        }
        for(int i=0;i<Global_stats.size;i++){
            vector2[i]+=temp3;
        }
    }
    double num=0;
    for(int i=0;i<Global_stats.size;i++){
        num+=vector2[i];
    }
    for(int i=0;i<Global_stats.size;i++){
        url[i].value=vector2[i]/num;
    }
    printf("sort\n");
    QuickSort(url,0,Global_stats.size-1);
    for(int i=1;i<=10;i++){
        fprintf(output,"%s %lf\n",url[Global_stats.size-i].s,url[Global_stats.size-i].value);
    }
    fprintf(output,"%ld",Global_stats.mem/1024);
    free(matrix);
    free(vector1);
    free(vector2);
    free(temp);
    free(temp2);
    free(url);
    fclose(input);
    fclose(output);
    return 0;
}