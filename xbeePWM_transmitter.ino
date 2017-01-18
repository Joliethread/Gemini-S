#define xBee Serial
#define CH_NUMB 9

/*******************************************************************************
@ xBee PWM transmitter R1
@ 01.12.2016

# 27.12.2016, Tested with Receiver, working!

*/

/***************** PIN DEFINITION **********************************************
[OUTPUT]
SERIAL_8N1  6   (PD6)

[INPUT]
TILT    A3  (analog)
YAW     A0  (analog)
MODE1   A1  (PC1)
MODE2   A2  (PC2)
SWITCH  3   (PD3)
FOTO    8   (PB0)
REC     2   (PD2)
FLIR11  7   (PD7)
FLIR12  13  (PB5)
FLIR21  9   (PB1)
FLIR22  10  (PB2)
FLIR31  11  (PB3)
FLIR32  12  (PB4)
*/

#define TILT   A3
#define YAW    A0

#define MODE1  A1
#define MODE2  A2
#define SWITCH 3
#define FOTO   8
#define REC    2
#define FLIR11 7
#define FLIR12 13
#define FLIR21 9
#define FLIR22 10
#define FLIR31 11
#define FLIR32 12

/***************** GLOBAL VARIABLE ********************************************/

unsigned int pwm_tx[CH_NUMB];

unsigned int analog_tilt, analog_yaw;
unsigned int val_old;


unsigned int yaw_filter(unsigned int val, int stepp){
    unsigned int x = val % stepp;

    if(x <= stepp/2){
        val = val - x;
    }else if(x > stepp/2){
        val = val + (stepp - x);
    }

    return val;
}

unsigned int tilt_filter(unsigned int val, unsigned int stepp){
    unsigned int x = val % stepp;

    if(x <= stepp/2){
        val = val - x;
    }else if(x > stepp/2){
        val = val + (stepp - x);
    }

    if(val > (val_old + stepp) || val < (val_old - stepp)){
        val_old = val;
        return val;
    }else{
        return val_old;
    }
}

void update_state(){
    //TILT
    analog_tilt = analogRead(TILT);
    pwm_tx[0] = map(analog_tilt, 0, 1023, 1000, 2000);
    pwm_tx[0] = tilt_filter(pwm_tx[0], 14);
    if( pwm_tx[0] < 1100 )
        pwm_tx[0] = 1000;

    //YAW
    analog_yaw = analogRead(YAW);
    pwm_tx[1] = map(analog_yaw, 0, 1023, 1000, 2000);
    pwm_tx[1] = yaw_filter(pwm_tx[1], 20);
    if( (pwm_tx[1] > 1400) && (pwm_tx[1] < 1550) )
        pwm_tx[1] = 1432;


    //MODE
    if(bit_is_clear(PINC, PC1)){
        pwm_tx[2] = 2000;
    }else if(bit_is_clear(PINC, PC2)){
        pwm_tx[2] = 1000;
    }else{
        pwm_tx[2] = 1500;
    }

    //SWITCH
    if(bit_is_clear(PIND, PD3)){
        pwm_tx[3] = 2000;
    }else{
        pwm_tx[3] = 1000;
    }

    //FOTO
    if(bit_is_clear(PINB, PB0)){
        pwm_tx[4] = 2000;
    }else{
        pwm_tx[4] = 1000;
    }

    //REC
    if(bit_is_clear(PIND, PD2)){
        pwm_tx[5] = 2000;
    }else{
        pwm_tx[5] = 1000;
    }

    //FLIR 1
    if(bit_is_clear(PINB, PB5)){
        pwm_tx[6] = 2000;
    }else if(bit_is_clear(PIND, PD7)){
        pwm_tx[6] = 1000;
    }else{
        pwm_tx[6] = 1500;
    }

    //FLIR 2
    if(bit_is_clear(PINB, PB2)){
        pwm_tx[7] = 2000;
    }else if(bit_is_clear(PINB, PB1)){
        pwm_tx[7] = 1000;
    }else{
        pwm_tx[7] = 1500;
    }

    //FLIR 3
    if(bit_is_clear(PINB, PB4)){
        pwm_tx[8] = 2000;
    }else if(bit_is_clear(PINB, PB3)){
        pwm_tx[8] = 1000;
    }else{
        pwm_tx[8] = 1500;
    }
}

void push_state(){
    for(int i; i < CH_NUMB; i++){
        xBee.write('a' + i);
        xBee.print(pwm_tx[i]);
    }
}

void setup() {
    xBee.begin(57600);

    pinMode(MODE1, INPUT_PULLUP);
    pinMode(MODE2, INPUT_PULLUP);
    pinMode(SWITCH, INPUT_PULLUP);
    pinMode(FOTO, INPUT_PULLUP);
    pinMode(REC, INPUT_PULLUP);
    pinMode(FLIR11, INPUT_PULLUP);
    pinMode(FLIR12, INPUT_PULLUP);
    pinMode(FLIR21, INPUT_PULLUP);
    pinMode(FLIR22, INPUT_PULLUP);
    pinMode(FLIR31, INPUT_PULLUP);
    pinMode(FLIR32, INPUT_PULLUP);
}

void loop() {
    update_state();
    push_state();
}
