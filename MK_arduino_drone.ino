String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

byte ibus_rx_buf[32] = {};
byte ibus_rx_data[32] = {};
int cnt = 0;
bool ibus_rx_cplt_flag = false;

int motorPin_1 = 3;
int motorPin_2 = 5;
int motorPin_3 = 6;
int motorPin_4 = 9;

typedef struct _FSiA6B_iBus
{
	unsigned short RH; //Right Horizontal
	unsigned short RV; //Right Vertical
	unsigned short LV; //Left Vertical
	unsigned short LH; //Left Horizontal
	unsigned short SwA;
	unsigned short SwB;
	unsigned short SwC;
	unsigned short SwD;
	unsigned short VrA;
	unsigned short VrB;

	unsigned char FailSafe;
}FSiA6B_iBus;

FSiA6B_iBus iBus;

void setup() {
  // initialize serial:
  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  Serial.println("Start program");
  pinMode(motorPin_1, OUTPUT);
  pinMode(motorPin_2, OUTPUT);
  pinMode(motorPin_3, OUTPUT);
  pinMode(motorPin_4, OUTPUT);

  delay(3000);

  analogWrite(motorPin_1, 200);
  analogWrite(motorPin_2, 200);
  analogWrite(motorPin_3, 200);
  analogWrite(motorPin_4, 200);

  Serial.println("Calibration MAX");
  delay(10000);

  analogWrite(motorPin_1, 50);
  analogWrite(motorPin_2, 50);
  analogWrite(motorPin_3, 50);
  analogWrite(motorPin_4, 50);

  Serial.println("Calibration MIN");
  delay(10000);
  Serial.println("Calibration complete");
}

void loop() {
  // print the string when a newline arrives:
  /*
  if (stringComplete) {
    Serial.println(inputString);
    inputString = "";
    stringComplete = false;
  }
  */
  if (ibus_rx_cplt_flag) {
    //Serial.print("!!!");
    //for(int i=0;i<32;i++) {
    //  Serial.print(ibus_rx_data[i], HEX);
    //}
    //Serial.println();

    iBus.RH = (ibus_rx_data[2] | ibus_rx_data[3]<<8) & 0x0fff;
    iBus.RV = (ibus_rx_data[4] | ibus_rx_data[5]<<8) & 0x0fff;
    iBus.LV = (ibus_rx_data[6] | ibus_rx_data[7]<<8) & 0x0fff;
    iBus.LH = (ibus_rx_data[8] | ibus_rx_data[9]<<8) & 0x0fff;
    iBus.SwA = (ibus_rx_data[10] | ibus_rx_data[11]<<8) & 0x0fff;
    iBus.SwC = (ibus_rx_data[12] | ibus_rx_data[13]<<8) & 0x0fff;

    //Serial.print(iBus.RH, DEC);
    //Serial.print('\t');
    //Serial.print(iBus.RV, DEC);
    //Serial.print('\t');
    Serial.print(iBus.LV, DEC);
    Serial.print('\t');
    //Serial.print(iBus.LH, DEC);
    //Serial.print('\t');
    //Serial.print(iBus.SwA, DEC);
    //Serial.print('\t');
    //Serial.print(iBus.SwC, DEC);
    Serial.print((iBus.LV-1000) / 4, DEC);
    Serial.print('\t');
    Serial.println();

    ibus_rx_cplt_flag = false;
  }

  analogWrite(motorPin_1, (iBus.LV-1000) / 4);
  analogWrite(motorPin_2, (iBus.LV-1000) / 4);
  analogWrite(motorPin_3, (iBus.LV-1000) / 4);
  analogWrite(motorPin_4, (iBus.LV-1000) / 4);
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    byte inChar = Serial.read();
    //Serial.print(inChar, HEX);
    // add it to the inputString:
    //inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    //if (inChar == '\n') {
    //  stringComplete = true;
    //}

    switch(cnt)
		{
      case 0:
        if(inChar == 0x20)
        {
          ibus_rx_buf[cnt] = inChar;
          cnt++;
        }
        break;
      case 1:
        if(inChar == 0x40)
        {
          ibus_rx_buf[cnt] = inChar;
          cnt++;
        }
        else
          cnt = 0;
        break;
      case 31:
        ibus_rx_buf[cnt] = inChar;
        cnt = 0;
        memcpy(ibus_rx_data, ibus_rx_buf, sizeof(ibus_rx_buf[0])*32);
        ibus_rx_cplt_flag = true;
        break;
      default:
        ibus_rx_buf[cnt] = inChar;
        cnt++;
        break;
		}

  }
}