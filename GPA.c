#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
int main(int argc, char *argv[])
{
	int num_subj;
	float *polyu;
	float *other;
	float polyu_gp,other_gp,sum_gp=0.0;
	char in_grade;
	int i;
	bool flag;
	num_subj=argc-1;
	polyu=(float*)malloc(sizeof(float)*num_subj);
	other=(float*)malloc(sizeof(float)*num_subj);
	int valid=0;
	printf("Polyu System:\n");
	for(i=1;i<=num_subj;i++){
		in_grade=argv[i][0];
		flag=true;
		if(strlen(argv[i])>2){polyu[i-1]=-1;other[i-1]=-1;continue;}
		switch(in_grade){
			case'A':polyu_gp=4.0;other_gp=11.0;break;
			case'B':polyu_gp=3.0;other_gp=8.0;break;
			case'C':polyu_gp=2.0;other_gp=5.0;break;
			case'D':polyu_gp=1.0;other_gp=2.0;break;
			case'F':polyu_gp=0.0;other_gp=0.0;flag=false;break;
			default:flag=false;
		}
		if(flag){
			if(argv[i][1]=='+'){polyu_gp+=0.3;other_gp+=1;}
			else if(argv[i][1]=='-'){
				if(in_grade=='D'){
					polyu_gp=-1;
				}
				else{polyu_gp-=0.3;}
				other_gp-=1;
			}
		}
		else if(in_grade!='F'){
			polyu_gp=-1;
			other_gp=-1;
		}
		polyu[i-1]=polyu_gp;other[i-1]=other_gp;
	}
	float sum=0.0;
	for(i=1;i<=num_subj;i++){
		if(polyu[i-1]==-1){
			printf("Grade for subject %d is %s,invalid\n",i,argv[i]);
		}
		else{
			valid++;
			printf("Grade for subject %d is %s,GP %2.1f\n",i,argv[i],polyu[i-1]);
			sum+=polyu[i-1];
		}
	}
	if(valid==0){printf("No valid subject.\n");}
	else{printf("Your GPA for %d valid subjects is %5.2f\n",valid,sum/valid);}
	valid=0;
	sum=0;
	printf("Other system:\n");
	for(i=1;i<=num_subj;i++){
		if(other[i-1]==-1){
			printf("Grade for subject %d is %s,invalid\n",i,argv[i]);
		}
		else{
			valid++;
			printf("Grade for subject %d is %s,GP %2.0f\n",i,argv[i],other[i-1]);
			sum+=other[i-1];
		}
	}
	if(valid==0){printf("No valid suject.\n");}
	else{printf("Your GPA for %d valid subjects is %5.2f\n",valid,sum/valid);}
}

