/*****************************************************
This is the demo sketch for the command line interface
by FreakLabs. It's a simple command line interface
where you can define your own commands and pass arguments
to them. 
*****************************************************/
#include "cmdArduino.h"

int led_pin = 13;
bool led_blink_enb = false;
int led_blink_delay_time = 1000;
int pwm_pin = 10;

/* Initial Constructor for command line */
Cmd cmd;

void setup()
{
  // set the led pin as an output. its part of the demo.
  pinMode(led_pin, OUTPUT); 
  pinMode(pwm_pin, OUTPUT); 
  
  // init the command line and set it for a speed of 57600
  cmd.begin(57600);
  
  // add the commands to the command table. These functions must
  // already exist in the sketch. See the functions below. 
  // The functions need to have the format:
  //
  // void func_name(int arg_cnt, char **args)
  //
  // arg_cnt is the number of arguments typed into the command line
  // args is a list of argument strings that were typed into the command line
  cmd.add("hello", hello);
  cmd.add("args", arg_display);
  cmd.add("blink", led_blink);
  cmd.add("pwm", led_pwm);

  Serial.println("Command Line Example 5 - Tying It All Together");  

}

void loop()
{
  cmd.poll();
  
  if (led_blink_enb)
  {
    digitalWrite(led_pin, HIGH);    // set the LED on
    delay(led_blink_delay_time);    // wait for a second
    digitalWrite(led_pin, LOW);     // set the LED off
    delay(led_blink_delay_time);     
  }
}

// Print "hello world" when called from the command line.
// 
// Usage:
// hello
void hello(int argCnt, char **args)
{
  Serial.println("Hello world.");
}

// Display the contents of the args string array. 
// 
// Usage:
// args 12 34 56 hello gothic baby
//
// Will display the contents of the args array as a list of strings
// Output:
// Arg 0: args
// Arg 1: 12
// Arg 2: 34
// Arg 3: 56
// Arg 4: hello
// Arg 5: gothic
// Arg 6: baby
void arg_display(int argCnt, char **args)
{
  for (int i=0; i<argCnt; i++)
  {
    Serial.print("Arg ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(args[i]);
  }
}

// Blink the LED with the delay time specified in the args.
//
// Usage:
// blink 100
//
// Blinks the LED with 100 msec on/off time
//
// Usage:
// blink
//
// Turns off the LED.
void led_blink(int argCnt, char **args)
{
  if (argCnt > 1)
  {
    led_blink_delay_time = cmd.conv(args[1], 10);
    led_blink_enb = true;
  }
  else
  {
    led_blink_enb = false;
  } 
}

// This function sets the brightness of an LED on pin 10 via PWM from the 
// command line. If no args are present,then the LED will be turned off.
//
// Usage:
// pwm 100
// 
// Sets the LED brightness to a value of 100 on a scale from 0 to 255
//
// Usage:
// pwm
//
// Turns off the LED
void led_pwm(int argCnt, char **args)
{
  int pwm_val;
  
  if (argCnt > 1)
  {
    // if args are present, then use the first arg as the brightness level
    pwm_val = cmd.conv(args[1], 10);
    analogWrite(pwm_pin, pwm_val);
  }
  else
  {
    // if no args, turn off the LED
    analogWrite(pwm_pin, 0);
  }
}
