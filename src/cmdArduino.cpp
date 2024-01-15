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
    \file Cmd.c

    This implements a simple command line interface for the Arduino so that
    its possible to execute individual functions within the sketch. 
*/
/**************************************************************************/
 
/* Included header file */
#include "cmdArduino.h"

/**************************************************************************/
/*!
    constructor - no pointers provided to any streams
*/
/**************************************************************************/
Cmd::Cmd() {

    /* Pint to the Serial Stream to the default HW Serial interface */
    _Cereal = &Serial;

    /* init the buffers */
    init_buffers();

}

/**************************************************************************/
/*!
    constructor - pointers provided to a HardwareSerial stream
*/
/**************************************************************************/
Cmd::Cmd(HardwareSerial *stream) {

    /* Point to the HardwareSerial Stream that the user passed */
    _hwStream = stream;
    set_stream();

    /* init the buffers */
    init_buffers();

}

/**************************************************************************/
/*!
    constructor - pointers provided to a SoftwareSerial stream
*/
/**************************************************************************/
#ifdef SoftwareSerial_h
    Cmd::Cmd(SoftwareSerial *stream) {

        /* Point to the HardwareSerial Stream that the user passed */
        _swStream = stream;
        set_stream();

        /* init the buffers */
        init_buffers();
    }
#endif

/**************************************************************************/
/*!
    Initialize the command line interface. This sets the terminal speed and
    and initializes things. 
*/
/**************************************************************************/
void Cmd::begin(uint32_t speed, SerialConfig config /*=SERIAL_8N1*/)
{
    #ifdef SoftwareSerial_h
        if (_hwStream) {
            _hwStream->begin(speed, config);
        } else {
            _swStream->begin(speed, config);
        }
    #else
        _hwStream->begin(speed, config);
    #endif

}

/**************************************************************************/
/*!
    Add a command to the command table. The commands should be added in
    at the setup() portion of the sketch. 
*/
/**************************************************************************/
void Cmd::add(const char *name, void (*func)(int argc, char **argv))
{
    // alloc memory for command struct
    cmd_tbl = (cmd_t *)malloc(sizeof(cmd_t));

    // alloc memory for command name
    char *cmd_name = (char *)malloc(strlen(name)+1);

    // copy command name
    strcpy(cmd_name, name);

    // terminate the command name
    cmd_name[strlen(name)] = '\0';

    // fill out structure
    cmd_tbl->cmd = cmd_name;
    cmd_tbl->func = func;
    cmd_tbl->next = cmd_tbl_list;
    cmd_tbl_list = cmd_tbl;
}

/**************************************************************************/
/*!
    Convert a string to a number. The base must be specified, ie: "32" is a
    different value in base 10 (decimal) and base 16 (hexadecimal).
*/
/**************************************************************************/
uint32_t Cmd::conv(char *str, uint8_t base)
{
    return strtol(str, NULL, base);
}

/**
 * @brief Set the the interface mode to be standard (normal user input) or JSON programmatic
 *          Note: setting the mode will automatically clear the command buffer
 * @param mode CLI_MODE_STD = standard mode , CLI_MODE_JSON = JSON programmatic mode, CLI_MODE_JSON_MAN = JSON manual mode
*/
void Cmd::mode(uint8_t mode) {
    /* Update the global tracker */
    _mode = mode;

    /* Reset the message pointers the message buffers */
    rst_msg_ptr();

    /* Determine printback settings */

    switch(_mode) {
        default:
        case CLI_MODE_STD:
            /* Enable printback to the terminal */
            _print = true;

            /* Display the command prompt */
            display_prompt();
            break;
        case CLI_MODE_JSON:
            /* Disable printback to the terminal and don't display the prompt */
            _print = false;
            break;
        case CLI_MODE_JSON_MAN:
            /* Enable printback to the terminal, but don't print a new prompt */
            _print = true;
            break;
    }
}

/**
 * @brief Allows the user to attach a callback function to be called after receivng a JSON message
 * @param *json_func() pointer to a callFback function provided by the user to be triggered when a JSON message is available
*/
void Cmd::attach_json_callback(void (*json_func)(char *json_msg)) {
    _json_func = json_func;
}

/* user can poll this function to check whether or not the exit character has been passed, allowing them to terminate the command line if desired */
uint8_t Cmd::exit_cli() {

    if (_exit_cli) {
        /* Clear flag - to prevent infinitely sending the exit flag */
        _exit_cli = false;

        /* reset the command buffer pointer, essentially dropping any in-progress commands */
        rst_msg_ptr();

        /* Return the true flag once per trigger */
        return true;
    } else {
        return false;
    }

}

/* terminate the msg buffer as a string and cleanup pointers */
void Cmd::return_key() {
    /* If this is the first character (i.e. - user hit 'enter' on a blank line), print a new line and do nothing */
    if (msg_ptr == msg) {
        /* print the command prompt */
        display_prompt();

        /* Break early - no other processing needed for a blank line */
        return;
    }

    /* make the Command buffer a null-terminated string */
    *msg_ptr = '\0';

    /* Clear the terminal line for printing */
    cli_println("");

    /* Pass the command buffer to the appropriate parser */
    switch(_mode) {
        default:
        case CLI_MODE_STD:
            parse_std((char *)msg);
            break;
        case CLI_MODE_JSON:
            parse_json((char *)msg);
            break;
        case CLI_MODE_JSON_MAN:
            parse_json((char *)msg);
            break;
    }

    /* reset the pointers back to their starting positions to prepare for next entries */
    rst_msg_ptr();
}

/**************************************************************************/
/*!
    Generate the main command prompt
*/
/**************************************************************************/
void Cmd::display()
{
    /* temporary RAM buffer for displaying the command banner + command prompt */
    char buf[CMD_BUF_SIZE];

    cli_println();

    /* Copy banner from flash to RAM buffer */
    strcpy_P(buf, cmd_banner);
    cli_println(buf);

    /* Copy prompt from flash to RAM buffer */
    strcpy_P(buf, cmd_prompt);
    cli_print(buf);
}

/**************************************************************************/
/*!
    Generate the main banner
*/
/**************************************************************************/
void Cmd::display_banner()
{
    /* temporary RAM buffer for displaying the command banner */
    char buf[CMD_BUF_SIZE];

    cli_println();

    /* Copy banner from flash to RAM buffer */
    strcpy_P(buf, cmd_banner);
    cli_println(buf);
}

/**************************************************************************/
/*!
    Generate the main prompt
*/
/**************************************************************************/
void Cmd::display_prompt()
{
    /* temporary RAM buffer for displaying the command prompt */
    char buf[CMD_BUF_SIZE];

    cli_println("");

    /* Copy banner from flash to RAM buffer */
    strcpy_P(buf, cmd_prompt);
    cli_print(buf);
}

/**************************************************************************/
/*!
    Parse the command line. This function tokenizes the command input, then
    searches for the command table entry associated with the commmand. Once found,
    it will jump to the corresponding function.
*/
/**************************************************************************/
void Cmd::parse_std(char *cmd)
{
    uint8_t argc, i = 0;
    char *argv[CMD_MAX_ARGS];
    char buf[CMD_BUF_SIZE];
    cmd_t *cmd_entry;

    fflush(stdout);

    // parse the command line statement and break it up into space-delimited
    // strings. the array of strings will be saved in the argv array.
    argv[i] = strtok(cmd, " ");
    do
    {
        argv[++i] = strtok(NULL, " ");
    } while ((i < CMD_MAX_ARGS) && (argv[i] != NULL));
    
    // save off the number of arguments for the particular command.
    argc = i;

    // parse the command table for valid command. used argv[0] which is the
    // actual command name typed in at the prompt
    for (cmd_entry = cmd_tbl; cmd_entry != NULL; cmd_entry = cmd_entry->next)
    {
        if (!strcmp(argv[0], cmd_entry->cmd))
        {
            cmd_entry->func(argc, argv);
            display();
            return;
        }
    }

    // command not recognized. print message and re-generate prompt.
    strcpy_P(buf, cmd_unrecog);
    cli_println(buf);

    display();
}

/* parse_json() - performs the buffer parsing based on JSON formatting */
void Cmd::parse_json(char *cmd) {
    /* Verify the string is actually in a JSON format (should be enclosed in {}, without any other brackets contained in the string ) */
    if(strchr(cmd, '{') == cmd && strchr(cmd + 1, '{') == NULL && strchr(cmd, '}') == cmd + strlen(cmd) - 1) {
        /* Valid JSON message --> call the users function */
        _json_func((char *)msg);
    }
}

/**************************************************************************/
/*!
    This function processes the individual characters typed into the command
    prompt. It saves them off into the message buffer unless its a "backspace"
    or "enter" key. 
*/
/**************************************************************************/
void Cmd::handler()
{   
    char c = _Cereal->read();

    /* See if we're tracking an esc command code, or standart text character */
    if (_esc_cmd_ptr - _esc_cmd) {
        handle_esc_cmd_char(c);
    } else {
        switch (c) {
            case CLI_ASCII_ESC:
                // escape
                esc_key();
                break;

            case CLI_ASCII_LF:
                // Do Nothing - only handle CR, ignore LF, but clear escape fl
                break;

            case CLI_ASCII_CR:
                // enter
                return_key();
                break;
            
            case CLI_ASCII_BS:
                // backspace
                backspace(CLI_ASCII_BS);
                break;

            case CLI_ASCII_DEL:
                // delete (used by some terminals for the 'backspace' key)
                backspace(CLI_ASCII_DEL);
                break;
            
            default:
                // normal key
                standard_key(c);
                break;
        }
    }
}

/* point to the appropriate stream, based on what the user has passed */
void Cmd::set_stream() {
    #ifdef SoftwareSerial_h
        /* point common Stream to either the HW or SW stream, depending on what was passed */
        _Cereal = !_hwStream ? (Stream*)_swStream : _hwStream;
    #else
        _Cereal = _hwStream;
    #endif
}

/* initialization for pointers and buffers */
void Cmd::init_buffers() {
    /* init the command table */
    cmd_tbl_list = NULL;

    /* Reset all message pointers */
    rst_msg_ptr();
}

/* reset the message pointers */
void Cmd::rst_msg_ptr() {
    /* init the msg ptr to the start of the command buffer */
    msg_ptr = msg;

    /* init the escape command ptr to the start of the escape command buffer */
    _esc_cmd_ptr = _esc_cmd;

    /* Set the cursor pointer to be at the starting position */
    _cursor_ptr = msg;
}

/**************************************************************************/
/*!
    This function should be set inside the main loop. It needs to be called
    constantly to check if there is any available input at the command prompt.
*/
/**************************************************************************/
void Cmd::poll()
{
    while (_Cereal->available())
    {
        handler();
    }
}

/* prepare to look for escape commands */
void Cmd::esc_key() {
    // if the esc key is pressed twice in a row
    if (_esc_cmd_ptr - _esc_cmd == 1) {_exit_cli = true;}

    // Add to the escape command char
    *_esc_cmd_ptr++ = CLI_ASCII_ESC;
}

/* standard char was passed, add to the buffer and proceed as normal */
void Cmd::standard_key(char c) {
    /**
     * Basic algorithm
     *      Increment msg_ptr size since we have a new character
     *      Add the character at the cursor pointer location and move the cursor pointer
     *      IF cursor pointer is at the end of the line, just print character and move on
     *      ELSE need to "insert" the character at the terminal's cursor position:
     *          1) Move all characters starting at the cursor pointer up by one byte
     *          2) Add the new character at the cursor pointer
     *          3) Clear the screen at the cursor and reprint all characters from the cursor to the end of the buffer
    */

    // verify we haven't reached the limit for msg length
    if ((msg_ptr - msg) == (CMD_MAX_MSG_SIZE-1))
    {   
        // command is too long, need to terminate it
        cli_println("");
        cli_println(F("Error: command too long. Please reduce command size."));

        // reset the pointers
        rst_msg_ptr();
        return;
    }

    // see if the cursor is at the end of the message, or in the middle somewhere
    if (_cursor_ptr == msg_ptr) {
        // normal character entered at the end of the buffer
        *_cursor_ptr++ = c;
        msg_ptr++;

        // print the character and move on
        cli_print(c);
    } else {
        /* move chars from n to n-1 before inserting the new char */
        for (uint8_t *chr_buf = msg_ptr; chr_buf > _cursor_ptr; chr_buf--) {*chr_buf = *(chr_buf - 1);}

        /* insert the new character at the cursor pointer and increment the buffer pointer size, since we've added a character */
        *_cursor_ptr = c;
        msg_ptr++;

        /* print the characters back to the terminal that are left in the buffer */
        uint16_t chars_printed = cursor_print();

        /* Move the terminal's cursor back to the desired position, which is one less than the characters printed (since we also printed the new char) */
        for (uint8_t i = 0; i < (chars_printed - 1); i++) {cli_print(ESC_CMD_LEFT);}

        /* now move the buffer pointers to be ready for new characters */
        _cursor_ptr++;
    }
}

/* currently handling ESC commands, so add char to the esc command buffer */
void Cmd::handle_esc_cmd_char(char c) {
    /* Check for unique scenario of back-to-back esc keys */
    if (c == CLI_ASCII_ESC) {
        esc_key();
        return;
    }

    /* Add to the escape command buffer */
    *_esc_cmd_ptr++ = c;

    /* See if we've collected the entire command */
    if ((_esc_cmd_ptr - _esc_cmd) >= ESC_CMD_SIZE) {
        /* If the 3rd digit is numerical, we need to keep receiving a 4th character */
        if (!_ext_esc_cmd && isdigit(c)) {
            _ext_esc_cmd = true;
            return;
        } else {
            /* Null terminate the buffer to create a string */
            *_esc_cmd_ptr = '\0';

            /* Check for a valid escape command */
            check_esc_cmd();

            /* Reset the escape commant pointer */
            _esc_cmd_ptr = _esc_cmd;

            /* Ensure the extended command flag is cleared */
            _ext_esc_cmd = false;
        }
    }
}

/* Check for valid escape commands */
void Cmd::check_esc_cmd() {
    if (strlen((char *)_esc_cmd) == ESC_CMD_SIZE) {
        /* checking against 3 char CMD */
        if (!strcmp((char*)_esc_cmd, ESC_CMD_UP)) {
            esc_cmd_up();
        } else if (!strcmp((char*)_esc_cmd, ESC_CMD_LEFT)) {
            esc_cmd_left();
        } else if (!strcmp((char*)_esc_cmd, ESC_CMD_RIGHT)) {
            esc_cmd_right();
        } else {
            /* Uncomment this block to help with debugging
                cli_print("[Invalid escape command received: ");
                for (uint8_t i = 0; i < ESC_CMD_SIZE; i++) {
                    cli_print("0x" + String(_esc_cmd[i], HEX) + " ");
                }
                cli_print("]");
            */
        }
    } else {
        /* checking against 4 char CMD */
        if (!strcmp((char*)_esc_cmd, ESC_CMD_DEL)) {
            esc_cmd_del();
        } else {
            /* Uncomment this block to help with debugging
                cli_print("[Invalid escape command received: ");
                for (uint8_t i = 0; i < ESC_EXT_CMD_SIZE; i++) {
                    cli_print("0x" + String(_esc_cmd[i], HEX) + " ");
                }
                cli_print("]");
            */
        }
    }
}

/* Escape Command Left Received */
void Cmd::esc_cmd_left() {
    /* If the cursor isn't already at the beginning, move it left */
    if (_cursor_ptr - msg) {
        /* Move the cursor left */
        cli_print(ESC_CMD_LEFT);

        /* Move the curosr pointer correspondingly */
        _cursor_ptr--;
    }
}

/* Escape Command Right Received */
void Cmd::esc_cmd_right() {
    /* If the cursor isn't already at the end, move it right */
    if (msg_ptr - _cursor_ptr) {
        /* Move the cursor left */
        cli_print(ESC_CMD_RIGHT);

        /* Move the curosr pointer correspondingly */
        _cursor_ptr++;
    }
}

/* Escape Command Up Received */
void Cmd::esc_cmd_up() {

}

/**
 * @brief Re-print characters from the cursor pointer to the end of the msg buffer
 * @retval Returns the qty of characters printed
*/
uint16_t Cmd::cursor_print() {
    /* Clear the terminal to the right of the cursor */
    cli_print(ESC_CMD_CLR_LINE_RIGHT);

    /* Print the characters from the cursor to the end */
    uint16_t chars_printed = 0;
    for (uint8_t *ptr = _cursor_ptr; ptr < msg_ptr; ptr++) {
        cli_print((char)*ptr);
        chars_printed++;
    }
    return chars_printed;
}

/* Escape Command DEL (delete key) Received */
void Cmd::esc_cmd_del() {
    /**
     * Basic algorithm for delete key:
     *  1) Downshift the contents of the buffer between the cursor position and the end of the msg buffer (overwriting the character at the cursors position)
     *  2) Clear the terminal to the right of the cursor, and reprint the new characters from the cursor pointer to the end of the msg buffer
     *  3) Return the terminal's cursor back to the original position it was in step 1
    */



    /**
     * For buffer contents chr_buf[i,i+1,...,n-1,n] do the following:
     *  1) move char from i+1 to i
     *  2) print the moved char back to the terminal
     *  3) loop (incrementing i) until n-1
     * 
     * Reminder: msg_ptr is always pointed to the "last valid char" + 1
    */
    
    /* move chars from i+1 to i */
    for (uint8_t *chr_buf = _cursor_ptr; chr_buf < msg_ptr; chr_buf++) {*chr_buf = *(chr_buf + 1);}

    /* move the msg_ptr down if we actually deleted something */
    if (_cursor_ptr < msg_ptr) {msg_ptr--;}

    /* print the characters back to the terminal that are left in the buffer */
    uint16_t chars_printed = cursor_print();

    /* Move the terminal's cursor back to the desired position (since printing above moved it to the end of the line) */
    for (uint8_t i = 0; i < chars_printed; i++) {cli_print(ESC_CMD_LEFT);}
}

/* Backspace character */
void Cmd::backspace(char _bs_char/*=CLI_ASCII_DEL*/) {
    /**
     * Basic algorithm:
     *      IF cursor is at the begining of the terminal, do nothing
     *      IF cursor is at the end, simply move the pointers down by one and print BS to the terminal
     *      ELSE we need to move the cursor down one spot, then just call the DEL function
    */
    if (_cursor_ptr > msg) {
        if (_cursor_ptr == msg_ptr) {
            msg_ptr--;
            _cursor_ptr--;
            cli_print(_bs_char);
        } else {
            /* Move cursor down one spot and call the DEL function */
            _cursor_ptr--;
            cli_print(ESC_CMD_LEFT);
            esc_cmd_del();
        }
    }
}