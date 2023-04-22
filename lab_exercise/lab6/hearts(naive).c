#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<stdbool.h>
#include <string.h>
struct Poker{
    char content[3];
    int suit;
    int value;
    struct Poker *next;
};

int get_value(char);
void swap(struct Poker*,int,int);
struct Poker* get_lowest(struct Poker**,int,char [],int);
void delete(struct Poker**,char []);

int get_value(char c){
    switch(c){
        case 'A':return 14;
        case '2':return 2;
        case '3':return 3;
        case '4':return 4;
        case '5':return 5;
        case '6':return 6;
        case '7':return 7;
        case '8':return 8;
        case '9':return 9;
        case 'T':return 10;
        case 'J':return 11;
        case 'Q':return 12;
        case 'K':return 13;
        case 'S':return 0;
        case 'H':return 1;
        case 'C':return 2;
        case 'D':return 3;
    }
}

void swap(struct Poker *array,int a, int b){
    struct Poker temp=array[a];
    array[a]=array[b];
    array[b]=temp;
}

struct Poker* get_lowest(struct Poker **head,int suit,char last_sign[],int max_value){
    struct Poker *second=(struct Poker*)malloc(sizeof(struct Poker));
    struct Poker *tail=(struct Poker*)malloc(sizeof(struct Poker));
    second=*head;
    tail=second->next;
    struct Poker *result=(struct Poker*)malloc(sizeof(struct Poker));
    strcpy(result->content,"");
    if(strcmp(last_sign,"lead")==0){
        second=*head;
        int val=14;
        if(second->next==NULL){
            return result;
        }
        second=second->next;
        while(second!=NULL){
            if(second->value<=val){
                strcpy(result->content,second->content);
                result->value=second->value;
                result->suit=second->suit;
                val=second->value;
            }
            second=second->next;
        }
        return result;
    }
    if(suit>=0){
        while(second!=NULL){
            // printf("\nresult is %s suit is %d %d\n",result->content,suit,max_value);
            if(second->suit==suit){
                strcpy(result->content,second->content);
                result->value=second->value;
                result->suit=second->suit;
            }
            second=second->next;
        }
        if(strcmp(result->content,"")!=0){
            return result;
        }
    }
//discard case
    second=*head;
    //if SQ is meet discard it first.
    while(second!=NULL){
        if(second->suit==0&&second->value==12){
            strcpy(result->content,second->content);
            result->value=second->value;
            result->suit=second->suit;
            return result;
        }
        if(second->suit>0){
            break;
        }
        second=second->next;
    }
    //discard the lowest heart;
    second=*head;
    while(second!=NULL){
        if(second->suit==1){
            strcpy(result->content,second->content);
            result->value=second->value;
            result->suit=second->suit;
            return result;
        }
        second=second->next;
    }
    if(strcmp(result->content,"")!=0){
        return result;
    }
    second=*head;
    second=second->next;
    //just discard the first card
    strcpy(result->content,second->content);
    result->value=second->value;
    result->suit=second->suit;
    return result;
}

void delete(struct Poker **head,char mark[]){
    struct Poker *second=(struct Poker*)malloc(sizeof(struct Poker));
    struct Poker *tail=(struct Poker*)malloc(sizeof(struct Poker));
    second=*head;
    tail=second->next;
    if(tail==NULL){
        return;//it is already empty
    }
    while(tail!=NULL){
        if(strcmp(tail->content,mark)==0){
            second->next=tail->next;
            return;
        }
        second=tail;
        tail=tail->next;
    }
    return;
}
int main(int argc,char *argv[]){
    int i=0;
    int j=0;
    int k;
    int n;
    int pid;
    int num_card;
    int p2c[4][2];//1 for write, 0 for read
    int c2p[4][2];
    char buf[300];
    char cards[52][3];
    char put[3];
    pid_t child_pid[4];
    // printf("%d\n",argc);
    fflush(stdout);
    for(i=0;i<4;i++){
        if(pipe(&p2c[i][0])<0){
            printf("pipe creation error\n");
            exit(1);
        }
        if(pipe(&c2p[i][0])<0){
            printf("pipe creation faile\n");
            exit(1);
        }
    }
    //this is the code for input
    while((n = read(STDIN_FILENO, buf, 300)) > 0) {
        i = 0, k = 0;
        buf[n] = '\0';
        while(i < n && buf[i] == ' ') i++;
        while (i < n) {
            cards[k][2] = '\0';
            cards[k][0] = buf[i++];
            cards[k][1] = buf[i++];
            k++;
            while(i < n && (buf[i] == ' ' || buf[i] == '\n')) i++;
        }
    }

    for(i=0;i<4;i++){
        pid=fork();
        if(pid<0){
            printf("fork failed");
            exit(1);
        }
        else if(pid==0){
            //a linked list with a null head(sensor);
            struct Poker *head=(struct Poker*)malloc(sizeof(struct Poker));
            head->suit=-1;
            head->value=-1;
            head->next=NULL;
            close(c2p[i][0]);
            close(p2c[i][1]);
            int n;
            char c_send[80];
            char c_receive[80];
            struct Poker deck[13];
            int n_suit;
            char init_deck[80]="";
            printf("Child %d pid %d: received ",i+1,getpid());
            for(k=0;k<13;k++){
                read(p2c[i][0],c_receive,80);
                strcpy(deck[k].content,c_receive);
                printf("%s ",deck[k].content);
                deck[k].suit=get_value(deck[k].content[0]);
                deck[k].value=get_value(deck[k].content[1]);
            }
            printf("\n");
            //arrange
            for(k=1;k<13;k++){
                char s[3];
                strcpy(s,deck[k].content);
                int key=deck[k].suit;
                int keyb=deck[k].value;
                for(j=k-1;j>=0;j--){
                    if(key>deck[j].suit){
                        break;
                    }
                    else if(key==deck[j].suit&&deck[j].value>keyb){
                        break;
                    }
                    strcpy(deck[j+1].content,deck[j].content);
                    deck[j+1].suit=deck[j].suit;
                    deck[j+1].value=deck[j].value;
                }
                strcpy(deck[j+1].content,s);
                deck[j+1].suit=key;
                deck[j+1].value=keyb;
                // printf("value deck=%d\n",deck[j+1].value);
            }
            struct Poker *temp=(struct Poker*)malloc(sizeof(struct Poker));
            temp=head;
            printf("Child %d pid %d: arranged ",i+1,getpid());
            for(k=0;k<13;k++){
                temp->next=&deck[k];
                printf("%s ",(temp->next)->content);
                temp=temp->next;
            }
            printf("\n");
            struct Poker *played_card=(struct Poker*)malloc(sizeof(struct Poker));
            while(head->next!=NULL){
                sleep(1);
                n=read(p2c[i][0],c_receive,80);
                c_receive[--n]='\0';
                if(strcmp(c_receive,"l")==0){//if it is the first one to play the cards
                    played_card=get_lowest(&head,-1,"lead",0);
                    write(c2p[i][1],played_card->content,80);
                    printf("Child %d pid %d: play %s\n",i+1,getpid(),played_card->content);
                    delete(&head,played_card->content);
                    // printf("\nthe suit is %d\n",n_suit);
                }
                else{
                    n_suit=get_value(c_receive[0]);
                    played_card=get_lowest(&head,n_suit,"",0);
                    write(c2p[i][1],played_card->content,80);
                    printf("Child %d pid %d: play %s\n",i+1,getpid(),played_card->content);
                    delete(&head,played_card->content);
                }
            }
            close(c2p[i][1]);
            close(p2c[i][0]);
            exit(0);
        }
        else {
            child_pid[i]=pid;
            close(c2p[i][1]);
            close(p2c[i][0]);
        }
    }
    if(pid>0){
        char send[80];
        char receive[80];
        printf("Parent pid %d: child players are ",getpid());
        for(i=0;i<4;i++){
            printf("%d ", child_pid[i]);
        }
        printf("\n");
        //deliver the card to each child
        num_card=0;
        for(k=0;k<13;k++){
            for(i=0;i<4;i++){
                write(p2c[i][1],cards[num_card++],80);
            }
        }
        int lead=0;
        int current_player;
        int round=1;
        int c_suit;
        int winner=lead;
        int n;
        int max_value;
        sleep(1);
        int mark[4];
        while(1){
            winner=lead;
            int current_player[4];
            printf("parent pid %d:round %d child %d to lead\n",getpid(),round,lead+1);
            strcpy(send,"l");
            char suit_card[3];
            current_player[0]=lead;
            int index=0;
            index=1;
            int current_mark=0;
            for(k=0;k<4;k++){
                if(k==0){
                    write(p2c[lead][1],send,80);
                    // sleep(1);
                    n=read(c2p[lead][0],receive,80);
                    strcpy(send,receive);
                    receive[--n]='\0';
                    c_suit=get_value(receive[0]);
                    if(c_suit==1){
                        current_mark++;
                    }
                    if(c_suit==0&&get_value(receive[1])==12){
                        current_mark+=13;
                    }
                    max_value=get_value(receive[1]);
                    strcpy(suit_card,receive);
                    printf("Parent pid %d:round %d Child %d plays %s\n",getpid(),round,lead+1,receive);
                }
                else{
                    //send format 2 for card, 1 for final, 1 for max_value
                    strcpy(send,suit_card);
                    if(k==3){
                        char K[2];
                        K[0]=max_value+'0';
                        strcat(send,"f");//the final child
                        strcat(send,K);
                    }
                    write(p2c[(lead+k)%4][1],send,80);
                    // sleep(1);
                    n=read(c2p[(lead+k)%4][0],receive,80);
                    receive[--n]='\0';
                    int my_suit=get_value(receive[0]);
                    if(my_suit==1){
                        current_mark+=1;
                    }
                    if(my_suit==0&&get_value(receive[1])==12){
                        current_mark+=13;
                    }
                    printf("Parent pid %d:round %d Child %d plays %s\n",getpid(),round,(lead+k)%4+1,receive);
                    if(c_suit==my_suit){
                        if(get_value(receive[1])>max_value){
                            max_value=get_value(receive[1]);
                            winner=(lead+k)%4;
                        }
                    }
                }
            }
            printf("round %d Parent pid %d:Child %d wins the trick\n",round,getpid(),winner+1);
            mark[winner]+=current_mark;
            lead=winner;
            if(round==13){
                break;
            }
            round++;
        }
        for(k=0;k<4;k++){
            if(mark[k]==26){
                mark[k]=0;
                for(n=0;n<4;n++){
                    if(n!=k){
                        mark[n]=26;
                    }
                }
                break;
            }
        }
        printf("Parent pid %d: score = <",getpid());
        for(k=0;k<4;k++){
            printf("%d ",mark[k]);
        }
        printf(">\n");
    }
    for(j=0;j<4;j++){
        waitpid(child_pid[j], NULL, 0);
    }
    exit(0);
}
