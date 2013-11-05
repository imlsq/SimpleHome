#include"AT89X52.h"

#define  uint	unsigned int

sbit D1=P0^6;
sbit D2=P0^7;
sbit key=P3^3;

bit read_flag=0;
char temp; 
 

void send_char_com(unsigned char ch) 
{
	SBUF=ch;
	while(TI==0);
	TI=0;
}

void delay(unsigned int num)
{
 while( --num );
}

void main ()
{
	int i; 
	TMOD=0x20;
	TH1=0xFF;
	PCON=0x80;
	SCON=0x50;
	TR1=1;
	ES = 1;
	IE |= 0x90;
	while(1){
		if(key==0)
		{
			delay(300);
			if(key==0)
			{
				D1=0;
				while(key==0);
			}
		}
		else 
		{
			D1=1;
		}
	}
} 


void serial () interrupt 4 using 3 
{
	if(RI)
	{
			temp=SBUF;
			RI=0;
			send_char_com(temp);
	    
	}
}