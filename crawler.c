//
//  main.c
//  crawler on the Internet with HTTP protocol
//
//  Created by 谢宇 on 2019/12/7.
//  Copyright © 2019 谢宇. All rights reserved.
//

#include "trie.h"

#define BUF_SIZE 2000000
#define URL_NUM 350000

void freeLink(LINK link){
    if(link!=NULL){
        freeLink(link->next);
        free(link);
    }
}

typedef struct l{
    char s[URL_LEN];
    struct l * next;
    int num;//
    int time;//resend when time <3
}element;

struct queue{
    element * front;
    element * rear;
    int size;
}URLqueue;

int client_sockfd;
struct sockaddr_in server_addr; //服务器端网络地址结构体
int socketbreak=0;
URL url;
RadixTree T;
FILE * outurl;

void queue_push(struct queue *q,char * a,int n,int time){
    element *t=(element*)malloc(sizeof(element));
    strcpy(t->s,a);
    t->next=NULL;
    t->num=n;
    t->time=time;
    q->size++;
    if(q->size==1){
        q->front=t;
        q->rear=t;
    }
    else{
        q->rear->next=t;
        q->rear=t;
    }
}

element queue_pop(struct queue *q){
    element * t1 = q->front;
    q->front = t1->next;
    q->size--;
    if(q->size==0){
        q->rear=NULL;
        q->front=NULL;
    }
    element x = *t1;
    free(t1);
    return x;
}

int queue_empty(struct queue q){
    if(q.size==0) return 1;
    else return 0;
}

int main(int argc, char *argv[]) {
    //initiate system
    time_t start,finish;
    double totaltime;
    time(&start);
    Global_stats.mem=0;
    Global_stats.num=0;
    T=(RadixTree)bupt_malloc(sizeof(RadixNode));
    T->nodeKind=0;
    for(int i=0;i<=M;i++){
        T->bh.ptr[i]=NULL;
    }
    T->bh.start=0;
    T->bh.num=0;
    if(argc!=3) exit(1);
    url=(URL)bupt_malloc(sizeof(URLstruct)*URL_NUM);
    outurl = fopen(argv[2],"w");
    URLqueue.front=NULL;
    URLqueue.rear=NULL;
    URLqueue.size=0;
    queue_push(&URLqueue,argv[1],0,1);
    strcpy(url[Global_stats.num].s,argv[1]);
    url[Global_stats.num].link=NULL;
    url[0].num=0;
    int temp1,temp2;
    T=insert(url[Global_stats.num].s,T,&url[Global_stats.num],-1,&temp1,&temp2);
    Global_stats.num++;
    //regex build
    char * pattern="<a[^>]* href=[\'\"][\r\n]*http://(news.sohu.com[^\'\"\r\n]*)[\r\n]*[\'\"][^>]*>";//match URL in label <a href="..."></a>
    int err;
    regex_t * regex;
    regmatch_t regmatch[2];
    regex = (regex_t*)bupt_malloc(sizeof(regex_t));
    err=regcomp(regex,pattern,REG_EXTENDED);
    //socket

    char buf[BUF_SIZE];  //数据传送的缓冲区
    memset(&server_addr,0,sizeof(server_addr)); //数据初始化--清零
    server_addr.sin_family=AF_INET; //设置为IP通信
    server_addr.sin_addr.s_addr=inet_addr("10.108.106.165");//实验服务器IP地址
    //server_addr.sin_addr.s_addr=inet_addr("10.109.16.218");//测试服务器IP地址
    server_addr.sin_port=htons(80); //服务器端口号

    /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/

    if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
    {
        perror("socket");
        return 1;
    }
    /*将套接字绑定到服务器的网络地址上*/
    if(connect(client_sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))<0)
    {
        perror("connect");
        return 1;
    }
    printf("connected to server\n");
    /*循环的发送接收信息并打印接收信息--recv返回接收到的字节数，send返回发送的字节数*/
    int repeat=1;
    while(queue_empty(URLqueue)!=1){
        //printf("%ld %d\n",Global_stats.num,URLqueue.size);
        //pthread_mutex_lock(&mutex2);
        element x=queue_pop(&URLqueue);
        //pthread_mutex_unlock(&mutex2);
        //printf("%s\n",x.s);
        sprintf(buf,"GET /%s HTTP/1.1\r\nHost: 10.109.16.53\r\nConnection: Keep-Alive\r\n"
                   "\r\n\r\n",x.s);
        //strcpy(buf,"GET / HTTP/1.1\r\nHost: 10.109.16.53\r\nConnection: keep-alive\r\n\r\n");
        int len,len2;
        len2=send(client_sockfd,buf,strlen(buf),0);
        len=recv(client_sockfd,buf,BUF_SIZE-1,0);
        if(Global_stats.num>=1000*repeat) {
            printf("%ld %d\n",Global_stats.num,URLqueue.size);
            repeat++;
        }
        if(len==0||len2<=0) {
            close(client_sockfd);
            client_sockfd=socket(PF_INET,SOCK_STREAM,0);
            //setsockopt(client_sockfd,IPPROTO_TCP,SO_KEEPALIVE,&live, sizeof(live));
            connect(client_sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
            printf("connect\n");
            queue_push(&URLqueue,x.s,x.num,x.time);
            continue;
        }
        //buf[len]='\0';
        int pos;
        for(pos=0;buf[pos]!='\r';pos++);
        buf[pos]=0;
        int t=string_cmp("HTTP/1.1 200 OK",buf,0);
        if(t!=0){
            if(x.time<2)queue_push(&URLqueue,x.s,x.num,x.time+1);
            continue;
        }
        //200 OK
        //
        buf[pos]='\r';
        char a=buf[len];
        buf[len]=0;
        char * p=strstr(buf,"Content-Length");
        if(p==NULL){
            queue_push(&URLqueue,x.s,x.num,x.time);
            continue;
        }
        p=p+16;
        int totalLen=0;
        while(*p!='\r'){
            totalLen*=10;
            totalLen+=*p-'0';
            p++;
        }
        p=strstr(buf,"\r\n\r\n");
        for(int i=0;&buf[i]!=p;i++){
            totalLen++;
        }
        totalLen+=4;
        buf[len]=a;
        int len1=len;
        while(len1<=totalLen){
            if(len1>=totalLen) break;
            len=recv(client_sockfd,&buf[len1],BUF_SIZE-1-len1,0);
            if(len<=0){
                queue_push(&URLqueue,x.s,x.num,x.time);
                break;
            }
            //printf("%d\n",len);
            len1+=len;
            //buf[len]='\0';
        }
        char error[1000];
        buf[totalLen]=0;
        //fprintf(test,"%s",buf);
        //err = regexec(regex,buf,2,regmatch,0);
        regerror(err, regex, error, sizeof(error));
        //printf("%s",error);
        char * temp=buf;
        err = regexec(regex,temp,2,regmatch,0);
        while(err==0){
            char l=temp[regmatch[1].rm_eo];
            temp[regmatch[1].rm_eo]=0;
            //printf("%s\n",&temp[regmatch[1].rm_so]);
            //check if url is repeated
            if(regmatch[1].rm_eo-regmatch[1].rm_so<URL_LEN){
                int flag=0;
                int node=0;
                //strcpy(url[Global_stats.num].s,x.s);
                int ans=search(T,&temp[regmatch[1].rm_so],strlen(&temp[regmatch[1].rm_so]),&node);
                if(ans==0){
                    strcpy(url[Global_stats.num].s,&temp[regmatch[1].rm_so]);
                    url[Global_stats.num].link=NULL;
                    url[Global_stats.num].num=Global_stats.num;
                    T=insert(&temp[regmatch[1].rm_so],T,&url[Global_stats.num],-1,&flag,&node);
                    queue_push(&URLqueue,&temp[regmatch[1].rm_so],Global_stats.num,1);
                    //fprintf(outurl,"%s %ld\n",url[Global_stats.num].s,Global_stats.num);
                    Global_stats.num++;
                }
                if(x.num>-1){
                    LINK link1=(LINK)bupt_malloc(sizeof(LINKnode));
                    link1->n=x.num;
                    if(ans!=0){
                        link1->next=url[node].link;
                        url[node].link=link1;
                    }
                    else{
                        link1->next=url[Global_stats.num-1].link;
                        url[Global_stats.num-1].link=link1;
                    }
                }
            }

            temp[regmatch[1].rm_eo]=l;
            temp=&temp[regmatch[1].rm_eo];
            err = regexec(regex,temp,2,regmatch,0);
        }
    }
    close(client_sockfd);//关闭套接字
    //output to file
    for(int i=0;i<Global_stats.num;i++){
        fprintf(outurl,"%s %d\n",url[i].s,i);
    }
    fprintf(outurl,"\n");
    for(int i=0;i<Global_stats.num;i++){
        LINK link1=url[i].link;
        while(link1!=NULL){
            fprintf(outurl,"%d %d\n",link1->n,i);
            link1=link1->next;
        }
    }
    //free memory
    regfree(regex);
    freeTree(T);
    for(int i=0;i<Global_stats.num;i++){
        freeLink(url[i].link);
    }
    free(url);
    time(&finish);
    totaltime=difftime(finish,start);
    fprintf(outurl,"%ldKB %lfs",Global_stats.mem/1024,totaltime);
    fclose(outurl);
    return 0;
}