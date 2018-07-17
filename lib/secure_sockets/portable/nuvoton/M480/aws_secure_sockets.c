/*
 * Amazon FreeRTOS Secure Socket V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_secure_sockets.c
 * @brief WiFi and Secure Socket interface implementation.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Socket and WiFi interface includes. */
#include "aws_secure_sockets.h"

/* Wi-Fi driver includes. */
#include "esp8266_wifi.h"

/**
 * @brief A Flag to indicate whether or not a socket is secure 
 * i.e. it uses TLS or not.
 */
#define securesocketsSOCKET_SECURE_FLAG                 ( 1UL << 0 )

/**
 * @brief A flag to indicate whether or not a socket is closed for receive.
 */
#define securesocketsSOCKET_READ_CLOSED_FLAG            ( 1UL << 1 )

/**
 * @brief A flag to indicate whether or not a socket is closed for send.
 */
#define securesocketsSOCKET_WRITE_CLOSED_FLAG           ( 1UL << 2 )

/**
 * @brief The maximum timeout
 *
 */
#define securesocketsMAX_TIMEOUT                        ( 30000 )

/* WiFi module definition */
typedef struct NuWiFiModule
{
    ESP_WIFI_Object_t xWifiObject;
    SemaphoreHandle_t xWifiSem;
} NuWiFiModule_t;
extern NuWiFiModule_t xNuWiFi;

/* Secure socket definition */
typedef struct NuSecureSocket
{
    ESP_WIFI_Conn_t xWiFiConn;
    uint8_t ucInUse;
    uint32_t ulFlags;
    TickType_t xSendTimeout;
    TickType_t xRecvTimeout;
#if 0
    char * pcDestination;
    void * pvTLSContext;
    char * pcServerCertificate;
    uint32_t ulServerCertificateLength;
#endif
} NuSecureSocket_t;
static NuSecureSocket_t xSockets[wificonfigMAX_SOCKETS];

static SemaphoreHandle_t xSSocketSem = NULL;
static const TickType_t xSemaphoreWaitTicks = pdMS_TO_TICKS(wificonfigMAX_SEMAPHORE_WAIT_TIME_MS);
/*-----------------------------------------------------------*/

static uint32_t prvGetFreeSocket( void )
{
    uint32_t ulSocketNum = (uint32_t)SOCKETS_INVALID_SOCKET;

    if (xSemaphoreTake(xSSocketSem, xSemaphoreWaitTicks) == pdTRUE) {
        for (ulSocketNum = 0 ; ulSocketNum < wificonfigMAX_SOCKETS ; ulSocketNum++) {
            if (xSockets[ulSocketNum].ucInUse == 0) {
                xSockets[ulSocketNum].ucInUse = 1;
                break;
            }
        }
        xSemaphoreGive(xSSocketSem);
    }

    if (ulSocketNum == wificonfigMAX_SOCKETS) {
        ulSocketNum = (uint32_t)SOCKETS_INVALID_SOCKET;
    }

    return ulSocketNum;
}
/*-----------------------------------------------------------*/

static void prvReturnSocket( uint32_t ulSocketNum )
{
    if (xSemaphoreTake(xSSocketSem, xSemaphoreWaitTicks) == pdTRUE) {
        xSockets[ulSocketNum].ucInUse = 0;
        xSemaphoreGive(xSSocketSem);
    }
}
/*-----------------------------------------------------------*/

static BaseType_t prvIsValidSocket( uint32_t ulSocketNum )
{
    BaseType_t xSocketRet = pdFALSE;

    configASSERT(ulSocketNum < wificonfigMAX_SOCKETS);

    if (xSemaphoreTake(xSSocketSem, xSemaphoreWaitTicks) == pdTRUE) {
        /* Check that the provided socket number is in use or not */
        if (xSockets[ulSocketNum].ucInUse == 1) {
            xSocketRet = pdTRUE;
        }
        xSemaphoreGive(xSSocketSem);
    }

    return xSocketRet;
}
/*-----------------------------------------------------------*/

static BaseType_t prvNetworkSend( void * pvContext,
                                  const unsigned char * pucData,
                                  size_t xDataLength )
{
    ESP_WIFI_Status_t xWiFiRet;
    NuSecureSocket_t *pxSecureSocket;
    BaseType_t xSocketRet = SOCKETS_SOCKET_ERROR;
    uint16_t usSentBytes;
    uint16_t usTotalSent = 0;
    uint32_t ulSocketNum = (uint32_t)pvContext;

    /* Shortcut for socket access */
    pxSecureSocket = &xSockets[ulSocketNum];

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        /* Send the data */
        while (usTotalSent < xDataLength) {
            usSentBytes = 0;
            xWiFiRet = ESP_WIFI_Send(&xNuWiFi.xWifiObject, &(pxSecureSocket->xWiFiConn), (uint8_t *)(pucData+usTotalSent),
                                     (uint16_t)(xDataLength-usTotalSent), &usSentBytes, pxSecureSocket->xSendTimeout);

            if (xWiFiRet == ESP_WIFI_STATUS_OK) {
                usTotalSent += usSentBytes;
            } else {
                xSocketRet = xWiFiRet;
                break;
            }
        }
        if (xWiFiRet == ESP_WIFI_STATUS_OK) {
            xSocketRet = (BaseType_t)xDataLength;
        }

        xSemaphoreGive(xNuWiFi.xWifiSem);
    }
    //taskYIELD();

    return xSocketRet;
}
/*-----------------------------------------------------------*/

static BaseType_t prvNetworkRecv( void * pvContext,
                                  unsigned char * pucReceiveBuffer,
                                  size_t xReceiveBufferLength )
{
    ESP_WIFI_Status_t xWiFiRet;
    NuSecureSocket_t *pxSecureSocket;
    BaseType_t xSocketRet;
    TickType_t xTickTimeout;
    uint32_t ulSocketNum = (uint32_t)pvContext;
    uint16_t usReceivedBytes = 0;

    /* Shortcut for socket access */
    pxSecureSocket = &xSockets[ulSocketNum];

   xTickTimeout = xTaskGetTickCount() + pxSecureSocket->xRecvTimeout;

    for ( ; ; ) {
        if (xSemaphoreTake(xNuWiFi.xWifiSem, xTickTimeout) == pdTRUE) {
            /* Receive the data */
            xWiFiRet = ESP_WIFI_Recv(&xNuWiFi.xWifiObject, &(pxSecureSocket->xWiFiConn), (uint8_t *)pucReceiveBuffer,
                                     (uint16_t)xReceiveBufferLength, &usReceivedBytes, pxSecureSocket->xRecvTimeout );

            xSemaphoreGive(xNuWiFi.xWifiSem);

            if ((xWiFiRet == ESP_WIFI_STATUS_OK) && (usReceivedBytes != 0)) {
                /* Success, return the number of bytes received */
                xSocketRet = (BaseType_t)usReceivedBytes;
                break;
            } else if ((xWiFiRet == ESP_WIFI_STATUS_TIMEOUT) || (usReceivedBytes == 0)) {
                /* Does not receive any data, retry it if still have time */
                if (xTaskGetTickCount() < xTickTimeout) {
                    vTaskDelay(1);
                } else {
                    /* Returning SOCKETS_EWOULDBLOCK will cause mBedTLS to fail
                     * and so we must return zero */
                    xSocketRet = 0;
                    break;
                }
            } else {
                xSocketRet = SOCKETS_SOCKET_ERROR;
                break;
            }
        } else {
            xSocketRet = 0;
            break;
        }
    }

    return xSocketRet;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    uint32_t ulSocketNum;

    configASSERT(lDomain == SOCKETS_AF_INET);
    configASSERT(lType == SOCKETS_SOCK_STREAM);
    configASSERT(lProtocol == SOCKETS_IPPROTO_TCP);

    /* Get a free socket */
    ulSocketNum = prvGetFreeSocket();
    if (ulSocketNum != ( uint32_t )SOCKETS_INVALID_SOCKET) {
        /* Initialize the members of xSockets */
        xSockets[ulSocketNum].xWiFiConn.LinkID = (uint8_t)ulSocketNum;
        xSockets[ulSocketNum].xWiFiConn.Type = ESP_WIFI_TCP;
        xSockets[ulSocketNum].ulFlags = 0;
        xSockets[ulSocketNum].xSendTimeout = pdMS_TO_TICKS(socketsconfigDEFAULT_SEND_TIMEOUT);
        xSockets[ulSocketNum].xRecvTimeout = pdMS_TO_TICKS(socketsconfigDEFAULT_RECV_TIMEOUT);
#if 0
        xSockets[ulSocketNum].pcDestination = NULL;
        xSockets[ulSocketNum].pvTLSContext = NULL;
        xSockets[ulSocketNum].pcServerCertificate = NULL;
        xSockets[ulSocketNum].ulServerCertificateLength = 0;
#endif
    }

    return (Socket_t)ulSocketNum;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Accept( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t * pxAddressLength )
{
    /* FIX ME. */
    return SOCKETS_INVALID_SOCKET;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    NuSecureSocket_t *pxSecureSocket;
    uint32_t ulSocketNum = (uint32_t)xSocket;
    int32_t lSocketRet = SOCKETS_SOCKET_ERROR;

    /* Check this is a valid socket */
    if (prvIsValidSocket(ulSocketNum) == pdTRUE) {
        pxSecureSocket = &xSockets[ulSocketNum];

        /* Set WiFi connection parameters */
        memcpy(&(pxSecureSocket->xWiFiConn.RemoteIP), &pxAddress->ulAddress, sizeof(uint32_t));
        pxSecureSocket->xWiFiConn.RemotePort = SOCKETS_ntohs(pxAddress->usPort);

        if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
            /* Start the client connection */
            if (ESP_WIFI_StartClient(&xNuWiFi.xWifiObject, &pxSecureSocket->xWiFiConn) == ESP_WIFI_STATUS_OK) {
                lSocketRet = SOCKETS_ERROR_NONE;
            }
            xSemaphoreGive(xNuWiFi.xWifiSem);
        }
    } else {
        configPRINTF(("ERROR: Invalid socket number !\n"));
        lSocketRet = SOCKETS_EINVAL;
    }

    /* Initialize TLS if necessary */
    if ((lSocketRet == SOCKETS_ERROR_NONE) && (pxSecureSocket->ulFlags & securesocketsSOCKET_SECURE_FLAG)) {
#if 0
        /* Setup TLS parameters. */
        xTLSParams.ulSize = sizeof( xTLSParams );
        xTLSParams.pcDestination = pxSecureSocket->pcDestination;
        xTLSParams.pcServerCertificate = pxSecureSocket->pcServerCertificate;
        xTLSParams.ulServerCertificateLength = pxSecureSocket->ulServerCertificateLength;
        xTLSParams.pvCallerContext = ( void * ) xSocket;
        xTLSParams.pxNetworkRecv = &( prvNetworkRecv );
        xTLSParams.pxNetworkSend = &( prvNetworkSend );

        /* Initialize TLS. */
        if( TLS_Init( &( pxSecureSocket->pvTLSContext ), &( xTLSParams ) ) == pdFREERTOS_ERRNO_NONE )
        {
            /* Initiate TLS handshake. */
            if( TLS_Connect( pxSecureSocket->pvTLSContext ) != pdFREERTOS_ERRNO_NONE )
            {
                /* TLS handshake failed. */
                lRetVal = SOCKETS_TLS_HANDSHAKE_ERROR;
            }
        }
        else
        {
            /* TLS Initialization failed. */
            lRetVal = SOCKETS_TLS_INIT_ERROR;
        }
#endif
    }

    return lSocketRet;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    NuSecureSocket_t *pxSecureSocket;
    uint32_t ulSocketNum = (uint32_t)xSocket;
    int32_t lSocketRet = SOCKETS_SOCKET_ERROR;

    configASSERT((pvBuffer != NULL));

    /* Check this is a valid socket */
    if (prvIsValidSocket(ulSocketNum) == pdTRUE) {
        pxSecureSocket = &xSockets[ulSocketNum];

        /* Check that send is allowed on the socket. */
        if ((pxSecureSocket->ulFlags & securesocketsSOCKET_READ_CLOSED_FLAG) == 0) {
            if ((pxSecureSocket->ulFlags & securesocketsSOCKET_SECURE_FLAG)) {
#if 0
                /* Receive through TLS pipe, if negotiated. */
                lSocketRet = TLS_Recv( pxSecureSocket->pvTLSContext, pvBuffer, xBufferLength );

                /* Convert the error code. */
                if( lSocketRet < 0 )
                {
                    /* TLS_Recv failed. */
                    lSocketRet = SOCKETS_TLS_RECV_ERROR;
                }
#endif
            } else {
                /* Receive non-secure data */
                lSocketRet = prvNetworkRecv(xSocket, pvBuffer, xBufferLength);
            }
        } else {
            /* The socket has been closed for write */
            lSocketRet = SOCKETS_ECLOSED;
        }
    } else {
        configPRINTF(("ERROR: Invalid socket number !\n"));
        lSocketRet = SOCKETS_EINVAL;
    }

    return lSocketRet;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    NuSecureSocket_t *pxSecureSocket;
    uint32_t ulSocketNum = (uint32_t)xSocket;
    int32_t lSocketRet = SOCKETS_SOCKET_ERROR;

    configASSERT((pvBuffer != NULL));

    /* Check this is a valid socket */
    if (prvIsValidSocket(ulSocketNum) == pdTRUE) {
        pxSecureSocket = &xSockets[ulSocketNum];

        /* Check that send is allowed on the socket. */
        if ((pxSecureSocket->ulFlags & securesocketsSOCKET_WRITE_CLOSED_FLAG) == 0) {
            if ((pxSecureSocket->ulFlags & securesocketsSOCKET_SECURE_FLAG)) {
#if 0
                /* Send through TLS pipe, if negotiated. */
                lSocketRet = TLS_Send( pxSecureSocket->pvTLSContext, pvBuffer, xDataLength );

                /* Convert the error code. */
                if( lSocketRet < 0 )
                {
                    /* TLS_Send failed. */
                    lSocketRet = SOCKETS_TLS_SEND_ERROR;
                }
#endif
            } else {
                /* Send non-secure data */
                lSocketRet = prvNetworkSend(xSocket, pvBuffer, xDataLength);
            }
        } else {
            /* The socket has been closed for write */
            lSocketRet = SOCKETS_ECLOSED;
        }
    } else {
        configPRINTF(("ERROR: Invalid socket number !\n"));
        lSocketRet = SOCKETS_EINVAL;
    }

    return lSocketRet;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    NuSecureSocket_t *pxSecureSocket;
    uint32_t ulSocketNum = (uint32_t)xSocket;
    int32_t lSocketRet = SOCKETS_EINVAL;

    /* Check this is a valid socket */
    if (prvIsValidSocket(ulSocketNum) == pdTRUE) {
        pxSecureSocket = &xSockets[ulSocketNum];

        switch (ulHow) {
        case SOCKETS_SHUT_RD:
            /* Further receive calls on this socket should return error. */
            pxSecureSocket->ulFlags |= securesocketsSOCKET_READ_CLOSED_FLAG;
            /* Return success to the user. */
            lSocketRet = SOCKETS_ERROR_NONE;
            break;

        case SOCKETS_SHUT_WR:
            /* Further send calls on this socket should return error. */
            pxSecureSocket->ulFlags |= securesocketsSOCKET_WRITE_CLOSED_FLAG;
            /* Return success to the user. */
            lSocketRet = SOCKETS_ERROR_NONE;
            break;

        case SOCKETS_SHUT_RDWR:
            /* Further send or receive calls on this socket should return error. */
            pxSecureSocket->ulFlags |= securesocketsSOCKET_READ_CLOSED_FLAG;
            pxSecureSocket->ulFlags |= securesocketsSOCKET_WRITE_CLOSED_FLAG;
            /* Return success to the user. */
            lSocketRet = SOCKETS_ERROR_NONE;
            break;

        default:
            /* An invalid value was passed for ulHow. */
            break;
        }
    } else {
        configPRINTF(("ERROR: Invalid socket number !\n"));
    }

    return lSocketRet;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Close( Socket_t xSocket )
{
    NuSecureSocket_t *pxSecureSocket;
    uint32_t ulSocketNum = (uint32_t)xSocket;
    int32_t lSocketRet = SOCKETS_SOCKET_ERROR;

    /* Check this is a valid socket */
    if (prvIsValidSocket(ulSocketNum) == pdTRUE) {
        pxSecureSocket = &xSockets[ulSocketNum];

        /* Mark the socket as closed */
        pxSecureSocket->ulFlags |= securesocketsSOCKET_READ_CLOSED_FLAG;
        pxSecureSocket->ulFlags |= securesocketsSOCKET_WRITE_CLOSED_FLAG;

#if 0
        /* Free the space allocated for pcDestination. */
        if( pxSecureSocket->pcDestination != NULL )
        {
            vPortFree( pxSecureSocket->pcDestination );
        }
        /* Free the space allocated for pcServerCertificate. */
        if( pxSecureSocket->pcServerCertificate != NULL )
        {
            vPortFree( pxSecureSocket->pcServerCertificate );
        }
        /* Cleanup TLS. */
        if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_SECURE_FLAG ) != 0UL )
        {
            TLS_Cleanup( pxSecureSocket->pvTLSContext );
        }
#endif

        pxSecureSocket->xWiFiConn.LinkID = (uint8_t)ulSocketNum;
        if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
            /* Stop the client connection */
            if (ESP_WIFI_StopClient(&xNuWiFi.xWifiObject, &pxSecureSocket->xWiFiConn) == ESP_WIFI_STATUS_OK) {
                /* Return the socket */
                prvReturnSocket(ulSocketNum);

                lSocketRet = SOCKETS_ERROR_NONE;
            }
            xSemaphoreGive(xNuWiFi.xWifiSem);
        }
    } else {
        configPRINTF(("ERROR: Invalid socket number !\n"));
        lSocketRet = SOCKETS_EINVAL;
    }

    return lSocketRet;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    NuSecureSocket_t *pxSecureSocket;
    TickType_t xTimeout;
    uint32_t ulSocketNum = (uint32_t)xSocket;
    int32_t lSocketRet = SOCKETS_ERROR_NONE;

    /* Check this is a valid socket */
    if (prvIsValidSocket(ulSocketNum) == pdTRUE) {
        pxSecureSocket = &xSockets[ulSocketNum];

        switch (lOptionName) {
#if 0
        case SOCKETS_SO_SERVER_NAME_INDICATION:

            /* Non-NULL destination string indicates that SNI extension should
             * be used during TLS negotiation. */
            pxSecureSocket->pcDestination = ( char * ) pvPortMalloc( 1U + xOptionLength );

            if( pxSecureSocket->pcDestination == NULL )
            {
                lSocketRet = SOCKETS_ENOMEM;
            }
            else
            {
                memcpy( pxSecureSocket->pcDestination, pvOptionValue, xOptionLength );
                pxSecureSocket->pcDestination[ xOptionLength ] = '\0';
            }

            break;

        case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:

            /* Non-NULL server certificate field indicates that the default trust
             * list should not be used. */
            pxSecureSocket->pcServerCertificate = ( char * ) pvPortMalloc( xOptionLength );

            if( pxSecureSocket->pcServerCertificate == NULL )
            {
                lSocketRet = SOCKETS_ENOMEM;
            }
            else
            {
                memcpy( pxSecureSocket->pcServerCertificate, pvOptionValue, xOptionLength );
                pxSecureSocket->ulServerCertificateLength = xOptionLength;
            }

            break;

        case SOCKETS_SO_REQUIRE_TLS:

            /* Turn on the secure socket flag to indicate that
             * TLS should be used. */
            pxSecureSocket->ulFlags |= stsecuresocketsSOCKET_SECURE_FLAG;
            break;

#endif
        case SOCKETS_SO_SNDTIMEO:
        case SOCKETS_SO_RCVTIMEO:
            /* pvOptionValue passed should be of TickType_t */
            xTimeout = *((const TickType_t *)pvOptionValue);

            if (xTimeout < securesocketsMAX_TIMEOUT) {
                if (lOptionName == SOCKETS_SO_SNDTIMEO) {
//                    pxSecureSocket->xSendTimeout = xTimeout;
                } else {
//                    pxSecureSocket->xRecvTimeout = xTimeout;
                }
            } else {
                lSocketRet = SOCKETS_EINVAL;
            }
            break;

        case SOCKETS_SO_NONBLOCK:
            /* Set timeout to be zero */
            pxSecureSocket->xSendTimeout = 1;
            pxSecureSocket->xRecvTimeout = 1;
            break;

        default:
            lSocketRet = SOCKETS_ENOPROTOOPT;
            break;
        }
    } else {
        configPRINTF(("ERROR: Invalid socket number !\n"));
        lSocketRet = SOCKETS_EINVAL;
    }

    return lSocketRet;
}
/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    uint32_t ulIpAddr = 0;

    configASSERT((pcHostName != NULL));

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        if (ESP_WIFI_GetHostIP(&xNuWiFi.xWifiObject, (char *)pcHostName, (uint8_t *)&(ulIpAddr)) == ESP_WIFI_STATUS_OK) {
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    }
    return ulIpAddr;
}
/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
    BaseType_t xSocketRet = pdFAIL;
    uint32_t ulSocketNum;

    xSSocketSem = xSemaphoreCreateMutex();
    if (xSSocketSem != NULL ) {
        xSocketRet = pdPASS;
    } else {
        configPRINTF(("ERROR: Failed to create the xSSocketSem mutex !\n" ));
        xSocketRet = pdFAIL;
    }

    for (ulSocketNum = 0 ; ulSocketNum < wificonfigMAX_SOCKETS ; ulSocketNum++) {
        xSockets[ulSocketNum].ucInUse = 0;
    }

    return xSocketRet;
}
/*-----------------------------------------------------------*/

uint32_t ulRand( void )
{
    /* FIX ME. */
    return 0;
}
/*-----------------------------------------------------------*/
