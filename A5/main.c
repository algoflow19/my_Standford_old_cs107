#include<stdio.h>

int main(){
	char mat[5];
	mat[0]='1';
	*(int*)(mat)=4;
	printf("%ul \n",mat);
	printf("%ul \n",mat+1);

	printf ("%d \n No bus error",*(int*)(mat));
	return 0;
}	
