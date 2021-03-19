#include <SPI.h>

#include <RFM98W_library.h>
//RFMLib radio =RFMLib(20,7,16,21);
//#define nss 20

//RFMLib radio =RFMLib(10,14,16,15);

#define nss 10
#define DIO0 2
#define DIO5 8
#define RST  9
#define IRQ0 0 // pin 2
#define IRQ1 1 //pin 3

RFMLib radio =RFMLib(nss,DIO0,DIO5,RST);

int printMsg = 0;
int nummsg = 0;
byte pinIrq=0;

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

  Serial.print("\nSetup Tx..."); 
  Serial.print("\nver RFM98w: ");  
  Serial.print(radio.getVersion(),HEX);  
  
  byte my_config[6] = {0x44,0x84,0x88,0xAC,0xCD, 0x08};
  radio.configure(my_config);
  Serial.print("\nSetup Tx...");   

 readAllRegs();
 
}

void loop()
{

  if(printMsg < 10)
  {
    Serial.print("\nradio.rfm_status = ");
    Serial.print(radio.rfm_status, HEX);
    Serial.print("\npin irq = ");
    Serial.print(pinIrq, HEX);
    Serial.print("\nloop "); Serial.print(printMsg++,DEC);  
  }

  nummsg++;
  
  if(radio.rfm_status ==0)
  {
    RFMLib::Packet p;
    p.data[0]='H';
    p.data[1]='E';
    p.data[2]='L';
    p.data[3]='L';
    p.data[4]='O';
    p.data[5]='-';
    p.data[6]='R';
    p.data[7]='F';
    p.data[8]='M';
    p.data[9]='9';
    p.data[10]='8';
    p.data[11]=' ';
    p.data[12]='N';
    p.data[13]=nummsg;
    p.len = 14;
    radio.beginTX(p); 
    attachInterrupt(/*DIO0*/IRQ0,RFMISR,RISING);
   Serial.print("\n ref40 = "); Serial.print(brRFM(0x40),HEX);
  }

  //pinIrq = radio.checkInterrupt();
  
  if(radio.rfm_done)
  {
        Serial.print("\nTX: Ending msg ");  
        Serial.print(nummsg,DEC); 
        radio.endTX();       
  } 

  delay(5000);
}

void RFMISR()
{
    Serial.print("\ninterrupt");
    radio.rfm_done = true; 
}


