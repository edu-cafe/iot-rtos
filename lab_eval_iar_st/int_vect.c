#include <stdio.h>
#define	NO1	10
#define	NO2	20
#define	NO3	30
void f1(int id);

void AAA (void) {f1(NO1);}
void BBB (void) {f1(NO2);}
void CCC (void) {f1(NO3);}

void f1(int id)
{
	printf("id:%d\n", id);
}

int main(void)
{
AAA();
CCC();
return 0;
}
