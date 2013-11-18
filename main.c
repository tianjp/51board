#include <reg52.h>
#include <intrins.h> 

#define FOSC 11059200      //System frequency
#define BAUD 9600           //UART baudrate

bit busy;

sbit PIN0 = P2^0;           //up pin
sbit PIN1 = P2^1;           //down pin

void SendData(char dat);
void SendString(char *s);

void Uart_Init()
{
	SCON = 0x50;            //8-bit variable UART

	TMOD = 0x20;            //Set Timer1 as 8-bit auto reload mode
	TH1 = TL1 = -(FOSC/12/32/BAUD); //Set auto-reload vaule
	TR1 = 1;                //Timer1 start run
	ES = 1;                 //Enable UART interrupt
	EA = 1;                 //Open master interrupt switch
}

void Uart_Isr() interrupt 4 using 1
{
    if (TI)
    {
		TI = 0;             //Clear transmit interrupt flag
        busy = 0;           //Clear transmit busy flag
    }
}

void SendData(char dat)
{
    while (busy);           //Wait for the completion of the previous data is sent
    ACC = dat;              //Calculate the even parity bit P (PSW.0)
    busy = 1;
    SBUF = ACC;             //Send data to UART buffer
}

void SendString(char *s)
{
    while (*s)              //Check the end of the string
    {
        SendData(*s++);     //Send current char and increment string ptr
    }
}

void Delay1ms(unsigned int cnt)		//@11.0592MHz
{
	unsigned char i, j;

	unsigned int k;

	for (k=0; k<cnt; ++k)
	{
		_nop_();
		i = 2;
		j = 199;
		do
		{
			while (--j);
		} while (--i);
	}
}

void main()
{
	Uart_Init();
	SendString("Uart Test!\r\n");
	
	while (1)
	{
		if (!PIN0)
		{
			Delay1ms(100); 
			if (!PIN0)
			{
				SendString("up\r\n");
			}
		}
		else if (!PIN1)
		{
			Delay1ms(100); 
			if (!PIN1)
			{
				SendString("down\r\n");
			}
		}

		Delay1ms(10);
	}

}



