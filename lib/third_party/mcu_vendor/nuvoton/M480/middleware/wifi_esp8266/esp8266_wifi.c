/**************************************************************************//**
 * @file     esp8266_wifi.c
 * @version  V1.00
 * @brief    M480 series ESP8266 WiFi driver source file
 *
 * @copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "stdlib.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Socket and Wi-Fi interface includes. */
#include "aws_wifi.h"

/* Wi-Fi driver includes. */
#include "esp8266_wifi.h"

ESP_WIFI_IPD_t xWifiIpd;

/* Uart rx buffer control */
#define RX_BUF_SIZE         2048
#define RX_BUF_RESET()      (rx_buf_ridx = rx_buf_widx = 0)

static uint8_t rx_buf[RX_BUF_SIZE];
static uint16_t rx_buf_ridx, rx_buf_widx;

void RX_BUF_PUSH(uint8_t d)
{
    rx_buf[rx_buf_widx++] = d;
    if (rx_buf_widx == RX_BUF_SIZE) {
        rx_buf_widx = 0;
    }
}

uint8_t RX_BUF_POP(void)
{
    uint8_t d = rx_buf[(rx_buf_ridx++)];

    if (rx_buf_ridx == RX_BUF_SIZE) {
        rx_buf_ridx = 0;
    }

    return d;
}

uint8_t RX_BUF_FULL(void)
{
    if ((rx_buf_widx == (RX_BUF_SIZE - 1)) && (rx_buf_ridx == 0)) {
        return 1;
    }

    if (rx_buf_ridx == (rx_buf_widx + 1)) {
        return 1;
    } else {
        return 0;
    }
}

uint16_t RX_BUF_COUNT(void)
{
    if (rx_buf_widx >= rx_buf_ridx) {
        return (rx_buf_widx - rx_buf_ridx);
    } else {
        return (RX_BUF_SIZE + rx_buf_widx - rx_buf_ridx);
    }
}


/**
 * @brief  Parse an IP address
 */
static void ParseIpAddr( char * string, uint8_t * addr )
{
    char *pcDelim = "\",.";
    char *pcPtr;
    uint8_t ucCount;

    pcPtr = strtok(string, pcDelim);

    for (ucCount = 0; ucCount < 4; ucCount++) {
        addr[ucCount] = atoi(pcPtr);
        pcPtr = strtok(NULL, pcDelim);
    }
}

/**
 * @brief  Parse an MAC address
 */
static void ParseMacAddr( char * string, uint8_t * addr )
{
    char *pcDelim = "\",:";
    char *pcPtr;
    uint8_t ucCount;

    pcPtr = strtok(string, pcDelim);

    for (ucCount = 0; ucCount < 6; ucCount++) {
        addr[ucCount] = strtol(pcPtr, NULL, 16);
        pcPtr = strtok(NULL, pcDelim);
    }
}

/**
 * @brief  Parse an Access Point information
 */
static void ParseApDetail( char * string, WIFIScanResult_t xBuffer )
{
    char *pcDelim = "\",";
    char *pcPtr;
    uint8_t ucNum = 0;
    uint8_t ucMode;

    pcPtr = strtok(string, pcDelim);

    while (pcPtr != NULL) {
        switch (ucNum++) {
        case 1:
            ucMode = (uint8_t)atoi(pcPtr);
            if (ucMode == 4) {
                /* Does not support WPA_WPA2, set it as WPA2 mode */
                ucMode = 3;
            } else if (ucMode > 4) {
                /* Set as eWiFiSecurityNotSupported */
                ucMode = 4;
            }
            xBuffer.xSecurity = (WIFISecurity_t)ucMode;
            break;

        case 2:
            strlcpy(&xBuffer.cSSID[0], pcPtr, wificonfigMAX_SSID_LEN + 1);
            break;

        case 3:
            xBuffer.cRSSI = (int8_t)atoi(pcPtr);
            break;

        case 4:
            ParseMacAddr(pcPtr, &xBuffer.ucBSSID[0]);
            break;

        case 5:
            xBuffer.cChannel = (int8_t)atoi(pcPtr);
            break;

        default:
            break;
        }
        pcPtr = strtok(NULL, pcDelim);
    }
}

/**
 * @brief  Get device IP and MAC address
 * @param  Obj: pointer to WiF module
 * @retval None
 */
static void AT_ParseAddress( ESP_WIFI_Object_t * pxObj )
{
    char *pcDelim = ",\r\n";
    char *pcPtr;

    pcPtr = strtok((char *)pxObj->CmdData, pcDelim);

    while (pcPtr != NULL){
        if (strcmp(pcPtr, "+CIFSR:STAIP") == 0) {
            pcPtr = strtok(NULL, pcDelim);
            ParseIpAddr(pcPtr, pxObj->StaIpAddr);
        } else if (strcmp(pcPtr, "+CIFSR:STAMAC") == 0) {
            pcPtr = strtok(NULL, pcDelim);
            ParseMacAddr(pcPtr, pxObj->StaMacAddr);
        } else if (strcmp(pcPtr, "+CIFSR:APIP") == 0) {
            pcPtr = strtok(NULL, pcDelim);
            ParseIpAddr(pcPtr, pxObj->ApIpAddr);
        } else if (strcmp(pcPtr, "+CIFSR:APMAC") == 0) {
            pcPtr = strtok(NULL, pcDelim);
            ParseMacAddr(pcPtr, pxObj->ApMacAddr);
        }
        pcPtr = strtok(NULL, pcDelim);
    }
}

/**
 * @brief  Get scanned AP information
 */
static void AT_ParseAccessPoint( ESP_WIFI_Object_t * pxObj, WIFIScanResult_t * pxBuffer, uint8_t ucNumNetworks )
{
    char *pcDelim = "()\r\n";
    char *pcPtr;
    uint8_t ucCount;

    pcPtr = strtok((char *)pxObj->CmdData, pcDelim);

    for (ucCount = 0; ucCount < ucNumNetworks; ucCount++) {
        while (pcPtr != NULL){
            if (strcmp(pcPtr, "+CWLAP:") == 0) {
                pcPtr = strtok(NULL, pcDelim);
                ParseApDetail(pcPtr, pxBuffer[ucCount]);
            }
            pcPtr = strtok(NULL, pcDelim);
        }
    }
}

/**
 * @brief  Initialize platform
 */
BaseType_t ESP_Platform_Init( ESP_WIFI_Object_t * pxObj )
{
    if (pxObj->Uart == (UART_T *)UART1) {
        CLK_EnableModuleClock(UART1_MODULE);
        /* Select UART1 clock source is HXT */
        CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART1SEL_HXT, CLK_CLKDIV0_UART1(1));
        /* Set PB multi-function pins for UART1 RXD, TXD */
        SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB3MFP_Msk | SYS_GPB_MFPL_PB2MFP_Msk);
        SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB3MFP_UART1_TXD | SYS_GPB_MFPL_PB2MFP_UART1_RXD);
    } else {
        configPRINTF(("Do not support MFP setting of UART_BASE 0x%p !\n", pxObj->Uart));
    }

    /* Configure UART */
    UART_Open(pxObj->Uart, pxObj->UartBaudRate);
    /* Set RX Trigger Level = 8 */
    pxObj->Uart->FIFO = (pxObj->Uart->FIFO &~ UART_FIFO_RFITL_Msk) | UART_FIFO_RFITL_8BYTES;
    /* Set Timeout time 0x3E bit-time */
    UART_SetTimeoutCnt(pxObj->Uart, 0x3E);
    /* enable uart */
    UART_EnableInt(pxObj->Uart, UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk | UART_INTEN_TOCNTEN_Msk);
    NVIC_EnableIRQ((IRQn_Type)pxObj->UartIrq);

    return pdTRUE;
}

/**
 * @brief  Write data to UART interface
 */
uint16_t ESP_IO_Send( ESP_WIFI_Object_t * pxObj, uint8_t pucTxBuf[], uint16_t usWriteBytes)
{
    Nuvoton_debug_printf(("[%s] \"%s\"\n", __func__, pucTxBuf));

    return UART_Write(pxObj->Uart, pucTxBuf, usWriteBytes);
}

/**
 * @brief  UART interrupt handler for RX
 */
void UART1_IRQHandler(void)
{
    uint8_t ucDat;

    while (!UART_GET_RX_EMPTY(UART1)) {
        if (RX_BUF_FULL()) {
            configPRINTF(("[%s] RX buffer full !!\n", __func__));
        }
        ucDat = UART_READ(UART1);
        RX_BUF_PUSH(ucDat);
    }
}

/**
 * @brief  Read data from UART interface
 */
ESP_WIFI_Status_t ESP_IO_Recv( ESP_WIFI_Object_t * pxObj, uint8_t pucRxBuf[], uint16_t usReadBytes, TickType_t xTimeout )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_TIMEOUT;
    TickType_t xTickTimeout;
    uint8_t ucRecvHeader[16];
    char *pcDelim = ",:";
    char *pcPtr;
    char *pcRecv;
    uint8_t ucExit = 0;
    uint16_t usCount;
    uint16_t usIpdLength;

    /* Set timeout ticks */
    xTickTimeout = xTaskGetTickCount() + xTimeout;

    /* Clear the receive buffer */
    pcPtr = (char *)(pucRxBuf);
    memset(pcPtr, 0x0, usReadBytes);

    for (usCount = 0; usCount < usReadBytes; usCount++) {
        /* Check RX empty => failed */
        while (RX_BUF_COUNT() == 0) {
            if (xTaskGetTickCount() > xTickTimeout) {
                configPRINTF(("ERROR: [%s] Reach the timeout %d !!\n", __func__, xTimeout));
                ucExit = 1;
                break;
            }
        }
        if (ucExit == 1) {
            break;
        }

        /* Get data from RX buffer */
        pucRxBuf[usCount] = RX_BUF_POP();

        /* Check the end of the message to reduce the response time */
        switch (pucRxBuf[usCount]) {
        case '\n':
            if (strstr(pcPtr, AT_OK_STRING)) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_OK;
            } else if ((strstr(pcPtr, AT_FAIL_STRING)) || (strstr(pcPtr, AT_ERROR_STRING))) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_ERROR;
            }
            break;

        case '>':
            if (strstr(pcPtr, AT_SEND_STRING) == pcPtr) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_SEND;
            }
            break;

        case ':':
            /* Check if meet the receive data format */
            pcRecv = strstr(pcPtr, AT_RECV_STRING);
            if (pcRecv) {
                /* Clear the receive buffer */
                memcpy(ucRecvHeader, pcRecv, sizeof(ucRecvHeader));
                pcRecv = (char *)(ucRecvHeader);

                pcPtr = strstr(pcRecv, AT_RECV_STRING);
                pcPtr = strtok(pcPtr, pcDelim);
                /* Check multiple connection */
                if (pxObj->IsMultiConn == pdTRUE) {
                    pcPtr = strtok(NULL, pcDelim);
                }
                pcPtr = strtok(NULL, pcDelim);
                usIpdLength = (uint16_t)atoi(pcPtr);
                if (pxObj->ActiveCmd != CMD_RECV) {
                    /* Current active command is not the receive, we need to store the IPD data */
                    for (; usIpdLength > 0; usIpdLength--) {
                        while (RX_BUF_COUNT() == 0) ;
                        xWifiIpd.Data[xWifiIpd.DataLength++] = RX_BUF_POP();
                    }
                    ucExit = 1;
                } else {
                    usReadBytes = usCount + usIpdLength + 1;

                }
                xRet = ESP_WIFI_STATUS_RECV;
            }
            break;

        default:
            break;
        }
        if (ucExit == 1) {
            break;
        }
    }
    Nuvoton_debug_printf(("[%s] \"%s\", usCount = %d, xRet = %d\n", __func__, pucRxBuf, usCount, xRet));

    return xRet;
}

/**
 * @brief  Execute AT command
 * @param  pxObj: pointer to WiF module
 * @param  pucCmd: pointer to AT command string
 * @param  ulTimeWaitMs: command wait time in ms
 * @retval Operation status
 */
static ESP_WIFI_Status_t ESP_AT_Command( ESP_WIFI_Object_t * pxObj, uint8_t * pucCmd, uint32_t ulTimeWaitMs )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_TIMEOUT;
    uint16_t usCount;

    /* Wait for last AT command is done */
    while (xTaskGetTickCount() < pxObj->AvailableTick) {
        taskYIELD();
    }

    if (ESP_IO_Send(pxObj, pucCmd, strlen((char *)pucCmd)) > 0) {
        xRet = ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), pxObj->Timeout);

        if (ulTimeWaitMs > 0) {
            /* Set the time that next AT command is workable */
            pxObj->AvailableTick = xTaskGetTickCount() + pdMS_TO_TICKS(ulTimeWaitMs);
        }
    }
    //Nuvoton_debug_printf(("[%s] xRet = %d\n", __func__, xRet));

    return xRet;
}

/**
 * @brief  Initialize WIFI module
 * @param  Obj: pointer to WiF module
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Init( ESP_WIFI_Object_t * pxObj )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_ERROR;

    if (ESP_Platform_Init(pxObj) == pdTRUE) {
        //xRet = ESP_WIFI_Reset(pxObj);
        xRet = ESP_WIFI_STATUS_OK;
    }

    return xRet;
}

/**
 * @brief  Connects to an AP
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Connect( ESP_WIFI_Object_t * pxObj, const char * cSSID, const char * cPassword )
{
    ESP_WIFI_Status_t xRet;

    /* Set WiFi to STA mode */
    xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT+CWMODE=1\r\n", 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        configPRINTF(("Try to connect to AP \"%s\" ...\n", cSSID));
        sprintf((char *)pxObj->CmdData, "AT+CWJAP=\"%s\",\"%s\"\r\n", cSSID, cPassword);
        xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);
        if (xRet == ESP_WIFI_STATUS_OK) {
            pxObj->IsConnected = pdTRUE;
        }
    }

    return xRet;
}

/**
 * @brief  Disconnects from the AP
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Disconnect( ESP_WIFI_Object_t * pxObj )
{
    return ESP_AT_Command(pxObj, (uint8_t *)"AT+CWQAP\r\n", 0);
}

/**
 * @brief  Restarts the module
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Reset( ESP_WIFI_Object_t * pxObj )
{
    return ESP_AT_Command(pxObj, (uint8_t *)"AT+RST\r\n", 1000);
}

/**
 * @brief  Lists Available APs
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Scan( ESP_WIFI_Object_t * pxObj, WIFIScanResult_t * pxBuffer, uint8_t ucNumNetworks )
{
    ESP_WIFI_Status_t xRet;

    xRet = ESP_AT_Command(pxObj, "AT+CWLAP\r\n", 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        AT_ParseAccessPoint(pxObj, pxBuffer, ucNumNetworks);
    }

    return xRet;
}

/**
 * @brief  Check the module is connected to an AP
 */
BaseType_t ESP_WIFI_IsConnected( ESP_WIFI_Object_t * pxObj )
{
    return pxObj->IsConnected;
}

/**
 * @brief  Get network status
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_GetNetStatus( ESP_WIFI_Object_t * pxObj )
{
    ESP_WIFI_Status_t xRet;

    xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT+CIFSR\r\n", 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        AT_ParseAddress(pxObj);
    }

    return xRet;
}

/**
 * @brief  Get the IP address from a host name
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_GetHostIP( ESP_WIFI_Object_t * pxObj, char * pcHost, uint8_t * pucIPAddr )
{
    ESP_WIFI_Status_t xRet;
    char *pcPtr;

    sprintf((char *)pxObj->CmdData, "AT+CIPDOMAIN=\"%s\"\r\n", pcHost);
    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        xRet = ESP_WIFI_STATUS_ERROR;

        pcPtr = strtok((char *)pxObj->CmdData, ":\n");
        while (pcPtr != NULL){
            if (strcmp(pcPtr, "+CIPDOMAIN") == 0) {
                pcPtr = strtok(NULL, ":\n");
                ParseIpAddr(pcPtr, pucIPAddr);
                xRet = ESP_WIFI_STATUS_OK;
                break;
            }
        }
    }

    return xRet;
}

/**
 * @brief  Enable or Disable Multiple Connections
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_SetMultiConn( ESP_WIFI_Object_t * pxObj, uint8_t mode )
{
    ESP_WIFI_Status_t xRet;

    sprintf((char *)pxObj->CmdData, "AT+CIPMUX=%d\r\n", mode);
    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        pxObj->IsMultiConn = (mode) ? pdTRUE : pdFALSE;
    }

    return xRet;
}

/**
 * @brief  Establishes TCP Connection, UDP Transmission or SSL Connection
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_StartClient( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn )
{
    ESP_WIFI_Status_t xRet;
    uint8_t ucCount;

    /* Set multiple connection */
    if (pxObj->IsMultiConn == pdTRUE) {
        sprintf((char *)pxObj->CmdData, "AT+CIPSTART=%d,", pxConn->LinkID);
    } else {
        sprintf((char *)pxObj->CmdData, "AT+CIPSTART=");
    }

    /* Set connection type */
    switch (pxConn->Type) {
    case 0:
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"TCP\",");
        break;
    case 1:
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"UDP\",");
        break;
    case 2:
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"SSL\",");
        break;
    }

    /* Set remote IP address and port */
    sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"%d", pxConn->RemoteIP[0]);
    for (ucCount = 1; ucCount < 4; ucCount++) {
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), ".%d", pxConn->RemoteIP[ucCount]);
    }
    sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\",%d", pxConn->RemotePort);

    /* Set remaining data */
    if (pxConn->Type == ESP_WIFI_UDP) {
        if (pxConn->UdpLocalPort > 0) {
            sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), ",%d,%d", pxConn->UdpLocalPort, pxConn->UdpMode);
        }
    } else {
        if (pxConn->TcpKeepAlive > 0) {
            sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), ",%d", pxConn->TcpKeepAlive);
        }
    }

    sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\r\n");
    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

    if (xRet != ESP_WIFI_STATUS_OK) {
        configPRINTF(("%s returns %d failed !!\n", __func__, xRet));
    }

    return xRet;
}

/**
 * @brief  Closes the TCP/UDP/SSL Connection
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_StopClient( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn )
{
    if (pxObj->IsMultiConn == pdTRUE) {
        sprintf((char *)pxObj->CmdData, "AT+CIPCLOSE=%d\r\n", pxConn->LinkID);
    } else {
        sprintf((char *)pxObj->CmdData, "AT+CIPCLOSE\r\n");
    }

    return ESP_AT_Command(pxObj, pxObj->CmdData, 0);
}

/**
 * @brief  Sends Data
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Send( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn, uint8_t * pcBuf, 
                                 uint16_t usReqLen, uint16_t * usSendLen, TickType_t xTimeout )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_ERROR;
    TickType_t xTickCurrent, xTickEnd, xTickTimeout;
    uint16_t usAskSend, usRealSend;
    uint8_t ucExit = 0;
    uint8_t ucCount;

    xTickEnd = xTaskGetTickCount() + xTimeout;

    if (pcBuf == NULL) {
        return xRet;
    } else {
        pcBuf[usReqLen] = 0;
    }

    (*usSendLen) = 0;
    while ((*usSendLen) < usReqLen) {
        usAskSend = usReqLen - (*usSendLen);
        if (usAskSend > ESP_WIFI_SEND_SIZE)
            usAskSend = ESP_WIFI_SEND_SIZE;

        /* Set multiple connection */
        if (pxObj->IsMultiConn == pdTRUE) {
            sprintf((char *)pxObj->CmdData, "AT+CIPSEND=%d,%d", pxConn->LinkID, usAskSend);
        } else {
            sprintf((char *)pxObj->CmdData, "AT+CIPSEND=%d", usAskSend);
        }

        /* Set remote IP and port for UDP connection */
        if ((pxConn->Type == ESP_WIFI_UDP) && (pxConn->UdpLocalPort > 0)) {
            sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"%d", pxConn->RemoteIP[0]);
            for (ucCount = 1; ucCount < 4; ucCount++) {
                sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), ".%d", pxConn->RemoteIP[ucCount]);
            }
            sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\",%d", pxConn->RemotePort);
        }

        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\r\n");
        /* Must wait a period of time after the CIPSEND AT command */
        xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 300);

        if (xRet == ESP_WIFI_STATUS_OK) {
            pxObj->ActiveCmd = CMD_SEND;
            xRet = ESP_WIFI_STATUS_ERROR;

            xTickCurrent = xTaskGetTickCount();
            if (xTickCurrent < xTickEnd) {
                xTickTimeout = xTickEnd - xTickCurrent;
                if (ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), xTickTimeout) == ESP_WIFI_STATUS_SEND) {
                    /* WiFi module allows to send data */
                    usRealSend = ESP_IO_Send(pxObj, pcBuf, usAskSend);
                    if (usRealSend > 0) {
                        do {
                            xTickCurrent = xTaskGetTickCount();
                            if (xTickCurrent >= xTickEnd) {
                                xRet = ESP_WIFI_STATUS_TIMEOUT;
                            } else {
                                xTickTimeout = xTickEnd - xTickCurrent;
                                xRet = ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), xTickTimeout);
                            }
                        } while (xRet == ESP_WIFI_STATUS_RECV);
                        (*usSendLen) += usRealSend;
                        pxObj->ActiveCmd = CMD_NONE;
                    }
                }
            } else {
                xRet = ESP_WIFI_STATUS_TIMEOUT;
            }
        }
    }

    return xRet;
}

/**
 * @brief  Receive Data
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Recv( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn, uint8_t * pcBuf, 
                                 uint16_t usReqLen, uint16_t * usRecvLen, TickType_t xTimeout )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_ERROR;
    TickType_t xTickCurrent, xTickEnd, xTickTimeout;
    char *pcDelim = ",:";
    char *pcPtr;
    uint8_t ucLinkID;
    uint16_t usCount;

    xTickEnd = xTaskGetTickCount() + xTimeout;

    if (pcBuf == NULL) {
        return xRet;
    }

    pxObj->ActiveCmd = CMD_RECV;
    (*usRecvLen) = 0;
    if (xWifiIpd.DataLength) {
        memcpy(pcBuf, xWifiIpd.Data, xWifiIpd.DataLength);
        (*usRecvLen) = xWifiIpd.DataLength;

        /* Reset the IPD content */
        xWifiIpd.DataLength = 0;
        memset(xWifiIpd.Data, 0x0, sizeof(xWifiIpd.Data));
    }

    while ((*usRecvLen) < usReqLen) {
        xTickCurrent = xTaskGetTickCount();
        if (xTickCurrent < xTickEnd) {
            xTickTimeout = xTickEnd - xTickCurrent;
        } else {
            break;
        }

        xRet = ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), xTickTimeout);
        if (xRet == ESP_WIFI_STATUS_RECV) {
            pcPtr = strstr((char *)(pxObj->CmdData), AT_RECV_STRING);
            if (pcPtr) {
                /* Meet the receive data format */
                pcPtr = strtok(pcPtr, pcDelim);

                if (pxObj->IsMultiConn == pdTRUE) {
                    /* Get the Link ID */
                    pcPtr = strtok(NULL, pcDelim);
                    ucLinkID = (int8_t)atoi(pcPtr);
                    if (ucLinkID != pxConn->LinkID) {
                        xRet = ESP_WIFI_STATUS_OK;
                        /* Receive message from wrong Link ID */
                        configPRINTF(("ERROR: Requested ID is %d, but received ID is %d !\n", pxConn->LinkID, ucLinkID));
                        return xRet;
                    }
                }
                /* Get the receive length */
                pcPtr = strtok(NULL, pcDelim);
                usCount += (uint16_t)atoi(pcPtr);

                /* copy the receive data */
                pcPtr = strtok(NULL, pcDelim);
                memcpy(pcBuf + (*usRecvLen), pcPtr, usCount);
                (*usRecvLen) += usCount;
                xRet = ESP_WIFI_STATUS_OK;
            }
        }
    }
    pxObj->ActiveCmd = CMD_NONE;

    return xRet;
}
