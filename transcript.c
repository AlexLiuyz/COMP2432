#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<stdbool.h>
#include<string.h>
float GETgpa(char *grade){
    float result;
    switch(grade[0]){
        case'A':result=4.0;break;
        case'B':result=3.0;break;
        case'C':result=2.0;break;
        case'D':result=1.0;break;
        case'F':result=0.0;break;
        default:return -1;
    }
    if(grade[1]=='+'&&grade[0]!='F'){
        result+=0.3;
    }
    else if(grade[1]=='-'&&grade[0]!='D'&&grade[0]!='F'){
        result-=0.3;
    }
    return result;
}
int main(int argc,char *argv[]){
    FILE *infilep,*datafile;
    char mark[10]="student";
    int i,j,num_file,k,num_subj;
    char *data=(char*)malloc(sizeof(char)*80);
    char *data_grade=(char*)malloc(sizeof(char)*2);
    char *id=(char*)malloc(sizeof(char)*80);
    char *name=(char*)malloc(sizeof(char)*80);
    bool flag=false;
    bool update=true;
    num_file=-1;
    for(i=0;i<argc;i++){
        if(strcmp(argv[i],mark)==0){
            break;
        }
        num_file++;
    }
    char dummy[10];
    char *current_course=(char*)malloc(sizeof(char)*20);
    char *current_year=(char*)malloc(sizeof(char)*5);
    char *current_sem=(char*)malloc(sizeof(char)*2);
    char *current_grade=(char*)malloc(sizeof(char)*2);
    bool check=false;
    for(i=0;i<argc;i++){
        if(flag){
            num_subj=0;
            infilep=fopen("student.dat","r");
            j=0;
            char course[num_file][15];
            char sem[num_file][2];
            char year[num_file][5];
            char grade[num_file][20];
            while(fscanf(infilep,"%s %s",id,name)!=EOF){
                if(strcmp(id,argv[i])==0||strcmp(name,argv[i])==0){
                    printf("transcript for %s %s\n",id,name);
                    int index=0;
                    for(j=1;j<=num_file;j++){
                        datafile=fopen(argv[j],"r");
                        fscanf(datafile,"%s %s %s %s",dummy,current_course,current_year,current_sem);//the mark here is
                        while(fscanf(datafile,"%s",data)!=EOF){
                            if(check){
                                memset(current_grade, 0, 20);
                                strncpy(current_grade,data,2);//get the grade of the student
                                break;
                            }
                            if(strcmp(data,id)==0){
                                check=true;//the student can be find in file
                            }
                        }
                        if(current_grade[0]>'Z'||current_grade[0]<'A'){
                            current_grade[0]='\0';
                            current_grade[1]='\0';
                        }
                        if(check){
                            printf("%s %s sem %s %s\n",current_course,current_year,current_sem,current_grade);
                        }
                        update=true;
                        if(check&&strlen(current_grade)!=0){
                            for(k=0;k<num_file;k++){
                                if(strcmp(course[k],current_course)==0){
                                    update=false;
                                    if(strcmp(year[k],current_year)<0){
                                        memset(year[k], 0, 20);
                                        memset(sem[k], 0, 20);
                                        strncpy(year[k],current_year,5);
                                        strncpy(sem[k],current_sem,2);
                                        memset(grade[k],0,20);
                                        strncpy(grade[k],current_grade,2);
                                    }
                                    else if(strcmp(year[k],current_year)==0){
                                        update=false;
                                        if(strcmp(sem[k],current_sem)<0){
                                            memset(sem[k], 0, 20);
                                            strncpy(sem[k],current_sem,2);
                                            memset(grade[k],0,20);
                                            strncpy(grade[k],current_grade,2);
                                        }
                                    }
                                }
                            }
                        }
                        if(check&&update){
                            memset(course[index], 0, 20);
                            memset(year[index], 0, 20);
                            memset(sem[index], 0, 20);
                            memset(grade[index],0,20);
                            strncpy(course[index],current_course,15);
                            strncpy(year[index],current_year,5);
                            strncpy(sem[index],current_sem,2);
                            strncpy(grade[index],current_grade,2);
                            index++;
                        }
                        check=false;
                    }
                    float sum=0.0;
                    for(j=0;j<num_file;j++){
                        // printf("course is %s\n",course[j]);
                        // printf("grade is %s\n",grade[j]);
                        if(GETgpa(grade[j])>=0){
                            num_subj++;
                            sum+=GETgpa(grade[j]);
                        }
                    }
                    close(datafile);
                    if(num_subj==0){
                        printf("Haven't got any valid grades.\n");
                    }
                    else{
                        printf("GPA for %d subjects %.2f\n",num_subj,sum/num_subj);
                    }

                }
            }
            fclose(infilep);
        }
        if(strcmp(argv[i],mark)==0){
            flag=true;
        }
    }
    return 0;
}
