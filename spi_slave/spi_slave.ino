#include <SPI.h>
volatile boolean process_it;                    //Flag for checking if the data is recieved from Master i.e. ESP8266
byte a;                                         //Byte to store the processed data
byte b;  
byte buf [100];
volatile byte pos;
void setup (void)
{
    //Serial.begin (115200);                        //Set UART baug rate to 115200
    Serial.begin(38400);
    Serial.print("\nSETUP SPI SLAVE 2\n");

    for(byte i =0 ; i<100;i++)
    {
      buf[i] = 0xff;
    }
    
    SPCR |= bit (SPE);                            //Configure ATMEGA328P/Arduino in slave mode
    pinMode(MISO, OUTPUT);                        //Configure MISO as output, SlaveOut
    process_it = false;                           //Initialize flag to FALSE
    SPI.attachInterrupt();                        //Enable interrupts for SPI--> You can do that like this too /*SPCR |= bit (SPIE)*/
}  
 
// SPI interrupt routine
ISR (SPI_STC_vect) //Вектор прерывания в WinAVR - SPI_STC_vect, где ISR - обработчик прерывания
{
    //byte c = SPI_SlaveReceive();
    byte c  = SPDR;           //Grab the data byte from the SPI Data Register (SPDR) //The SPI control register
    a = c>>4  & 0x0f;                             //Put the byte into a temporary variable for processing
    b = (c<<4)& 0xf0;
    SPDR = a|b;                                 //process the data byte and put it back into the SPDR for the Master to read it
    
    if(pos < sizeof buf)
    {
       buf[pos++] = c;
    }

    process_it = true;                            //Set the Flag  as TRUE

   // SPI_MasterTransmit(a|b);
}  

//функция приема байта
byte SPI_SlaveReceive()
{
  while(!(SPSR & (1<<SPIF)));//ждем если занят
  return SPDR;//возвращаем байт данных
}

//функция передачи байта
void SPI_MasterTransmit(byte cData)
{
    SPDR = cData;//начинаем передачу
    while(!(SPSR & (1<<SPIF)));//ждем пока передача завершится
}
 
void loop (void)
{
    if (process_it)                               //Check if the data has been processed
    {
        Serial.print("\nRecieved N=");//UART - Notify if recived a byte from master
        Serial.print(pos,DEC); Serial.print(": ");
       //buf [pos] = 0;  
       
        for(byte i = 0; i < pos; i++)
        {
          Serial.print(buf[i],HEX);
        }
     
        pos = 0;
        process_it = false;                         //Set the Flag to False
    
    }
}
