/*
 * Amazon FreeRTOS
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
#ifndef AWS_TEST_TCP_H
#define AWS_TEST_TCP_H

/* Non-Encrypted Echo Server.
 * Update tcptestECHO_SERVER_ADDR# and
 * tcptestECHO_PORT with IP address
 * and port of unencrypted TCP echo server. */
#define tcptestECHO_SERVER_ADDR0 10
#define tcptestECHO_SERVER_ADDR1 77
#define tcptestECHO_SERVER_ADDR2 116
#define tcptestECHO_SERVER_ADDR3 67
#define tcptestECHO_PORT  ( 1025 )

/* Encrypted Echo Server.
 * If tcptestSECURE_SERVER is set to 1, the following must be updated:
 * 1. aws_clientcredential.h to use a valid AWS endpoint.
 * 2. aws_clientcredential_keys.h with corresponding AWS keys.
 * 3. tcptestECHO_SERVER_TLS_ADDR0-3 with the IP address of an
 * echo server using TLS.
 * 4. tcptestECHO_PORT_TLS, with the port number of the echo server
 * using TLS.
 * 5. tcptestECHO_HOST_ROOT_CA with the trusted root certificate of the
 * echo server using TLS. */
#define tcptestSECURE_SERVER             1

#define tcptestECHO_SERVER_TLS_ADDR0 10
#define tcptestECHO_SERVER_TLS_ADDR1 77
#define tcptestECHO_SERVER_TLS_ADDR2 116
#define tcptestECHO_SERVER_TLS_ADDR3 67
#define tcptestECHO_PORT_TLS ( 33281 )

/* Number of times to retry a connection if it fails. */
#define tcptestRETRY_CONNECTION_TIMES    6

/* The root certificate used for the encrypted echo server.
 * This certificate is self-signed, and not in the trusted catalog. */
static const char tcptestECHO_HOST_ROOT_CA[] = "-----BEGIN CERTIFICATE-----\n"
"MIIDRDCCAiygAwIBAgIQITAW/ABovL39xYIjI3bKqDANBgkqhkiG9w0BAQsFADAu\n"
"MQswCQYDVQQGEwJVUzETMBEGA1UEChMKQW1hem9uLmNvbTEKMAgGA1UEAwwBKjAe\n"
"Fw0xOTAyMTEwNjEyMzNaFw0yMDAyMTEwNjEyMzNaMC4xCzAJBgNVBAYTAlVTMRMw\n"
"EQYDVQQKEwpBbWF6b24uY29tMQowCAYDVQQDDAEqMIIBIjANBgkqhkiG9w0BAQEF\n"
"AAOCAQ8AMIIBCgKCAQEArJoRHZPwLrgqETXf5H30H4OqzC9L0RHYaiunbrPVlUAd\n"
"QshjgLP7b4mahvsQhkN0XBoIlAQIkvKRbQuCSfiCP2WXfYc1UYGZdhW3kb/QhNdj\n"
"QzQnNFnJ4B72nHory6+KRtzgs7+XACQAzQGtRqWAnPjdvodoAjkx3qwNhTrcmqnF\n"
"0wdVc2s9gY5jpysWF5JGlM78Nw00GIYE7BsX/CZaIV4tXEDky6F9kmnxO35sg94s\n"
"KrAOniisGufGDYqt7IlqjWDgxxdnGh4ssHul89j5EyXny3S8ZoLXeR9Mwa3RZL3k\n"
"KRlQdpdgAHoNAyU1Jwrc1PvofSts3cvwpJ+HJpdcIQIDAQABo14wXDAOBgNVHQ8B\n"
"Af8EBAMCAoQwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMA8GA1UdEwEB\n"
"/wQFMAMBAf8wGgYDVR0RBBMwEYIJbG9jYWxob3N0hwQKTXRDMA0GCSqGSIb3DQEB\n"
"CwUAA4IBAQA8RBmdrE9TupRiEtttR+rEFgS03GeKvexaoRM3vRF8yWrzzigCJ1AU\n"
"/WrJQ6xHH2hKqcI6RLInk21kffN8s9d9zpUq9oDx6wsuv0Q3pCBKXW9WqC3UFFsh\n"
"YFazR0z5QSd2/U/5G32oyFmE+W3u9K/N8RtZUhxBzo1EhoVFS6k/TK9gze+JlvXc\n"
"7rqQ4E6aHLukfDWUXY4oRnaY/fEHHBlBz1XMGkKxYwLlBE+SbinjsbbLu/5qmuKH\n"
"LiVq0+Q/NuG4SCuMr55g9j+xcj4UVqsFEEGUxpgA9Jqx5zWKclV2ds+MCT9M+KQ0\n"
"YxWw+iA//aAmFFwH5z3qEOdExiu4iPcD\n"
"-----END CERTIFICATE-----\n";

#endif /* ifndef AWS_TEST_TCP_H */
