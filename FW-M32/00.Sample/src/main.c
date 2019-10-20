
#define RCC_APB2ENR (*(volatile unsigned *)0x40021018)
#define GPIOB_CRH   (*(volatile unsigned *)0x40010C04)
#define GPIOB_BRR   (*(volatile unsigned *)0x40010C14)

int main(void)
{
	//APB2 Peripheral Clock Enable
	RCC_APB2ENR |= 0x8;

	//bit4-7 clear : GPB9 setting clear
	GPIOB_CRH &= 0xFFFFFF0F;
    
	//bit[5:4] : 01 => GPB9 Mode : Output Mode(10MHz)
	//bit[7:6] : 00 => GPB9 Configuration : General purpose output push-pull 
	GPIOB_CRH |= 0x10;

	GPIOB_BRR |= 0x200;     //Red Led On (GPB9)
	
	while(1)
	{
		;
	}
}

