/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/
/*!
    \file 
    \ingroup


*/
/**************************************************************************/
#ifndef CMDARDUINO_H
    #define CMDARDUINO_H

    /* Include referenced libraries */
    #include <stdint.h>
    #include <Arduino.h>
    #include <HardwareSerial.h>


    /* Global Strings to be stored in flash to save RAM */
    const char cmd_banner[] PROGMEM = "*************** CMD *******************";    // Command Prompt Strings (stored in flash to save RAM)
    const char cmd_prompt[] PROGMEM = "CMD >> ";                                    // Command Prompt Strings (stored in flash to save RAM)
    const char cmd_unrecog[] PROGMEM = "CMD: Command not recognized.";              // Command Prompt Strings (stored in flash to save RAM)
    #define CMD_BUF_SIZE 50     // Set this number greater than or equal to the largest string length above (used for strcpy)

    /* Common Definitions for use with this module */
    #ifndef CMD_MAX_MSG_SIZE
        #define CMD_MAX_MSG_SIZE    1024                                            //Maximum command line input supported (in bytes).
                                                                                    //If larger input is desired, user can #define CMD_MAX_MSG_SIZE prior to including this library
    #endif

    #ifndef CMD_MAX_ARGS
        #define CMD_MAX_ARGS 128    // Maximum number of command line arguments that can be parsed
                                    //If larger input is desired, user can #define CMD_MAX_ARGS prior to including this library
    #endif

    #define CLI_EXIT_CHAR 0x1B                                                      //ASCII hex for 'Escape' - can be used to end the command line

    /* Custom command line structure */
    typedef struct _cmd_t
    {
        char *cmd;
        void (*func)(int argc, char **argv);
        struct _cmd_t *next;
    } cmd_t;


    /* Class Definition */
    class Cmd
    {

        public:
            /* Constructor of Cmd Class - no pointers provided to any streams */
            Cmd();

            /* Constructor of Cmd Class - pointers provided to a HardwareSerial stream */
            Cmd(HardwareSerial *stream);

            #ifdef SoftwareSerial_h
                /* Constructor of Cmd Class - pointers provided to a SoftwareSerial stream */
                Cmd(SoftwareSerial *stream);
            #endif

            /* begin() - should only be called if the calling program hasn't initated the serial stream yet */
            void begin(uint32_t speed, uint8_t config=SERIAL_8N1);

            /* poll() - repeteadly called by the user's loop to check the command line inputs */
            void poll();

            /* add() - allows the user to add command + callback functions to be triggered when the command is seen */
            void add(const char *name, void (*func)(int argc, char **argv));

            /* conv() - allows the user to convert a string to number */
            uint32_t conv(char *str, uint8_t base=10);

            /* user can poll this function to check whether or not the exit character has been passed, allowing them to terminate the command line if desired */
            uint8_t exit_cli();

        private:
            /* display() - prints the command banner + command prompt strings to the stream */
            void display();

            /* display_banner() - prints the command banner string to the stream */
            void display_banner();

            /* display_prompt() - prints the command prompt string to the stream */
            void display_prompt();

            /* parse() - performs the buffer parsing to check for commands / arguments */ 
            void parse(char *cmd);

            /* handler() - called by poll() to handle all periodic tasks/buffer checking */
            void handler();

            /* point to the appropriate stream, based on what the user has passed */
            void set_stream();

            /* initialization for pointers and buffers */
            void init_buffers();

            /* Command input buffer */
            uint8_t msg[CMD_MAX_MSG_SIZE];

            /* Pointer to the Command input buffer */
            uint8_t *msg_ptr;
            
            /* Custom structure for command table entries */
            cmd_t *cmd_tbl_list, *cmd_tbl;
            
            /* Stream pointer to provide support on multiple HW or SW Serial Interfaces (allows flexibility for stream calls)*/
            Stream *_Cereal;

            /* HardwareSerial pointer to provide .begin() support for HW Serial Interfaces */
            HardwareSerial* _hwStream;

            #ifdef SoftwareSerial_h
                /* SoftwareSerial pointer to provide .begin() support for SW Serial Interfaces */
                SoftwareSerial* _swStream;
            #endif

            /* Boolean to allow calling functions to check if the exit character was enterred - allowing the termination of the CLI */
            uint8_t exit_char = false;

    };

#endif //CMDARDUINO_H
