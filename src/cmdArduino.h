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

    #define CLI_ASCII_CR '\r'       // ASCII Carriage Return
    #define CLI_ASCII_LF '\n'       // ASCII Line Feed
    #define CLI_ASCII_BS '\b'       // ASCII Backspace
    #define CLI_ASCII_DEL 0x7F      // ASCII DEL (sometimes used by terminals when pressing backpsace)
    #define CLI_ASCII_ESC '\e'      // ASCII ESC

    #define ESC_CMD_SIZE 3                      // Length of escape commands to track (including the \e character).  Currently, only support arrow keys: [A, [C, [D
    #define ESC_CMD_UP              "\e[A"      // Arrow up (recall last command)
    #define ESC_CMD_RIGHT           "\e[C"      // Moves the cursor to the right
    #define ESC_CMD_LEFT            "\e[D"      // Moves the cursor to the left

    #define ESC_EXT_CMD_SIZE 4                  // Length of escape commands to track when a number is detected in the 3rd position
    #define ESC_CMD_CLR_LINE_FULL   "\e[2K"     // clears the entire line
    #define ESC_CMD_DEL             "\e[3~"     // command sent by putty when the DEL key is pressed
    #define ESC_CMD_CLR_LINE_RIGHT  "\e[0K"     // clears to the end of the line (starting at cursors position)

    #define CLI_MODE_STD 0                      // standard / manual input (commands / args parsed by spaces).  Terminal printback is on.
    #define CLI_MODE_JSON 1                     // JSON / programmatic input (command sequence is parsed as a JSON string with keys, all text must be between {} ).  Terminal printback is off.
    #define CLI_MODE_JSON_MAN 2                 // JSON / manual input (command sequence is parsed as a JSON string with keys, all text must be between {} ).  Terminal printback is on.

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
            void begin(uint32_t speed, SerialConfig config=SERIAL_8N1);

            /* poll() - repeteadly called by the user's loop to check the command line inputs */
            void poll();

            /* add() - allows the user to add command + callback functions to be triggered when the command is seen */
            void add(const char *name, void (*func)(int argc, char **argv));

            /* conv() - allows the user to convert a string to number */
            uint32_t conv(char *str, uint8_t base=10);

            /**
             * @brief Set the the interface mode to be standard (normal user input) or JSON programmatic
             *          Note: setting the mode will automatically clear the command buffer
             * @param mode CLI_MODE_STD = standard mode , CLI_MODE_JSON = JSON programmatic mode, CLI_MODE_JSON_MAN = JSON manual mode
            */
            void mode(uint8_t mode);

            /**
             * @brief Allows the user to attach a callback function to be called after receivng a JSON message
             * @param *json_func() pointer to a callback function provided by the user to be triggered when a JSON message is available
            */
           void attach_json_callback(void (*json_func)(char *json_msg));

            /* user can poll this function to check whether or not the exit character has been passed, allowing them to terminate the command line if desired */
            uint8_t exit_cli();

        private:
            /* display() - prints the command banner + command prompt strings to the stream */
            void display();

            /* display_banner() - prints the command banner string to the stream */
            void display_banner();

            /* display_prompt() - prints the command prompt string to the stream */
            void display_prompt();

            /* parse_std() - performs the buffer parsing to check for commands / arguments */ 
            void parse_std(char *cmd);

            /* parse_json() - performs the buffer parsing based on JSON formatting */
            void parse_json(char *cmd);

            /* handler() - called by poll() to handle all periodic tasks/buffer checking */
            void handler();

            /* point to the appropriate stream, based on what the user has passed */
            void set_stream();

            /* initialization for pointers and buffers */
            void init_buffers();

            /* reset the message pointers */
            void rst_msg_ptr();

            /* terminate the msg buffer as a string and cleanup pointers */
            void return_key();

            /* prepare to look for escape commands */
            void esc_key();

            /* standard char was passed, add to the buffer and proceed as normal */
            void standard_key(char c);

            /* currently handling ESC commands, so add char to the esc command buffer */
            void handle_esc_cmd_char(char c);

            /* Check for valid escape commands */
            void check_esc_cmd();

            /* Escape Command Left Received */
            void esc_cmd_left();

            /* Escape Command Right Received */
            void esc_cmd_right();

            /* Escape Command Up Received */
            void esc_cmd_up();

            /**
             * @brief Re-print characters from the cursor pointer to the end of the msg buffer
             * @retval Returns the qty of characters printed
            */
            uint16_t cursor_print();

            /* Escape Command DEL (delete key) Received */
            void esc_cmd_del();

            /* Backspace character */
            void backspace(char _bs_char=CLI_ASCII_DEL);

            /* Simple print, gated by the global boolean */
            #define cli_print(x) if(_print){_Cereal->print(x);}

            /* Simple println, gated by the global boolean */
            #define cli_println(x) if(_print){_Cereal->println(x);}

            /* Command input buffer */
            uint8_t msg[CMD_MAX_MSG_SIZE];

            /* Pointer to the Command input buffer */
            uint8_t *msg_ptr;
            
            /* Custom structure for command table entries */
            cmd_t *cmd_tbl_list, *cmd_tbl;

            /* Escape command input buffer (increase buffer size by one to make it a NULL terminated string )*/
            uint8_t _esc_cmd[ESC_EXT_CMD_SIZE + 1];

            /* Pointer to the escape command buffer */
            uint8_t *_esc_cmd_ptr;

            /* Pointer to keep track of the cursor location when performing escape commands */
            uint8_t *_cursor_ptr;
            
            /* Stream pointer to provide support on multiple HW or SW Serial Interfaces (allows flexibility for stream calls)*/
            Stream *_Cereal;

            /* HardwareSerial pointer to provide .begin() support for HW Serial Interfaces */
            HardwareSerial* _hwStream;

            #ifdef SoftwareSerial_h
                /* SoftwareSerial pointer to provide .begin() support for SW Serial Interfaces */
                SoftwareSerial* _swStream;
            #endif

            /* Boolean to keep track of whether we've received an exit command */
            uint8_t _exit_cli = false;

            /* Boolean to keep track of whether we've received an extended escape command */
            uint8_t _ext_esc_cmd = false;

            /* Mode variable to keep track of the command line mode */
            uint8_t _mode = CLI_MODE_STD;

            /* Boolean to keep track of whether we should printback characters to the terminal */
            uint8_t _print = true;

            /* callback function to be called after receiving a JSON message */
            void (*_json_func)(char *json_msg) = NULL;
    };

#endif //CMDARDUINO_H
