
#define RCC_APB2ENR (*(volatile unsigned *)0x40021018)
#define GPIOB_CRL   (*(volatile unsigned *)0x40010C00)
#define GPIOB_CRH   (*(volatile unsigned *)0x40010C04)
#define GPIOB_BRR   (*(volatile unsigned *)0x40010C14)
#define GPIOB_BSRR  (*(volatile unsigned *)0x40010C10)

void delay(int cnt)
{
    volatile int i, j;
    for(i=0; i<cnt; cnt++) {
        for(j=0; i<500000; i++) {
            j = j;  j = j;
        }
    }
}

int main(void)
{
	//APB2 Peripheral Clock Enable
	RCC_APB2ENR |= 0x8;

    //bit20-23 clear : GPB5 setting clear
	GPIOB_CRL &= 0xFF0FFFFF;
	//bit0-7 clear : GPB8,GPB9 setting clear
	GPIOB_CRH &= 0xFFFFFF00;

    //bit[21:20] : 01 => GPB5 Mode : Output Mode(10MHz)
	//bit[23:22] : 00 => GPB5 Configuration : General purpose output push-pull 
	//GPIOB_CRL |= 0x100000;
	GPIOB_CRL |= (0x1 << 20);
    
	GPIOB_CRH |= 0x11;
	
	while(1)
	{
        ..............
	}
}
