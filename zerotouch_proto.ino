

/*
 * https://circuits4you.com
 * ESP32 LED Blink Example
 * Board ESP23 DEVKIT V1
 * 
 * ON Board LED GPIO 2
 */


#define STP_MTR_PWR 16

#define VLV_STP_MTR_P1  14
#define VLV_STP_MTR_P2  27
#define VLV_STP_MTR_P3  26
#define VLV_STP_MTR_P4  25

#define NZP_STP_MTR_P1  23
#define NZP_STP_MTR_P2  22
#define NZP_STP_MTR_P3  12
#define NZP_STP_MTR_P4  13

#define LID   2
#define SEAT  33
#define PUMP_VLV_SOLENOID 17
#define UVC   21
#define PUMP  32

#define PER_DTECT_SW    36
#define ADCPIN 39
#define FLUSH   15
#define RELAY   4


#define   STATE_IDLE      1
#define   STATE_URN        2
#define   STATE_LRN        3

//#define HIGH  1
//#define LOW   0

#define   CM_15     1.35
#define   CM_20     1.29
#define   CM_50     0.50
#define   CM_60     0.30

#define INTR_pin    1

#define KEY1        19
#define KEY2        18
#define KEY3        5
#define KEY4        34
#define KEY5        35

int usr_detect = 0;
int usr_seat_occupy = 0;
int state;
int wave_detect = 0;
int key_press = 0;
int step_count = 0;


// ************** funtion prototyping *******************//

//void delay_ms(unsigned int i);
//void delay_sec(unsigned int i);
void open_lid(void);
void close_lid(void);
void open_seat_cover(void);
void close_seat_cover(void);
void uvc_on(void);
void uvc_off(void);
void flush_off(void);
void flush_on(void);
void pump_off(void);
void pump_on(void);

void pump_vlv_open(void);
void pump_vlv_close(void);


int read_keyinput(int key);

void check_user(void);
void key_scan(void );


//char per_detect_sw;

void setup() {

  state = STATE_IDLE;
  // Set pin mode
 
  pinMode(STP_MTR_PWR,OUTPUT);
  digitalWrite(STP_MTR_PWR,LOW);delay(500);

 
  
  pinMode(VLV_STP_MTR_P1,OUTPUT);
  pinMode(VLV_STP_MTR_P2,OUTPUT);
  pinMode(VLV_STP_MTR_P3,OUTPUT);
  pinMode(VLV_STP_MTR_P4,OUTPUT);

  pinMode(NZP_STP_MTR_P1,OUTPUT);
  pinMode(NZP_STP_MTR_P2,OUTPUT);
  pinMode(NZP_STP_MTR_P3,OUTPUT);
  pinMode(NZP_STP_MTR_P4,OUTPUT);
  
  pinMode(LID,OUTPUT);
  digitalWrite(LID,LOW);
  pinMode(SEAT,OUTPUT);
  digitalWrite(SEAT,LOW);
  

  pinMode(PUMP_VLV_SOLENOID,OUTPUT);
  digitalWrite(PUMP_VLV_SOLENOID,LOW); 

  pinMode(UVC,OUTPUT);
  digitalWrite(UVC,LOW); 

  pinMode(PUMP,OUTPUT);
  digitalWrite(PUMP,LOW);

  pinMode(FLUSH,OUTPUT);
  digitalWrite(FLUSH,HIGH);

  pinMode(RELAY,OUTPUT);
  digitalWrite(RELAY,LOW);

  pinMode(PER_DTECT_SW,INPUT);

  pinMode(INTR_pin, INPUT);
  attachInterrupt ( digitalPinToInterrupt (INTR_pin), Ext_INT1_ISR, RISING);

  pinMode(KEY1,INPUT);
  pinMode(KEY2,INPUT);
  pinMode(KEY3,INPUT);
  pinMode(KEY4,INPUT);
  pinMode(KEY5,INPUT);
}


void  Ext_INT1_ISR() 
{
  wave_detect = 1;
}
void write_vlv_mtr(int a,int b,int c,int d){
digitalWrite(VLV_STP_MTR_P1, a);
digitalWrite(VLV_STP_MTR_P2, b);
digitalWrite(VLV_STP_MTR_P3, c);
digitalWrite(VLV_STP_MTR_P4, d);
}

void vlv_mtr_onestep_fwd(int del){
  write_vlv_mtr(1,0,0,0); delayMicroseconds(del);
  write_vlv_mtr(0,1,0,0); delayMicroseconds(del);
  write_vlv_mtr(0,0,1,0); delayMicroseconds(del);
  write_vlv_mtr(0,0,0,1); delayMicroseconds(del);
}

void vlv_mtr_onestep_rev(int del){
  write_vlv_mtr(0,0,1,1); delayMicroseconds(del);
  write_vlv_mtr(0,1,1,0); delayMicroseconds(del);
  write_vlv_mtr(1,1,0,0); delayMicroseconds(del);
  write_vlv_mtr(1,0,0,1); delayMicroseconds(del);
}

void write_nzp_mtr(int a,int b,int c,int d){
  digitalWrite(NZP_STP_MTR_P1, a);
  digitalWrite(NZP_STP_MTR_P2, b);
  digitalWrite(NZP_STP_MTR_P3, c);
  digitalWrite(NZP_STP_MTR_P4, d);
}

void nzp_mtr_onestep_fwd(int del){
  write_nzp_mtr(1,0,0,1); delayMicroseconds(del);
  write_nzp_mtr(1,1,0,0); delayMicroseconds(del);
  write_nzp_mtr(0,1,1,0); delayMicroseconds(del);
  write_nzp_mtr(0,0,1,1); delayMicroseconds(del);
}

void nzp_mtr_onestep_rev(int del){
  write_nzp_mtr(0,0,1,1); delayMicroseconds(del);
  write_nzp_mtr(0,1,1,0); delayMicroseconds(del);
  write_nzp_mtr(1,1,0,0); delayMicroseconds(del);
  write_nzp_mtr(1,0,0,1); delayMicroseconds(del);
}





void loop(){

check_user();
key_scan();


if ( state == STATE_URN && usr_detect ==1 )
{
  key_press = 0;
  wave_detect = 1;
 
  }

/////////////////////////////////////////////////////////////////////////////////////////////
//************************NEW CODE ******************************************************** //
/////////////////////////////////////////////////////////////////////////////////////////////
 
switch(state)
  {
    case STATE_IDLE:  if ( usr_detect)
          {
            
            open_lid();             
            delay(20);
            open_seat_cover();
            
            delay(2000);
            state = STATE_URN;
          }
         break;
    
   case  STATE_URN: if ( !usr_detect)
          {
            flush_on();
            delay(1000);
            flush_off();     
            uvc_on();
            delay(1000);
            uvc_off();                   
            close_seat_cover();
            delay(3000);
            close_lid(); 
            state = STATE_IDLE;
          }
          else if( usr_seat_occupy )
          {
             state = STATE_LRN;
             
          }
    break;
    
    case  STATE_LRN:  if (!usr_detect)
        {
              uvc_on();
              delay(3000);
              uvc_off();
              delay(3000);
              close_seat_cover();
              delay(3000);
              close_lid(); 
              delay(3000);
      //        detachInterrupt(INTR_pin);      
              state = STATE_IDLE;
        }
        else {
        
            if (key_press == 1)
            {
              for (step_count= 0; step_count < 510; step_count++)
              {
                nzp_mtr_onestep_rev(2500);
                }
                
              pump_vlv_open();
              pump_on();              
              delay(1000);
            }
            if (key_press == 5)
            {
              pump_off();
              delay(1000);
              pump_vlv_close();
              delay(1000);
              for (step_count= 0; step_count < 510; step_count++)
              {
                nzp_mtr_onestep_fwd(2500);
                }
              }              
            }
  break; 
  } 

  key_press =0;
 
}

//**************************Functions **********************************************//
void open_lid(void)       { digitalWrite(LID,HIGH);  }
void close_lid(void)      { digitalWrite(LID,LOW);   }
void open_seat_cover(void){ digitalWrite(SEAT,HIGH); }
void close_seat_cover(void){ digitalWrite(SEAT,LOW); }
void flush_on (void)      { digitalWrite(FLUSH,HIGH);}
void flush_off(void)      { digitalWrite(FLUSH,LOW); }
void uvc_on(void)         { digitalWrite(UVC,HIGH);  }
void uvc_off(void)        { digitalWrite(UVC,LOW);   }
void pump_vlv_open(void)  { digitalWrite(PUMP_VLV_SOLENOID,HIGH); }
void pump_vlv_close(void) { digitalWrite(PUMP_VLV_SOLENOID,LOW);  }
void pump_on(void)        { digitalWrite(PUMP,HIGH); }
void pump_off(void)       { digitalWrite(PUMP,LOW);  }
void relay_on(void)       { digitalWrite(PUMP,LOW);}
void relay_off(void)       { digitalWrite(PUMP,HIGH);}

void check_user(void)
{
  int   adcValue, i;
  float voltValue;
  static int confrm_count = 0;
  
  voltValue = 0;
  
 for (i = 0;  i < 5; i++)
 {
  adcValue = analogRead(ADCPIN);
  voltValue += adcValue;
  delayMicroseconds(65000);  
 }
  voltValue /= 5;
  voltValue *= 3;
  voltValue /= 4096;
 

  
  if ( voltValue > CM_15 ){
    
      if(usr_detect)
            confrm_count++;
        if( confrm_count > 3)
        {   
          usr_seat_occupy = 1;
        }
     // usr_detect      = 1;
      
  }
  else if ( voltValue > CM_50   &&  voltValue < CM_20){
      if(!usr_detect)
            confrm_count++;
        if( confrm_count > 3)
        {    
          usr_detect = 1;
          usr_seat_occupy = 0;
          confrm_count = 0;
        }
  }
  else if ( voltValue  < CM_60 ){
        if(usr_detect)
            confrm_count++;
        if( confrm_count > 3)
        {   
          usr_detect = 0;
          usr_seat_occupy = 0;
          confrm_count = 0;
        }
  }

}



int read_keyinput(int key){
  
    char temp_key1=0, temp_key2=0, temp_key3=0;
    char i;

    for (i = 0; i< 4; i++){
          temp_key1 = digitalRead(key);
          temp_key3 = temp_key2;
          temp_key2 = temp_key1;
          delay(30);
        }

    if (temp_key1 & temp_key2 & temp_key3 ){
        return (1); 
    }
    else
    return (0);
}

void key_scan(void ){

  int i = 0;
  if (read_keyinput(KEY1))
      i |= 0x01;
  if (read_keyinput(KEY2))
      i |= 0x02;
  if (read_keyinput(KEY3))
      i |= 0x04;
  if (read_keyinput(KEY4))
      i |= 0x08;
  if (read_keyinput(KEY5))
      i |= 0x10;
      
 }
void scan_inputs(void ){
 float user_dist = 0; 

  switch(state)
  {
    case STATE_IDLE:  
          check_user();
    break;
    
    case STATE_URN:
          check_user();
    break;

    case STATE_LRN:
          check_user();
          // scan keys
  }
}
