/*
 * uart_interrupt.c
 * 
 * Copyright (C) 2020, SpaceLab.
 * 
 * This file is part of EPS 2.0.
 * 
 * EPS 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * EPS 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EPS 2.0. If not, see <http:/\/www.gnu.org/licenses/>.
 * 
 */

/**
 * \brief UART with interrupt driver implementation.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * \author André M. P. de Mattos <andre.mattos@spacelab.ufsc.br>
 *
 * \version 0.2.28
 * 
 * \date 2021/08/01
 * 
 * \addtogroup uart_interrupt
 * \{
 */

#include <hal/usci_a_uart.h>
#include <hal/gpio.h>

#include <config/config.h>
#include <system/sys_log/sys_log.h>

#include "uart_interrupt.h"

static uint8_t uart_rx_buffer[UART_RX_BUFFER_MAX_SIZE] = {0};
static uint8_t uart_rx_data_size = 0;

/**
 * @brief UART interrupt service routine function prototype
 * 
 */
void USCI_A0_ISR(void);


int uart_interrupt_init(uart_interrupt_port_t port, uart_interrupt_config_t config)
{
    USCI_A_UART_initParam uart_params = {0};
    int err = 0;

    switch(config.baudrate)
    {
        case 1200:
            uart_params.clockPrescalar      = 1665; /* 1200 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 11;   /* 1200 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 1200 bps @ 31.981568 MHz */
            break;
        case 2400:
            uart_params.clockPrescalar      = 832;  /* 2400 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 14;   /* 2400 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 2400 bps @ 31.981568 MHz */
            break;
        case 4800:
            uart_params.clockPrescalar      = 416;  /* 4800 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 7;    /* 4800 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 4800 bps @ 31.981568 MHz */
            break;
        case 9600:
            uart_params.clockPrescalar      = 208;  /* 9600 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 3;    /* 9600 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 9600 bps @ 31.981568 MHz */
            break;
        case 19200:
            uart_params.clockPrescalar      = 104;  /* 19200 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 2;    /* 19200 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 19200 bps @ 31.981568 MHz */
            break;
        case 38400:
            uart_params.clockPrescalar      = 52;   /* 38400 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 1;    /* 38400 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 38400 bps @ 31.981568 MHz */
            break;
        case 57600:
            uart_params.clockPrescalar      = 34;   /* 57600 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 11;   /* 57600 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 57600 bps @ 31.981568 MHz */
            break;
        case 115200:
            uart_params.clockPrescalar      = 17;   /* 115200 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 6;    /* 115200 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 115200 bps @ 31.981568 MHz */
            break;
        case 230400:
            uart_params.clockPrescalar      = 8;    /* 230400 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 11;   /* 230400 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 5;    /* 230400 bps @ 31.981568 MHz */
            break;
        case 460800:
            uart_params.clockPrescalar      = 4;    /* 460800 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 3;    /* 460800 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 5;    /* 460800 bps @ 31.981568 MHz */
            break;
        default:
        #if defined ( CONFIG_DRIVERS_DEBUG_ENABLED ) && ( CONFIG_DRIVERS_DEBUG_ENABLED == 1 )
            sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during the initialization: Invalid baudrate!");
            sys_log_new_line();
        #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
            err = -1;      /* Invalid baudrate value */
            break;
    }

    uart_params.selectClockSource   = USCI_A_UART_CLOCKSOURCE_SMCLK;
    uart_params.parity              = config.parity;
    uart_params.msborLsbFirst       = USCI_A_UART_LSB_FIRST;
    uart_params.numberofStopBits    = config.stop_bits;
    uart_params.uartMode            = USCI_A_UART_MODE;
    uart_params.overSampling        = USCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;

    uint16_t base_address;

    switch(port)
    {
        case UART_PORT_0:
            base_address = USCI_A0_BASE;
            GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN4 + GPIO_PIN5);
            break;
        case UART_PORT_1:
            base_address = USCI_A1_BASE;
            GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN2 + GPIO_PIN3);
            break;
        case UART_PORT_2:
            base_address = USCI_A2_BASE;
            GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P9, GPIO_PIN2 + GPIO_PIN3);
            break;
        default:
            #if defined ( CONFIG_DRIVERS_DEBUG_ENABLED ) && ( CONFIG_DRIVERS_DEBUG_ENABLED == 1 )
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during the initialization: Invalid port!");
                sys_log_new_line();
            #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
            err = -1;      /* Invalid port */
            break;
    }

    if (USCI_A_UART_init(base_address, &uart_params) != STATUS_SUCCESS)
    {
        #if defined ( CONFIG_DRIVERS_DEBUG_ENABLED ) && ( CONFIG_DRIVERS_DEBUG_ENABLED == 1 )
            sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during the initialization!");
            sys_log_new_line();
        #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
        err = -1;
    }

    return err;
}

int uart_interrupt_enable(uart_interrupt_port_t port)
{
    int err = 0;
    uint16_t base_address;
    switch (port)
    {
        case UART_PORT_0:
            base_address = USCI_A0_BASE;
            break;
        case UART_PORT_1:
            base_address = USCI_A1_BASE;
            break;
        case UART_PORT_2:
            base_address = USCI_A2_BASE;
            break;
        default:
            #if defined ( CONFIG_DRIVERS_DEBUG_ENABLED ) && ( CONFIG_DRIVERS_DEBUG_ENABLED == 1 )
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Invalid port!");
                sys_log_new_line();
            #endif             /* CONFIG_DRIVERS_DEBUG_ENABLED */
            err = -1; /* Invalid UART port */
            break;
    }

    USCI_A_UART_enable(base_address);
    USCI_A_UART_clearInterrupt(base_address, 
        USCI_A_UART_RECEIVE_INTERRUPT +
        USCI_A_UART_BREAKCHAR_INTERRUPT
    );
    USCI_A_UART_enableInterrupt(base_address, 
        USCI_A_UART_RECEIVE_INTERRUPT +
        USCI_A_UART_BREAKCHAR_INTERRUPT
    );

    return err;
};

int uart_interrupt_disable(uart_interrupt_port_t port)
{
    int err = 0;
    uint16_t base_address;
    switch (port)
    {
        case UART_PORT_0:
            base_address = USCI_A0_BASE;
            break;
        case UART_PORT_1:
            base_address = USCI_A1_BASE;
            break;
        case UART_PORT_2:
            base_address = USCI_A2_BASE;
            break;
        default:
            #if defined ( CONFIG_DRIVERS_DEBUG_ENABLED ) && ( CONFIG_DRIVERS_DEBUG_ENABLED == 1 )
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Invalid port!");
                sys_log_new_line();
            #endif             /* CONFIG_DRIVERS_DEBUG_ENABLED */
            err = -1; /* Invalid UART port */
            break;
    }

    USCI_A_UART_disable(base_address);
    USCI_A_UART_disableInterrupt(base_address,
        USCI_A_UART_RECEIVE_INTERRUPT +
        USCI_A_UART_BREAKCHAR_INTERRUPT
    );

    return err;
};

int uart_interrupt_write(uart_interrupt_port_t port, uint8_t *data, uint16_t len)
{
    int err = 0;
    uint16_t base_address;

    switch(port)
    {
        case UART_PORT_0:   
            base_address = USCI_A0_BASE;    
            break;
        case UART_PORT_1:   
            base_address = USCI_A1_BASE;    
            break;
        case UART_PORT_2:   
            base_address = USCI_A2_BASE;    
            break;
        default:
            #if defined ( CONFIG_DRIVERS_DEBUG_ENABLED ) && ( CONFIG_DRIVERS_DEBUG_ENABLED == 1 )
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during writing: Invalid port!");
                sys_log_new_line();
            #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
            err = -1;
            break;
    }

    uint16_t i = 0;
    for(i=0; i<len; i++)
    {
        USCI_A_UART_transmitData(base_address, data[i]);
    }

    //USCI_A_UART_transmitBreak(base_address);

    return err;
}

int uart_interrupt_read(uart_interrupt_port_t port, uint8_t *data, uint16_t *len)
{
    int err = 0;
    uint16_t base_address;

    switch(port)
    {
        case UART_PORT_0:   
            base_address = USCI_A0_BASE;    
            break;
        case UART_PORT_1:   
            base_address = USCI_A1_BASE;    
            break;
        case UART_PORT_2:   
            base_address = USCI_A2_BASE;    
            break;
        default:
            #if defined ( CONFIG_DRIVERS_DEBUG_ENABLED ) && ( CONFIG_DRIVERS_DEBUG_ENABLED == 1 )
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during writing: Invalid port!");
                sys_log_new_line();
            #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
            err = -1;
            break;
    }

    if (err == 0)
    {
        (void) memcpy(data, uart_rx_buffer, uart_rx_data_size);

        *len = uart_rx_data_size;
    }

    return err;
}

/**
 * \brief UART interrupt service routine (interrupt handler). Implemented only for USCI_A0.
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR(void)
{
    uint8_t uart_rx_buffer_index = 0;
    switch(__even_in_range(UCA0IV,4))
    {
        case UART_RECEIVE_INTERRUPT_FLAG:
            uart_rx_buffer[uart_rx_buffer_index] = UCA0RXBUF;

            if( ( uart_rx_buffer_index++ > WRITE_COMMAND_MAX_SIZE ) && ( uart_rx_buffer[0] == WRITE_COMMAND_ID ) )
            {    
                uart_rx_buffer_index = 0;
                uart_rx_data_size = 7;
                uart_interrupt_notify_from_rcv_isr();
            }

            if( ( uart_rx_buffer_index++ > READ_COMMAND_MAX_SIZE ) && ( uart_rx_buffer[0] == READ_COMMAND_ID ) )
            {    
                uart_rx_buffer_index = 0;
                uart_rx_data_size = 4;
                uart_interrupt_notify_from_rcv_isr();
            }
            break;
        default: 
            break;  
    }
}

__attribute__((weak)) void uart_interrupt_notify_from_rcv_isr(void)
{
    #if defined ( CONFIG_DRIVERS_DEBUG_ENABLED ) && ( CONFIG_DRIVERS_DEBUG_ENABLED == 1 )
        sys_log_print_event_from_module(SYS_LOG_INFO, UART_INTERRUPT_MODULE_NAME, "Notified to uart rcv handler");
        sys_log_new_line();
    #endif
}

/** \} End of uart group */
