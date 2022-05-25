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
    Generate the main command prompt
*/
/**************************************************************************/
void Cmd::display()
{
    /* temporary RAM buffer for displaying the command banner + command prompt */
    char buf[50];

    _Cereal->println();

    /* Copy banner from flash to RAM buffer */
    strcpy_P(buf, cmd_banner);
    _Cereal->println(buf);

    /* Copy prompt from flash to RAM buffer */
    strcpy_P(buf, cmd_prompt);
    _Cereal->print(buf);
}

/**************************************************************************/
/*!
    Generate the main banner
*/
/**************************************************************************/
void Cmd::display_banner()
{
    /* temporary RAM buffer for displaying the command banner */
    char buf[50];

    _Cereal->println();

    /* Copy banner from flash to RAM buffer */
    strcpy_P(buf, cmd_banner);
    _Cereal->println(buf);
}

/**************************************************************************/
/*!
    Generate the main prompt
*/
/**************************************************************************/
void Cmd::display_prompt()
{
    /* temporary RAM buffer for displaying the command prompt */
    char buf[50];

    _Cereal->println();

    /* Copy banner from flash to RAM buffer */
    strcpy_P(buf, cmd_prompt);
    _Cereal->print(buf);
}

/**************************************************************************/
/*!
    Parse the command line. This function tokenizes the command input, then
    searches for the command table entry associated with the commmand. Once found,
    it will jump to the corresponding function.
*/
/**************************************************************************/
void Cmd::parse(char *cmd)
{
    uint8_t argc, i = 0;
    char *argv[30];
    char buf[50];
    cmd_t *cmd_entry;

    fflush(stdout);

    // parse the command line statement and break it up into space-delimited
    // strings. the array of strings will be saved in the argv array.
    argv[i] = strtok(cmd, " ");
    do
    {
        argv[++i] = strtok(NULL, " ");
    } while ((i < 30) && (argv[i] != NULL));
    
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
    _Cereal->println(buf);

    display();
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

    switch (c)
    {
    case '\r':
        /* If this is the first character (i.e. - user hit 'enter' on a blank line), print a new line and do nothing */
        if (msg_ptr == msg) {
            /* print the command prompt */
            display_prompt();

            /* Break early - no other processing needed for a blank line */
            break;
        }

        /* make the Command buffer a null-terminated string */
        *msg_ptr = '\0';

        /* Clear the terminal line for printing */
        _Cereal->println("");

        /* Pass the command buffer to the parser */
        parse((char *)msg);

        /* reset the pointer back to the command buffer to prepare for next entries */
        msg_ptr = msg;
        break;
    
    case '\b':
        // ASCII Backspace
        if (msg_ptr > msg)
        {
            msg_ptr--;
            _Cereal->print(c);
        }
        break;

    case 0x7F:
        // delete (used by some terminals for the 'backspace' key)
        if (msg_ptr > msg)
        {
            msg_ptr--;
            _Cereal->print(c);
        }
        break;

    case CLI_EXIT_CHAR:
        // set the exit character flag
        exit_char = true;

        // No need to print or move pointer, this is a non valid command character
        break;
    
    default:
        // normal character entered. add it to the buffer
        _Cereal->print(c);
        *msg_ptr++ = c;

        // msg too long, clear command and display warning 
        if ((msg_ptr - msg) == (CMD_MAX_MSG_SIZE-1))
        {
            _Cereal->println("");
            _Cereal->println("Error: command too long. Please reduce command size.");
            msg_ptr = msg;
        }
        break;
    }
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

/**************************************************************************/
/*!
    Initialize the command line interface. This sets the terminal speed and
    and initializes things. 
*/
/**************************************************************************/
void Cmd::begin(uint32_t speed, uint8_t config /*=SERIAL_8N1*/)
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

/* initialization for pointers and buffers */
void Cmd::init_buffers() {

    /* init the msg ptr to the start of the command buffer */
    msg_ptr = msg;

    /* init the command table */
    cmd_tbl_list = NULL;
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

/* user can poll this function to check whether or not the exit character has been passed, allowing them to terminate the command line if desired */
uint8_t Cmd::exit_cli() {

    if (exit_char) {
        /* Clear flag - to prevent infinitely sending the exit flag */
        exit_char = false;

        /* reset the command buffer pointer, essentially dropping any in-progress commands */
        msg_ptr = msg;

        /* Return the true flag once per trigger */
        return true;
    } else {
        return false;
    }

}
