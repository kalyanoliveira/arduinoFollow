// Ultrasonic Distance Sensor digital pins
#define trig_pin 7
#define echo_pin 8

// Left Infrared Sensor digital pin
#define left_ir_pin 12

// Right Infrared Sensor digital pin
#define right_ir_pin 4

// Photoresistor analog pin
#define photoresistor_pin 0

// Motor pins
// OUT1 = Positive, OUT2 = Negative
// OUT3 = Positive, OUT4 = Negative
// Motor A is right
// Motor B is left
#define a_en 3
#define a_in1 5
#define a_in2 6
#define b_en 11
#define b_in3 9
#define b_in4 10

// LED pins
#define led_pin 13

// Returns distance measurement (cm) of Ultrasonic Sensor
int measure_sonic();

// Returns measurement from left Infrared sensor
int measure_left_ir();

// Returns measurement from right Infrared sensor
int measure_right_ir();

// Returns measurement from Photoresistor
int measure_phr();

// Motor functions
void motor_off();
void motor_forwards(int speed);
void motor_backwards(int speed);
void motor_right(int speed);
void motor_left(int speed);

// States
void follow();
void search();
void angry_action();

// State related varialbes
bool angry = false;
int search_count = 0;
const int search_count_max = 4;
const int distance = 30;
const int brightness = 100;

void setup()
{
    // Set up serial communication
    Serial.begin(9600);

    // Set pin modes for Ultrasonic
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, INPUT);

    // Set pin modes for Infrared Sensors
    pinMode(left_ir_pin, INPUT);
    pinMode(right_ir_pin, INPUT);

    // Set pin modes for L298N
    pinMode(a_en, OUTPUT);
    pinMode(a_in1, OUTPUT);
    pinMode(a_in2, OUTPUT);
    pinMode(b_en, OUTPUT);
    pinMode(b_in3, OUTPUT);
    pinMode(b_in4, OUTPUT);

    // Pin mode for LEDs
    pinMode(led_pin, OUTPUT);

    // Turn off motors
    motor_off();

    // Turn off LEDS
    digitalWrite(13, LOW);

}

void loop()
{
    
    // If its bright
    if (measure_phr() >= brightness)
    {
        // Get angry
        angry = true;

        // Turn red on LEDs
        digitalWrite(led_pin, HIGH);

        // Try to follow, try to search and follow, or get calm
        angry_action();
    }
    // If its dark, but we are angry
    else if (angry == true)
    {
        // Try to follow, try to search and follow, or get calm
        angry_action();
    }
    // If its dark, and we are not angry, standby
    else
    {
        return;
    }
}

// Returns distance measurement (cm) of Ultrasonic Sensor
int measure_sonic()
{
    // Trigger Ultrasonic to start emitting
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    // Read the Echo pin from the Ultrasonic, convert to cm by /58
    int duration = pulseIn(echo_pin, HIGH)/58;

    return duration;
}

// Returns measurement from left Infrared sensor
int measure_left_ir()
{
    // 0 means detection of obstacle
    return digitalRead(left_ir_pin);
}

// Returns measurement from right Infrared sensor
int measure_right_ir()
{
    // 0 means detection of obstacle
    return digitalRead(right_ir_pin);
}

// Returns measurement from Photoresistor
int measure_phr()
{
    return analogRead(photoresistor_pin);
}

// Motor functions
void motor_off()
{
    digitalWrite(a_in1, LOW);
    digitalWrite(a_in2, LOW);
    digitalWrite(b_in3, LOW);
    digitalWrite(b_in4, LOW);
}
void motor_forwards(int speed)
{
    analogWrite(a_en, speed);
    analogWrite(b_en, speed);
    digitalWrite(a_in1, HIGH);
    digitalWrite(a_in2, LOW);
    digitalWrite(b_in3, HIGH);
    digitalWrite(b_in4, LOW);
}
void motor_backwards(int speed)
{
    analogWrite(a_en, speed);
    analogWrite(b_en, speed);
    digitalWrite(a_in1, LOW);
    digitalWrite(a_in2, HIGH);
    digitalWrite(b_in3, LOW);
    digitalWrite(b_in4, HIGH);
}
void motor_right(int speed)
{
    analogWrite(a_en, speed/2);
    analogWrite(b_en, speed);
    digitalWrite(a_in1, HIGH);
    digitalWrite(a_in2, LOW);
    digitalWrite(b_in3, HIGH);
    digitalWrite(b_in4, LOW);
}
void motor_left(int speed)
{
    analogWrite(a_en, speed);
    analogWrite(b_en, speed/2);
    digitalWrite(a_in1, HIGH);
    digitalWrite(a_in2, LOW);
    digitalWrite(b_in3, HIGH);
    digitalWrite(b_in4, LOW);
}

void follow()
{
    // If both Infrareds output true, more forwards
    if (1 == 1 && 1 == 1)
    {
        motor_forwards(255);
    }
    // If Left Infrared is true, and Right Infrared is false, move Left
    else if (1 == 1 && 1 == 0)
    {
        motor_left(255);
    }
    // If Left Infrared is false, and Right Infrared is true, move Right
    else if (1 == 0 && 1 == 1)
    {
        motor_right(255);
    }
    // If for some reason none of these happen, just move forward
    else
    {
        motor_forwards(255);
    }
}

void search()
{
    motor_right(120);
    delay(1000);
    motor_off();

    if (measure_sonic() <= distance)
    {
        follow();
        return;
    }

    motor_left(120);
    delay(2000);
    motor_off();
    
    if (measure_sonic() <= distance)
    {
        follow();
        return;
    }

    motor_right(120);
    delay(1000);
    motor_off();

    if (measure_sonic() <= distance)
    {
        follow();
        return;
    }

    return;
}

void angry_action()
{
    // If there's a target nearby, follow it
    if (measure_sonic() <= distance)
    {
        follow();
        return;
    }
    // If no target found, search for it
    else if (search_count < search_count_max)
    {
        search_count++;
        search();
        return;
    }
    // If number of searches has exceeded 4, reset
    else if (search_count >= search_count_max)
    {  
        // Get calm
        angry = false;

        // Turn off red LEDs
        digitalWrite(led_pin, HIGH);

        // Turn off motors
        motor_off();

        // Reset searches
        search_count = 0;
        return;
    }
}
