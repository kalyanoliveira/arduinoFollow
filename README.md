# arduinoFollow
So, here’s a lengthy explanation of the code located in final.ino

The easiest way to understand it is by first taking a look at the loop() function of the code, in line 108. We start with an “if” statement, which calls for the function measure_phr(), and then compares it with the variable brightness (which was defined in line 67). 

What measure_phr() does, as is shown in line 185, is to read the analog output of the photoresistor pin (which according to line 12 is the analog pin 0 in the Arduino Uno) and then it gives that value back to us for the comparison. 
So essentially measure_phr() just measures the level of brightness in the photoresistor, and if that brightness is larger than our arbitrary value of the brightness variable (namely 300), we execute the code inside the first “if” block.
The code for that first “if” block is quite simple: we set the “angry” variable, defined in line 56, to true, and then we execute the angry_action() function.

Now, if that first “if” block fails to be true – that is, if the level of brightness that we measured in the photoresistor is less than 300 – then we execute the following “else-if” block (on line 122), which quite simply asks us if the variable “angry” is set to true. If it is, then we execute the “angry_action()” function.

If both the first “if” block and the second “else-if” block fail, then we execute the third and final “else” block (on line 128), which essentially tells the robot to “do nothing”, symbolized by the “return” keyword.

So we can see that if the surrounding environment is bright enough, the robot will get “angry” and perform some actions defined in “angry_action()”. If the surrounding environment is not bright enough, but for some reason – likely due to previous iterations of loop() – the robot is “angry”, we still execute the actions defined in “angry_action()”. If the surrounding environment is not bright, and the robot is not “angry”, then we simply tell it to idle – to do nothing.

-----

This is all that can be said for the loop() function. Now we move on to the “angry_action()” function (located in line 342), since that function can potentially be executed by the loop() function multiple times. 

We start by measuring the ultrasonic sensor via the measure_sonic() function, and then we save that measurement into the “measure” variable.

The implementation for the measure_sonic() function can be found on line 141, and it’s a standard implementation across Arduino projects. We trigger the trig pin of the ultrasonic sensor by setting it to HIGH, wait for 10 microseconds, and set it to LOW. Then we receive the signal of the echo pin of the ultrasonic sensor using the pulseIn function, which automatically converts the received electrical current into a numerical value. We then divide that value by 58, hence converting it into centimeters, and save it into the “duration” variable. We then give back to the program the value stored in that “duration” variable. Also notice how both the trig_pin and echo_pin were defined in lines 2 and 3.

Once we have the measurement from the ultrasonic sensor, we use it to check some conditions in the “if” statement block of line 346. If those conditions are satisfied, then we execute the follow() function.

Specifically, we look for a distance measurement from the ultrasonic that is less than or equal to a “distance” variable (which was defined in line 64) AND that is not equal to 0. We ran into some problems with the actual hardware of the ultrasonic sensor; sometimes it was outputting 0 when it shouldn’t. So, checking if the measurement is not equal to 0 is making sure we don’t get any errors, and by extension any unwanted behavior.

Now, if the conditions of the “if” block on line 346 are not satisfied, then we move on to the “else-if” block of line 353, which asks the program if the “search_count” variable (defined in line 58) is less than the “search_count_max” (defined in line 59).

If that’s true, then we increase the search_count variable by one, symbolized by line 355, and execute the “search()” function.

If the conditions for this “else-if” block are not met, though, then we simply move on to the final “else-if” block of line 366. Here, we ask if the search_count variable is higher than or equal to the search_count_max variable. 
If that’s true (and, if the previous Boolean statements were evaluated to be false, it has to be true), then we set the angry variable to false, turn off the motors by executing the motor_off() function (more on that later), and reset the search_count variable by setting it to 0.

So, we can see here that the angry_action() function is designed in a clever way that it allows our robot to “search”, namely by some actions defined in the “search()” function, given the fact that it does not have anything to “follow” via the “follow()” function – that is, given the fact that there’s nothing close enough to the robot for it to “follow”, which is determined by the measurement of the ultrasonic sensor and the comparisons made in the “if” block of line 346. It will not search forever, though; every time it searches for something, that “search_count” variable increases. Once it reaches a maximum value, which we specific via the “search_count_max variable”, it gives up on searching, and bounces back to the “calm” state.

---

Next, we discuss the search() function (line 323), because it is called upon angry_action() several times. I have already included a lengthy explanation of it via some comments in the code, so I’ll just re-iterate it here: every “search_scope” milliseconds, a number variable defined in line 61, we’ll take a measurement from the ultrasonic sensor. For instance, if search_interval (defined in line 60) is 2, that means we’ll search for 4 seconds: 2 seconds whilst the robot is spinning on its axis to the left, and 2 seconds whilst the robot is spinning on its axis to the right. If “search_scope” is 10 the, inside those 2 second intervals, we’ll measure the Ultrasonic sensor every “10” milliseconds. If in any of those measurements of every “10” milliseconds we find something that is close enough to the Ultrasonic sensor, then we call upon the follow() function.

So, as you can see, the search() function first makes the robot spin on its axis to the left by using the function “motor_spin_left(follow_speed/2)” (more on that later). Then, it calls the “sweep()” function, which performs the actions I described in the previous paragraph. Then, it spins on its axis to the right using the “motor_spin_right(follow_speed/2)” function (once again, more on that later), and then calls upon “sweep()” again.

“sweep()” is defined just above “search()”, in line 309, and makes use of a for loop to repeatedly measure the ultrasonic sensor while the robot is spinning on its axis. Essentially, the code inside the “for” block of line 311 is executed “repetitions” times. The “repetitions” variable is defined on line 62, and it automatically calculates the number of intervals of “search_scope” milliseconds that fit inside “search_interval” seconds. Also notice that we use the same structure of “angry_action()” to call upon the “follow()” function: we save the measurement of the ultrasonic sensor into a “measure” variable, and then we compare that “measure” number to the “distance” variable, alongside the error checking of “not-equal-to-zero”.

---

All that is left now is to explain the “follow()” function and the assortment of motor functions. 

“follow()”, defined in line 270, makes usage of “if” statements, the aforementioned “measure_sonic()”, and the complementary “measure_left_ir()” and “measure_right_ir()” functions to call upon motor functions. 

These latter two are similar to other “measure something functions”; their definitions on lines 159 and 171 read the output from the “right_ir_pin” and “left_ir_pin” pins via digitalRead(), and it gives us back this value. Notice how the returned value can only be 0 or 1, since the infrared sensor can be either detecting something or not detecting something. Also see that “right_ir_pin” and “left_ir_pin” were defined in the beginning of the program, in lines 6 and 9.

The first action in “follow()” is to set the “search_count” variable back to 0, therefore resetting the “search” behavior of the robot in case the object gets far enough away from the ultrasonic sensor whilst the robot is following.

Following, we check if the object in front of the ultrasonic sensor is too close to it by the “if” block of line 278. In a similar fashion to what was already explained, we save the distance measurement of the ultrasonic sensor into a “measure” variable. We still make sure that that value is not equal to 0, but this time we also ask the program if that value is less than “distance_min”, a variable defined in line 65. If that is true, that means that whatever object the robot is trying to follow is too close to the ultrasonic sensor, so we turn off the motors by calling the “motor_off()” function.

Then, use 4 “if else-if else” blocks to evaluate the measurements from the dual infrared sensors. If both sensors measure 1, which symbolizes “true, there is an object in front of the infrared sensor”, then by the “if” block of line 286 we call the “motor_forwards(follow_speed)” function. If the left IR is outputting 1, and the right one is outputting false, then we call the “motor_left(follow_speed)” function. If the opposite happens, a “true-state” right IR and a “false-state” left IR, then we call the “motor_right(follow_speed)” function. If none of these scenarios happen, then we simply call the “motor_forwards(follow_speed)” function.

---

The motor functions are contained within lines 197 to 255 and are all pretty similar and Arduino standard practices. We essentially just use a bunch of digitalWrite()’s and the pins defined in lines 19-24 to send out HIGH/LOW signals to the H-Bridge. 

For instance, in line 205 we have the motor_forwards() function. It works by setting HIGH and LOW pairs between the pins “a_in1” and “a_in2”, and “b_in3” and “b_in4”. Alternating which member of each pair is set to HIGH or LOW changes the direction in which the motors are spinning. For instance, setting “a_in1” pin to LOW and “a_in2” pin to HIGH spins the “a motor” forwards; by extension, setting the “a_in1” pin to HIGH and “a_in2” pin to LOW spins the “a motor” backwards.
If one wishes to turn off the motors, like is shown in “motor_off()” of line 197, all that needs to be done is set the pins “a_in1”, “a_in2”, “b_in3”, and “b_in4” to LOW.

There’s one extra detail worth noting, though; all of these functions, besides motor_off(), take as input a number variable called “speed”. As the name suggests, that number is the speed at which the motors are turned. In all instances of the code calling for the motor functions, we have set that number to be equal to “follow_speed”, a variable defined in line 69 (for instance, in follow() we called for “motor_forwards(follow_speed)”). This number can vary from 0 to 255, and we use it during analogWrite(). By doing an analogWrite() to either “a_en” or “b_en” and including that “speed” variable in the place of HIGH/LOW, like is shown in lines 208 and 209, we can change the speed at which the motors “a” and “b” are spinning. 

Setting the variable “speed” to 0 corresponds to minimum speed, whilst setting it to 255 corresponds maximum speed.
We take advantage of this, for instance, to implement the functions motor_right() and motor_left(). All we do is set one of the motors, “a” or “b” depending on the directions left and right, to run slower than the other by diving the variable “speed” by 2. This is shown, for instance, on lines 228 and 229 for the motor_right() functions, and on lines 238 and 239 for the motor_left() function.

Finally, for the motor_spin_left() and motor_spin_right() functions, all that needs to noted is that the motors “a” and “b” are set to spin in opposite directions, thus making the robot spin on its axis.

---

Now I’ll note a couple of more aspects about the code, but not related to specific functions themselves. First, that in the setup() function we start with line 78, which “sets up serial communication”. If you skim back throughout the code, you’ll see that in many instances we used the Serial.print() function. This is done for debugging purposes, such that we can use the Serial Monitor to, well, monitor our code.

Also notice the usage of prototypes at the beginning of the code. Essentially, to be able to write the implementation for the auxiliary functions below setup() and loop(), we have to include these in the beginning of the code (lines 30 to 53). This is a feature specific to C, and if it interests the reader, check our this guide (https://www.programiz.com/c-programming/c-user-defined-functions#:~:text=A%20function%20prototype%20is%20simply,be%20used%20in%20the%20program).

Finally, do note that some of the variables defined in the span of lines 56 to 69 contain the token “const” before their declaration. As the token implies, this means that they are constants, and thus once they are defined, they cannot be changed. This is useful in preventing the creation of bugs that we ourselves created, like accidently changing a variable that should not be changed. 
