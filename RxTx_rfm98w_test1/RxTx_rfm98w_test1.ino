#include <SPI.h>

#include <RFM98W_library.h>
//RFMLib radio =RFMLib(20,7,16,21);
//#define nss 20

#define nss 10
#define DIO0 2
#define DIO5 8
#define RST  9
#define IRQ0 0 // pin 2
#define IRQ1 1 //pin 3

RFMLib radio =RFMLib(nss,DIO0,DIO5,RST);
//RFMLib radio =RFMLib(10,7,8,9);

bool bRx;
bool bIrqRx;
bool bIrqTx;

int printMsg = 0;
RFMLib::Packet rx;
RFMLib::Packet tx;

void getModeRm(unsigned char reg)
{
  Serial.print("\nmode rm is: "); Serial.print(((reg>>5)&1)==1?" OOK":" FSK"); Serial.print((reg&0x80)==0x80?" LoRa":" FSK/OOK");
  Serial.print("\nTransceiver modes:\n");
  switch(reg&0x7)
  {
    case 0:  Serial.print("000 Sleep mode"); break;
    case 1:  Serial.print("001 Stdby mode"); break;
    case 2:  Serial.print("010 FS mode TX (FSTx)");break;
    case 3:  Serial.print("011 Transmitter mode (Tx)");break;
    case 4:  Serial.print("100 FS mode RX (FSRx)");break;
    case 5:  Serial.print("101 Receiver mode (Rx)");break;
    case 6:  Serial.print("110 reserved");break;
    case 7:  Serial.print("111 reserved");break;
  };

   Serial.print((reg&0x8)==0x8?"\n'1' Low Frequency Mode (access to LF test registers)":"\n'0' High Frequency Mode (access to HF test registers)");
}

void readAllRegs()
{
   byte i = 1;
   while(i<=0x3f)
   {
    Serial.print("\nReg addr  = "); Serial.print(i,HEX); Serial.print("->");
    byte regData = brRFM(i);
    Serial.print(regData,HEX);
  
    if(i==1) 
    {
       getModeRm(regData);
    }
    
    i++;
  }  
}

byte brRFM(byte ad)
{ //single byte read
    digitalWrite(nss,LOW);
    SPI.transfer(ad & B01111111);//wrn bit low
    byte val = SPI.transfer(0);//read, but we still have to spec a value?
    digitalWrite(nss,HIGH);
    return val;
}

void bwRFM(byte ad, byte val)
{//single byte write
    digitalWrite(nss,LOW);
    SPI.transfer(ad | 128);//set wrn bit - WRITE = 1
    SPI.transfer(val);
    digitalWrite(nss,HIGH);
}

void setup(){
  
  SPI.begin();
  Serial.begin(38400);

  Serial.print("\nSetup Rx...");  
  Serial.print("\nver RFM98w: ");  
  Serial.print(radio.getVersion(),HEX);
  
  byte my_config[6] = {0x44,0x84,0x88,0xAC,0xCD, 0x08};
  radio.configure(my_config);
  attachInterrupt(/*DIO0*/IRQ0,RFMISR,RISING);

  delay(1000);//1 sek

  readAllRegs();
}

void loop()
{
  if(radio.rfm_status == 0 &&  bRx == false)
  {
    radio.beginRX(); 
   // attachInterrupt(/*DIO0*/IRQ0,RFMISR,RISING);
  }

  if(radio.rfm_done)
  {
        if(radio.rfm_status == 2 && bIrqRx)
        {
          bIrqRx = false;
          Serial.print("\nRX: Ending");   
          //RFMLib::Packet rx;
          radio.endRX(rx);
          Serial.print("\nReceived: ");
          for(byte i = 0;i<rx.len;i++)
          {Serial.print(rx.data[i],HEX);Serial.print(" ");}
         // Serial.println();  
          bRx = true;
        }
  }

  if(bRx)
  {   
          delay(1000);
          bRx = false;
          //answer
          //RFMLib::Packet p;

          for(byte i = 0;i<rx.len;i++)
          { tx.data[i] = rx.data[rx.len-1-i];}
      
          tx.len = rx.len;
          radio.beginTX(tx);   

          Serial.print("\nAnswer ["); Serial.print(tx.len,DEC);Serial.print("]");
          for(byte i = 0;i<tx.len;i++)
          {Serial.print(tx.data[i],HEX);Serial.print(",");}
  }

  if(radio.rfm_status == 1 && radio.rfm_done && bIrqTx)
  {
        bIrqTx = false;
        Serial.print("\nTX: Ending msg ");  
        radio.endTX();       
  } 
}

void RFMISR()
{
   Serial.print("\ninterrupt is:  ");

   //to register irq:
   if(radio.pullInterruptRx()) { Serial.print("irq_RX"); bIrqRx= true;}
   if(radio.pullInterruptTx()) { Serial.print("irq_TX"); bIrqTx= true;} 
   
   radio.rfm_done = true; 
}



