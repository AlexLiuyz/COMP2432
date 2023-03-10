#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>
int generate(int pattern[], int num, float timespan)
{
    int i;
    float u, r;
    for (i = 0; i < num; i++) {
        u = (float)rand() / RAND_MAX;
        r = -log(u)*timespan;
        pattern[i] = (int)r + 1;
    }
    return 0;
}

int main(int argc,char *argv[])
{
    int i,*servicePattern,*arrivalPattern,num,num_c,p,num_task;
    if((char)argv[1][0]=='D'){
        num=(argc-3)/2;
        servicePattern=(int*)malloc(sizeof(int)*num);
        arrivalPattern=(int*)malloc(sizeof(int)*num);
        int index=0;
        //get the input from the user for debug mode
        for(i=2;i<num+2;i++){
            arrivalPattern[index++]=atoi(argv[i]);
        }
        index=0;
        for(i=num+3;i<argc;i++){
            servicePattern[index++]=atoi(argv[i]);
        }
        printf("Parent pid %d: debug mode\n",getpid());
        num_c=1;
    }
    if((char)argv[1][0]=='S'){
        num_c=atoi(argv[2]);
        num_task=(argc-3)/4;
        printf("Parent, pid %d: %d Child,%d tasks,simulation mode\n",getpid(),num_c,num_task);
        if(num_task<num_c){
            num_c=num_task;
        }
    }
    for(p=0;p<num_c;p++){
        //start the child process
        int c_id=fork();
        if(c_id<0){
            printf("Fork faild\n");
            exit(1);
        }
        else if(c_id==0){
            int j=1;//count the number of customer
            int queue=0;//count the length of the queue
            int id=1;//count the customer have been served
            bool flag=true;//the flag is represent if there is customer at the counter
            int state=0;//for the customer who wasn't come after another need to go to state 0
            float sum_queue=0;
            int max=0;//stand for maximum queue length
            float total=0.0;//stands for total waiting time
            float average_wait=0.0,average_queue=0.0;
            int current=0;
            bool pri=false;//whether print
            if((char)argv[1][0]=='D'){
                printf("Child %d,pid %d: %d customers arriving at ",p+1,getpid(),num);
                int t=0;
                for(i=0;i<num;i++){
                    t+=arrivalPattern[i];
                    printf("%d ",t);
                    arrivalPattern[i]=t;
                }
                printf("\n");
                printf("Child %d,pid %d: %d customers requiring service for ",p+1,getpid(),num);
                for(i=0;i<num;i++){
                    printf("%d ",servicePattern[i]);
                }
                printf("\n");
                int *t_finish=(int*)malloc(sizeof(int)*(num+1));//time for the serving to start,not use the first position
                for(i=0;i<num;i++){
                    t_finish[i]=0;
                }
                int *result=(int*)malloc(sizeof(int)*4);//the first position is for arriving,the second and third is for waits, the fourth is for arriving departs
                for(i=0;i<4;i++){
                    result[i]=-1;
                }
                current=0;//count as the clock
                for(current=0;id<=num;current++){
                    if(current==arrivalPattern[j-1]){
                        pri=true;
                        result[0]=j;
                        j++;
                        if(state==1){
                            queue++;
                        }
                        flag=true;

                    }
                    if(state==0&&flag){
                        pri=true;
                        t_finish[id]=current+servicePattern[id-1];
                        state=1;
                        result[1]=id;
                        result[2]=current-arrivalPattern[id-1];
                    }
                    if(t_finish[id]==current){
                        pri=true;
                        result[3]=id;
                        id++;
                        if(id<=num){
                            if(current>=arrivalPattern[id-1]){
                                pri=true;
                                result[1]=id;
                                result[2]=current-arrivalPattern[id-1];
                                total+=(current-arrivalPattern[id-1]);
                                t_finish[id]=current+servicePattern[id-1];
                            }
                            else{
                                state=0;
                                flag=false;
                            }
                        }
                        if(queue>0){
                            queue--;
                        }
                    }
                    sum_queue+=queue;
                    if(queue>max){
                        max=queue;
                    }
                    if(pri){
                            printf("Child %d,pid %d: time %d ",p+1,getpid(),current);
                        if(result[3]!=-1){
                                printf("customer %d departs,",result[3]);
                        }
                        if(result[0]!=-1){
                            printf(" customer %d arrives,",result[0]);
                        }
                        if(result[1]!=-1&&result[2]!=-1){
                            printf("customer %d waits for %d,",result[1],result[2]);
                        }
                        printf("queue length %d\n",queue);
                    }
                    int o=0;
                    for(o=0;o<4;o++){
                        result[o]=-1;
                    }
                    pri=false;
                }
                average_wait=total/num;
                if(current-1==0){
                    average_queue=0;
                }
                else{
                    average_queue=sum_queue/(current-1);
                }
                printf("Child %d, pid %d: all customers served at time %d \n",p+1,getpid(),current-1);
                printf("Child %d, pid %d: maximum queue length %d \n",p+1,getpid(),max);
                printf("Child %d, pid %d: average queue length %.3f\n",p+1,getpid(),average_queue);
                printf("Child %d, pid %d: total waiting time %.0f\n",p+1,getpid(),total);
                printf("Child %d, pid %d: average waiting time %.3f\n",p+1,getpid(),average_wait);
                printf("Child %d, pid %d: Child 1 completed execution\n",p+1,getpid());
                exit(0);
            }
            else if((char)argv[1][0]=='S'){
                int simulate=0;
                for(simulate=3+4*p;simulate<argc;simulate+=num_c*4){
                    j=1;
                    queue=0;
                    id=1;
                    flag=true;
                    state=0;
                    sum_queue=0;
                    max=0;
                    total=0.0;
                    average_wait=0.0;
                    average_queue=0.0;
                    current=0;
                    printf("Child %d, pid %d: simulating %d customers, seed %d, arrival %.1f, service %.1f\n",p+1,getpid(),atoi(argv[simulate]),atoi(argv[simulate+1]),atof(argv[simulate+2]),atof(argv[simulate+3]));
                    int seed=atoi(argv[simulate+1]);
                    srand(seed);
                    int size=atoi(argv[simulate]);
                    arrivalPattern=(int*)malloc(sizeof(int)*size);
                    servicePattern=(int*)malloc(sizeof(int)*size);
                    generate(arrivalPattern,size,atof(argv[simulate+2]));
                    generate(servicePattern,size,atof(argv[simulate+3]));
                    arrivalPattern[0]=0;
                    int t=0;
                    for(i=0;i<size;i++){
                        t+=arrivalPattern[i];
                        arrivalPattern[i]=t;
                    }
                    int *t_finish=(int*)malloc(sizeof(int)*(size+1));//time for the next serving to start,not use the first position
                    for(i=0;i<size+1;i++){
                        t_finish[i]=0;
                    }
                    current=0;//count as the clock
                    for(current=0;id<=size;current++){
                        if(current==arrivalPattern[j-1]){
                            j++;
                            if(state==1){//if someone is beinh served, then the coming one would be stay in queue
                                queue++;
                            }
                            flag=true;
                        }
                        if(state==0&&flag){
                            t_finish[id]=current+servicePattern[id-1];
                            state=1;
                        }
                        else if(t_finish[id]==current){
                            id++;
                            if(id<=size){
                                if(current>=arrivalPattern[id-1]){
                                    total+=(current-arrivalPattern[id-1]);
                                    t_finish[id]=current+servicePattern[id-1];
                                }
                                else{
                                    state=0;
                                    flag=false;
                                }
                            }
                            if(queue>0){
                                queue--;
                            }
                        }
                        sum_queue+=queue;
                        if(queue>max){
                            max=queue;
                        }
                    }
                    average_wait=total/size;
                    if(current==0){
                        average_queue=0;
                    }
                    else{
                        average_queue=sum_queue/(current-1);
                    }
                    printf("Child %d, pid %d: all customers served at time %d \n",p+1,getpid(),current-1);
                    printf("Child %d, pid %d: maximum queue length %d \n",p+1,getpid(),max);
                    printf("Child %d, pid %d: average queue length %.3f\n",p+1,getpid(),average_queue);
                    printf("Child %d, pid %d: total waiting time %.0f\n",p+1,getpid(),total);
                    printf("Child %d, pid %d: average waiting time %.3f\n",p+1,getpid(),average_wait);
                }
                printf("Child %d, pid %d: Child %d completed execution\n",p+1,getpid(),p+1);
                exit(0);
            }
        }
    }
    for(i=0;i<num_c;i++){
        // int state;
        // wait(&state);
        wait(NULL);
        printf("Parent, pid %d: Child %d completed execution\n",getpid(),i+1);
        // printf("%d\n",state);
    }
    if(i==num_c){
        exit(0);
    }
}
