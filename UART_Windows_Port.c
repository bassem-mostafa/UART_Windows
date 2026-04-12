// #############################################################################
// #### Copyright ##############################################################
// #############################################################################

/*
 * Copyright 2024 BaSSeM
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// #############################################################################
// #### Description ############################################################
// #############################################################################

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

#include "Platform.h"

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

#ifndef DEBUG
    #define DEBUG
#endif

#ifdef DEBUG
    #undef DEBUG
#endif

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#ifdef UART_WINDOWS

// #############################################################################
// #### Include(s) #############################################################
// #############################################################################

    #include "../../UART_Internal.h"
    #include "UART_Windows_Port.h"

    #include <fcntl.h>
    #include <stdio.h>
    #include <termios.h>

// #############################################################################
// #### Private Macro(s) #######################################################
// #############################################################################

// #############################################################################
// #### Private Type(s) ########################################################
// #############################################################################

typedef struct UART_InstanceContext
{
    FILE * file;
} UART_InstanceContext_t;

// #############################################################################
// #### Private Method(s) Prototype ############################################
// #############################################################################

// #############################################################################
// #### Private Variable(s) ####################################################
// #############################################################################

// #############################################################################
// #### Private Method(s) ######################################################
// #############################################################################

// #############################################################################
// #### Public Method(s) #######################################################
// #############################################################################

UART_Status_t UART_IsValid( UART_t UART )
{
    UART_Status_t UART_Status = UART_Status_Error;
    do
    {
        UART_Trace( "%s( UART=%d )", __FUNCTION__, UART );
        switch ( UART )
        {
            case UART_1:
            case UART_2:
                UART_Status = UART_Status_Success;
                break;
            default:
                UART_Status = UART_Status_ArgumentInvalid;
                break;
        }
    }
    while ( 0 );
    return UART_Status;
}

UART_Status_t UART_Instance_Initialize( UART_Instance_t * UART_Instance )
{
    UART_Status_t UART_Status = UART_Status_Error;
    do
    {
        UART_Trace( "%s( Instance=%p )", __FUNCTION__, UART_Instance );
        if ( ( UART_Status = UART_Instance_IsValid( UART_Instance ) ) != UART_Status_Success )
        {
            break;
        }
        if ( UART_Instance->Context == NULL )
        {
            RAM_Status_t RAM_Status = RAM_Status_Error;
            if ( ( RAM_Status = RAM_Allocate( RAM_1, ( RAM_Reference_t * ) &UART_Instance->Context, UTIL_SizeOf( UART_InstanceContext_t ) ) ) != RAM_Status_Success )
            {
                UART_Status = UART_Status_Error;
                break;
            }
            // Double check context validity
            if ( UART_Instance->Context == NULL )
            {
                UART_Status = UART_Status_Error;
                break;
            }
        }
        switch ( UART_Instance->UART )
        {
            case UART_1:
            case UART_2:
                do
                {
                    int port = -1;
                    switch ( UART_Instance->UART )
                    {
                        case UART_1:
                            port = 1;
                            break;
                        case UART_2:
                            port = 101;
                            break;
                        default:
                            break;
                    }
                    if ( port < 0 )
                    {
                        UART_Status = UART_Status_Error;
                        break;
                    }
                    FILE * handle = NULL;
                    const char PortNameFormat[] = "/dev/ttyS%d"; // "/dev/ttyS0" => "COM1", "/dev/ttyS1" => "COM2"
                    char PortName[ 20 ];
                    snprintf( PortName, sizeof( PortName ), PortNameFormat, --port );
                    handle = fdopen( open( PortName, O_RDWR | O_NOCTTY | O_NDELAY ), "r+" );
                    if ( handle == NULL )
                    {
                        UART_Status = UART_Status_Error;
                        break;
                    }

                    struct termios options;
                    if ( tcgetattr( fileno( handle ), &options ) != 0 )
                    {
                        // FIXME
                    }
                    options.c_cflag |= B57600;  // 9600 Baud-rate, possible values are {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400} & {B57600, B115200, B128000, B230400, B256000, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000}
                    options.c_cflag &= ~PARENB; // No Parity, possible values are {No: (~PARENB), Even: (PARENB), Odd: (PARENB | PARODD)}
                    options.c_cflag &= ~CSTOPB; // One Stop, possible values are {One: (~CSTOPB), Two: (CSTOPB)}
                    options.c_cflag &= ~CSIZE;  // CS5 equivalent, possible values are {CS5, CS6, CS7, CS8}
                    options.c_cflag |= CS8;
                    if ( tcsetattr( fileno( handle ), TCSANOW, &options ) != 0 )
                    {
                        // FIXME
                    }
                    if ( setvbuf( handle, NULL, _IONBF, 0 ) != 0 )
                    {
                        // FIXME Stream buffering disable failed
                    }
                    UART_Instance->Context->file = handle;
                    UART_Status = UART_Status_Success;
                }
                while ( 0 );
                break;
            default:
                // Nothing to be done
                break;
        }
    }
    while ( 0 );
    return UART_Status;
}

UART_Status_t UART_Instance_Cycle( UART_Instance_t * UART_Instance )
{
    UART_Status_t UART_Status = UART_Status_Error;
    do
    {
        UART_Trace( "%s( Instance=%p )", __FUNCTION__, UART_Instance );
        if ( ( UART_Status = UART_Instance_IsValid( UART_Instance ) ) != UART_Status_Success )
        {
            break;
        }
        // Nothing to be done
        UART_Status = UART_Status_Success;
    }
    while ( 0 );
    return UART_Status;
}

UART_Status_t UART_Instance_DeInitialize( UART_Instance_t * UART_Instance )
{
    UART_Status_t UART_Status = UART_Status_Error;
    do
    {
        UART_Trace( "%s( Instance=%p )", __FUNCTION__, UART_Instance );
        if ( ( UART_Status = UART_Instance_IsValid( UART_Instance ) ) != UART_Status_Success )
        {
            break;
        }
        switch ( UART_Instance->UART )
        {
            case UART_1:
            case UART_2:
                do
                {
                    FILE * handle = UART_Instance->Context->file;
                    if ( handle != NULL )
                    {
                        if ( fclose( handle ) != 0 )
                        {
                            UART_Error( "Couldn't close stream handle UART_%d", UART_Instance->UART );
                        }
                    }
                    UART_Instance->Context->file = NULL;
                    UART_Status = UART_Status_Success;
                }
                while ( 0 );
                break;
            default:
                break;
        }
    }
    while ( 0 );
    return UART_Status;
}

UART_Status_t UART_Instance_Write( UART_Instance_t * UART_Instance, UART_Data_t * UART_Data, UART_DataLength_t UART_DataLength )
{
    UART_Status_t UART_Status = UART_Status_Error;
    do
    {
        UART_Trace( "%s( Instance=%p )", __FUNCTION__, UART_Instance );
        if ( ( UART_Status = UART_Instance_IsValid( UART_Instance ) ) != UART_Status_Success )
        {
            break;
        }
        if ( fwrite( UART_Data, sizeof( UART_Data_t ), UART_DataLength, UART_Instance->Context->file ) != UART_DataLength )
        {
            UART_Status = UART_Status_Error;
            break;
        }
        UART_Status = UART_Status_Success;
    }
    while ( 0 );
    return UART_Status;
}

UART_Status_t UART_Instance_Read( UART_Instance_t * UART_Instance, UART_Data_t * UART_Data, UART_DataLength_t UART_DataLength )
{
    UART_Status_t UART_Status = UART_Status_Error;
    do
    {
        UART_Trace( "%s( Instance=%p )", __FUNCTION__, UART_Instance );
        if ( ( UART_Status = UART_Instance_IsValid( UART_Instance ) ) != UART_Status_Success )
        {
            break;
        }
        if ( fread( UART_Data, sizeof( UART_Data_t ), UART_DataLength, UART_Instance->Context->file ) != UART_DataLength )
        {
            UART_Status = UART_Status_Error;
            break;
        }
        UART_Status = UART_Status_Success;
    }
    while ( 0 );
    return UART_Status;
}

// #############################################################################
// #### Public Variable(s) #####################################################
// #############################################################################

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#endif /* UART_WINDOWS */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
