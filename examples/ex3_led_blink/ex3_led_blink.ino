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

/* Initial Constructor for command line */
Cmd cmd;

void setup()
{
  // set the led pin as an output. its part of the demo.
  pinMode(led_pin, OUTPUT); 
  
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
  cmd.add("blink", led_blink);

  Serial.println("Command Line Example 3 - Interactively Blinking LED");
}

void loop()
{
  cmd.poll();
  
  // This is where the blinking happens. The led_blink function
  // only controls the delay time and whether to enable the blinking
  // action or not. 
  // One thing to be careful of is having delays in the loop() function.
  // This will slow down the response time of the command line since
  // the loop() function needs to get past the delays before it can
  // check for any commands at the command line. 
  if (led_blink_enb)
  {
    digitalWrite(led_pin, HIGH);    // set the LED on
    delay(led_blink_delay_time);    // wait for a second
    digitalWrite(led_pin, LOW);     // set the LED off
    delay(led_blink_delay_time);     
  }
}

// Blink the LED. This is an example of using command line arguments
// to call a function in a sketch.
// If a numeric arg is specified, then use that to set the 
// delay time. If called with no arguments, then turn the LED off.
//
// Usage: At the command line, to blink the LED, type:
// blink 100
// 
// This blinks the LED with a 100 msec on/off time. 
//
// Usage: At the command line, to turn off the LED, type:
// blink
//
// Calling the function with no arguments will turn off the LED
//
// Also, you'll notice that the function "cmd.conv()" is needed. Since
// the numeric arg is stored as an ASCII string, it needs to be converted
// to an integer. When you call cmdStr2Num, you need to specify two arguments:
// 1) the numeric string to be converted
// 2) the numeric base that will be used to convert it,ie: 10 = decimal, 16 = hex
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


