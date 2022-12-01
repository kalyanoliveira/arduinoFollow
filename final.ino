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

// Power out pin
#define power 2

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
void motor_spin_left(int speed);
void motor_spin_right(int speed);

// States
void follow();
void search();
void angry_action();

// State related varialbes
bool angry = false;

int search_count = 0; // Used by program
const int search_count_max = 3; // Normal counting integer
const int search_interval = 2; // Seconds
const int search_scope = 250; // Milliseconds
const int repetitions = (search_interval * 1000) / search_scope; // Used by program

const int distance = 30; // Centimeters
const int distance_min = 5; // Centimeters

const int brightness = 300; // Unitless number

const int follow_speed = 250; // Unitless number





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

    // Turn off motors
    motor_off();

    // Use digital pin as power output for other components
    pinMode(power, OUTPUT);
    digitalWrite(power, HIGH);
}





void loop()
{
    // If its bright
    if (measure_phr() >= brightness)
    {
        Serial.print("ANGRY!\n");
        
        // Get angry
        angry = true;

        // Try to follow, try to search and possibly follow, or get calm
        angry_action();
    }
    // If its dark, but we are angry
    else if (angry == true)
    {
        // Try to follow, try to search and possibly follow, or get calm
        angry_action();
    }
    // If its dark, and we are not angry, standby
    else
    {
        return;
    }
    
    return;
}





// Returns distance measurement (cm) of Ultrasonic Sensor
int measure_sonic()
{
    // Trigger Ultrasonic to start emitting
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    // Read the Echo pin from the Ultrasonic, convert to cm by / 58
    int duration = pulseIn(echo_pin, HIGH) / 58;

    Serial.print("Sonic: ");
    Serial.print(duration);
    Serial.print("\n");

    return duration;
}

// Returns measurement from left Infrared sensor
int measure_left_ir()
{
    // 0 means detection of obstacle
    int value = digitalRead(left_ir_pin);
    
    Serial.print("Left IR: ");
    Serial.print(value);
    Serial.print("\n");
    
    return value;
}

// Returns measurement from right Infrared sensor
int measure_right_ir()
{
    // 0 means detection of obstacle
    int value = digitalRead(right_ir_pin);
    
    Serial.print("Right IR: ");
    Serial.print(value);
    Serial.print("\n");
    
    return value;
}

// Returns measurement from Photoresistor
int measure_phr()
{
    int value = analogRead(photoresistor_pin);
    
    Serial.print("Photo: ");
    Serial.print(value);
    Serial.print("\n");
    
    return value;
}

// Motor functions
void motor_off()
{
    Serial.print("Motors off\n");
    digitalWrite(a_in1, LOW);
    digitalWrite(a_in2, LOW);
    digitalWrite(b_in3, LOW);
    digitalWrite(b_in4, LOW);
}
void motor_forwards(int speed)
{
    Serial.print("Motors forwards\n");
    analogWrite(a_en, speed);
    analogWrite(b_en, speed);
    digitalWrite(a_in1, LOW);
    digitalWrite(a_in2, HIGH);
    digitalWrite(b_in3, LOW);
    digitalWrite(b_in4, HIGH);
}
void motor_backwards(int speed)
{
    Serial.print("Motors backwards\n");
    analogWrite(a_en, speed);
    analogWrite(b_en, speed);
    digitalWrite(a_in1, HIGH);
    digitalWrite(a_in2, LOW);
    digitalWrite(b_in3, HIGH);
    digitalWrite(b_in4, LOW);
}
void motor_right(int speed)
{
    Serial.print("Motors right\n");
    analogWrite(a_en, speed/2);
    analogWrite(b_en, speed);
    digitalWrite(a_in1, LOW);
    digitalWrite(a_in2, HIGH);
    digitalWrite(b_in3, LOW);
    digitalWrite(b_in4, HIGH);
}
void motor_left(int speed)
{
    Serial.print("Motors left\n");
    analogWrite(a_en, speed);
    analogWrite(b_en, speed/2);
    digitalWrite(a_in1, LOW);
    digitalWrite(a_in2, HIGH);
    digitalWrite(b_in3, LOW);
    digitalWrite(b_in4, HIGH);
}
void motor_spin_left(int speed)
{
    Serial.print("Motors spin left\n");
    analogWrite(a_en, speed);
    analogWrite(b_en, speed);
    digitalWrite(a_in1, LOW);
    digitalWrite(a_in2, HIGH);
    digitalWrite(b_in3, HIGH);
    digitalWrite(b_in4, LOW);
}
void motor_spin_right(int speed)
{
    Serial.print("Motors spin right\n");
    analogWrite(a_en, speed);
    analogWrite(b_en, speed);
    digitalWrite(a_in1, HIGH);
    digitalWrite(a_in2, LOW);
    digitalWrite(b_in3, LOW);
    digitalWrite(b_in4, HIGH);
}





void follow()
{
    Serial.print("Following\n");

    search_count = 0;

    // If we are too close, turn off motors
    int measure = measure_sonic();
    if (measure <= distance_min && measure != 0)
    {
        motor_off();
        return;
    }

    // Else, let's decide a direction to follow
    // If both Infrareds output true, more forwards
    if (measure_left_ir() == 1 && measure_right_ir() == 1)
    {
        motor_forwards(follow_speed);
    }
    // If Left Infrared is true, and Right Infrared is false, move Left
    else if (measure_left_ir() == 1 && measure_right_ir() == 0)
    {
        motor_left(follow_speed);
    }
    // If Left Infrared is false, and Right Infrared is true, move Right
    else if (measure_left_ir() == 0 && measure_right_ir() == 1)
    {
        motor_right(follow_speed);
    }
    // If for some reason none of these happen, just move forward
    else
    {
        motor_forwards(follow_speed);
    }

    return;
}

void sweep()
{
    for (int i = 0; i < repetitions; i++)
    {
        delay(search_scope);

        int measure = measure_sonic();
        if (measure <= distance && measure != 0)
        {
            follow();
            return;
        }
    }
}
void search()
{
    Serial.print("Searching\n");

    // Every "search_scope" milliseconds we'll take a measurement from the Ultrasonic sensor.
    // For instance, if search_interval is 2, that means we'll search for 4 seconds (2 seconds whilst spinning left, and 2 seconds whilst spinning right)
    // If "search_scope" is 10 then, inside those 2 second intervals, we'll measure the Ultrasonic once every "10" milliseconds
    
    // Spin on its axis turning left whilst performing a swep search for "search_interval" seconds. If target found, follow and stop searching. Else, continue
    motor_spin_left(follow_speed/2);
    sweep();

    // Spin on its axis turning right whilst performing a swep search for "search_interval" seconds. If target found, follow and stop searching. Else, continue
    motor_spin_right(follow_speed/2);
    sweep();
    
    return;
}

void angry_action()
{
    // If there's a target nearby, follow it
    int measure = measure_sonic();
    if (measure <= distance && measure != 0)
    {
        follow();
        return;
    }
    
    // If no target found, search for it
    else if (search_count < search_count_max)
    {
        search_count++;
        
        Serial.print("Search N: ");
        Serial.print(search_count);
        Serial.print("\n");
        
        search();
        return;
    }
    
    // If number of searches has exceeded the max, reset
    else if (search_count >= search_count_max)
    {  
        Serial.print("kalm\n");
        
        // Get calm
        angry = false;

        // Turn off motors
        motor_off();

        // Reset searches
        search_count = 0;
        return;
    }
}
