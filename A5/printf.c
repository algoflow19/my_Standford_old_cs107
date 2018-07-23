
#include<stdio.h>


int cout(int a,int b,int c,int d,int e){
	printf("%d \n",a);
	printf("%lu is the param a addr\n",&a);
	printf("%d \n",b);
	printf("%lu is the param b addr\n",&b);
	printf("%d \n",c);
	printf("%lu is the param c addr\n",&c);
	printf("%lu is the param d addr\n",&d);
	printf("%lu is the param e addr\n",&e);
	printf("%d is the param e addr\n",e);
	e=1000;
	return 0;
}


