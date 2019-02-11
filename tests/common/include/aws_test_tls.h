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
#ifndef _AWS_TLS_TEST_H_
#define _AWS_TLS_TEST_H_

/*
* PEM-encoded client certificate.
*
* Certificate for P-256 elliptic curve key.
*/
static const char tlstestCLIENT_CERTIFICATE_PEM_EC[] = "-----BEGIN CERTIFICATE-----\n"
"MIIChjCCAW6gAwIBAgIVAPbVlq94Sb5EvUr5y+jDYI1Js8GmMA0GCSqGSIb3DQEB\n"
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xOTAyMTEwMjI4\n"
"MjFaFw00OTEyMzEyMzU5NTlaMBUxEzARBgNVBAoTCkFtYXpvbi5jb20wWTATBgcq\n"
"hkjOPQIBBggqhkjOPQMBBwNCAATFcuPoMJ6e2JxkiZtiNM2GBiwc2QvgSQif+c6D\n"
"Q9iMGtsEGYKTyygRC6y/lienRzeX/EJTDRnYexeITONYCIKGo2AwXjAfBgNVHSME\n"
"GDAWgBR5fzZRXyAg8f2+7n957qvueMwD6zAdBgNVHQ4EFgQU8rx0vRmomgl3xpvq\n"
"8xGqg6kfGEgwDAYDVR0TAQH/BAIwADAOBgNVHQ8BAf8EBAMCB4AwDQYJKoZIhvcN\n"
"AQELBQADggEBAKrIoqTVxNzIuH8585Gm+igHR0qCg8TBRJBII5LDbKvy/cy2Yj9a\n"
"jWrU/EMHOOXKCWqz3NGBMcj6/LFheJKV+pUBXx2Ah8fzMU9z1xQ8F7KcwRLPdjUI\n"
"CFjgiBNOwp4XA65nbeopSJjxhTSiMU7gE6Fqw1B0avBQpivd6KJvg/L9NNnJXbOo\n"
"AazBbJKACCdoRGlT1zpc5vV0OD2vQ67G/TTYH4fRsm1z6AYSvjRBIRWOx8BHWL/a\n"
"BJC7mp76R/QqsFk/xYJYgom/pSTPoUq7C0zqDkBX6+P4lMrPZud1SbNMzmHY3Xul\n"
"1HsLGt62thYJwzxnLWPYDQH2+ne2nAZUaZo=\n"
"-----END CERTIFICATE-----\n";

/*
* PEM-encoded client private key.
*
* This is a P-256 elliptic curve key.
*/
static const char tlstestCLIENT_PRIVATE_KEY_PEM_EC[] = "-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEIAxml2hjAW6H96u8hwMQnhisCBVgZSekOvvXeMpxi8MEoAoGCCqGSM49\n"
"AwEHoUQDQgAExXLj6DCenticZImbYjTNhgYsHNkL4EkIn/nOg0PYjBrbBBmCk8so\n"
"EQusv5Ynp0c3l/xCUw0Z2HsXiEzjWAiChg==\n"
"-----END EC PRIVATE KEY-----\n";

/* One character of this certificate has been changed in the issuer
 * name from Amazon Web Services to Amazon Web Cervices. */
static const char tlstestCLIENT_CERTIFICATE_PEM_MALFORMED[] = "-----BEGIN CERTIFICATE-----\n"
"MIIDWTCCAkGgAwIBAgIUQ3+k1EFDFbd6vPXAC+Xg873GZiIwDQYJKoZIhvcNAQEL\n"
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBDZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDIxMTAyMjgy\n"
"MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANONqkGt+Y9h3VEZVYE+\n"
"BEI2sV/MVcjyBhDkPdrohmBJnSylTj18FH6gO7lZHWr4p8mYBeACWKvSDkOo93cq\n"
"a81I7J1J3+svEjXLbXO7ltsMCMLYZtaaemBWVGVG/ltfYcYSbpBFjwuFHDQX3qkB\n"
"ETtF+h6mF0xtWmqdxa8XqnzLC8i3pJhHf/UiwM2amGnEWzfjGT04gjOCWXciDlHe\n"
"6NN85QlZtIXik7xs/2DbWeiqJnURlH/gmXDvHlVagZZxAk4y/zWPtN0Pp5H1rlSx\n"
"4VFuv8Wht/gi1BTdSNQgzXtpYs4n1OF6JfkfDfZxXIFvYcanVHnWosUUhiXcHIBb\n"
"XV0CAwEAAaNgMF4wHwYDVR0jBBgwFoAU9PbBY4Yu1gGGvfRTy2R1FO8jC20wHQYD\n"
"VR0OBBYEFBXX8F9hrEk9yowW56RDJIVTp7aTMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCsx2dFlcVCC90fOJaFhenuxrFu\n"
"Oxdd4R2wEYCXcDEPoM1XQq00fjBucW+1YRvDBtpiZekG99dyNyIXv9LXCyQrPavM\n"
"OOSzLlQxFK1Oq+dwcoqtG9ChSBO7VFBLJ+Mx4m0yfwxhxfYsg+clqFd5TEXmPksV\n"
"kVojgB46ev2YGb4MQWFgAasmaWJl3StinIjkChK0MvJ9Xj5QcOI/HCbwoR9MCWum\n"
"DL9rReA+FuLDJlIbCpPzDUmft3s7MBF7dRjfZM8e8TP+COdLOzDyY+eAQqlwzewy\n"
"P5ykUc9GzFm4gj/UIXuKoILK6z+YJUKcoxKNt7Q7uxVwQFFzlRSUus3eKjbC\n"
"-----END CERTIFICATE-----\n";

/* Certificate which is not trusted by the broker. */
static const char tlstestCLIENT_UNTRUSTED_CERTIFICATE_PEM[] = "-----BEGIN CERTIFICATE-----\n"
"MIIBTzCB96ADAgECAgECMAoGCCqGSM49BAMCMBUxEzARBgNVBAoTCkFtYXpvbi5j\n"
"b20wHhcNMTkwMjExMDIzMDIxWhcNMTkwMjEyMDIzMDIxWjAVMRMwEQYDVQQKEwpB\n"
"bWF6b24uY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEFV9lwdrkcI1LeOVK\n"
"+sWWAhzwj0FKEHNJfTXBIL+3Yh0b+OgYEZlZITnzLiLoJiWEoGJYBhyMLEMOBKEj\n"
"ovNJwqM4MDYwDgYDVR0PAQH/BAQDAgKkMBMGA1UdJQQMMAoGCCsGAQUFBwMBMA8G\n"
"A1UdEwEB/wQFMAMBAf8wCgYIKoZIzj0EAwIDRwAwRAIgQpcdALeNPYkFCaDBam7K\n"
"n1lfwf1HVDtxPmhNyN9lSkYCIEkjM/lX6ychDLCZbKtFYgVtELQ1ftkeRfLBm+ix\n"
"UnFw\n"
"-----END CERTIFICATE-----\n";

/* Private key corresponding to the untrusted certificate. */
static const char tlstestCLIENT_UNTRUSTED_PRIVATE_KEY_PEM[] = "-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEIDC3MVRzxYp2aPNIZn9KFk/SJv7Qcnt9vrR1KdQT1JuMoAoGCCqGSM49\n"
"AwEHoUQDQgAEFV9lwdrkcI1LeOVK+sWWAhzwj0FKEHNJfTXBIL+3Yh0b+OgYEZlZ\n"
"ITnzLiLoJiWEoGJYBhyMLEMOBKEjovNJwg==\n"
"-----END EC PRIVATE KEY-----\n";

/* Device certificate created using BYOC instructions. */
static const char tlstestCLIENT_BYOC_CERTIFICATE_PEM[] = "-----BEGIN CERTIFICATE-----\n"
"MIIBUDCB96ADAgECAgECMAoGCCqGSM49BAMCMBUxEzARBgNVBAoTCkFtYXpvbi5j\n"
"b20wHhcNMTkwMjExMDIzMDIxWhcNMTkwMjEyMDIzMDIxWjAVMRMwEQYDVQQKEwpB\n"
"bWF6b24uY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEFhNtrtZKnmnzliwS\n"
"nW4gkkMfPgE6nCZpdKAWb/ly5UmT0I91lOJHA5gorejBiX/dEqo+x9/FcFHwlYCf\n"
"6Tz1/KM4MDYwDgYDVR0PAQH/BAQDAgKkMBMGA1UdJQQMMAoGCCsGAQUFBwMBMA8G\n"
"A1UdEwEB/wQFMAMBAf8wCgYIKoZIzj0EAwIDSAAwRQIgVgZ9D8GqxxQVsrCFQltX\n"
"bpESYiKLJw+OnNieE30vJ5ACIQCUrJ8rWAKwpnUZglI1v8Sg9tJudw/nr1Ekj+n3\n"
"VxOiSQ==\n"
"-----END CERTIFICATE-----\n";

/* Device private key created using BYOC instructions. */
static const char tlstestCLIENT_BYOC_PRIVATE_KEY_PEM[] = "-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEIAuP7rd8w+VqOMyuOGXYUUKaOnYHvK3h+zUQxQP0uzjooAoGCCqGSM49\n"
"AwEHoUQDQgAEFhNtrtZKnmnzliwSnW4gkkMfPgE6nCZpdKAWb/ly5UmT0I91lOJH\n"
"A5gorejBiX/dEqo+x9/FcFHwlYCf6Tz1/A==\n"
"-----END EC PRIVATE KEY-----\n";

#endif
