#ifndef __DEMO_CA_H__
#define  __DEMO_CA_H__

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/certs.h"

#if defined(MBEDTLS_PEM_PARSE_C)
#define ROOT_CA 																											 \
"-----BEGIN CERTIFICATE-----\r\n"																			 \
"MIIDxTCCAq2gAwIBAgIJALM1winYO2xzMA0GCSqGSIb3DQEBCwUAMHkxCzAJBgNV\r\n" \
"BAYTAkNOMRIwEAYDVQQIDAlHdWFuZ0RvbmcxETAPBgNVBAcMCFNoZW5aaGVuMRAw\r\n" \
"DgYDVQQKDAdUZW5jZW50MRcwFQYDVQQLDA5UZW5jZW50IElvdGh1YjEYMBYGA1UE\r\n" \
"AwwPd3d3LnRlbmNlbnQuY29tMB4XDTE3MTEyNzA0MjA1OVoXDTMyMTEyMzA0MjA1\r\n" \
"OVoweTELMAkGA1UEBhMCQ04xEjAQBgNVBAgMCUd1YW5nRG9uZzERMA8GA1UEBwwI\r\n" \
"U2hlblpoZW4xEDAOBgNVBAoMB1RlbmNlbnQxFzAVBgNVBAsMDlRlbmNlbnQgSW90\r\n" \
"aHViMRgwFgYDVQQDDA93d3cudGVuY2VudC5jb20wggEiMA0GCSqGSIb3DQEBAQUA\r\n" \
"A4IBDwAwggEKAoIBAQDVxwDZRVkU5WexneBEkdaKs4ehgQbzpbufrWo5Lb5gJ3i0\r\n" \
"eukbOB81yAaavb23oiNta4gmMTq2F6/hAFsRv4J2bdTs5SxwEYbiYU1teGHuUQHO\r\n" \
"iQsZCdNTJgcikga9JYKWcBjFEnAxKycNsmqsq4AJ0CEyZbo//IYX3czEQtYWHjp7\r\n" \
"FJOlPPd1idKtFMVNG6LGXEwS/TPElE+grYOxwB7Anx3iC5ZpE5lo5tTioFTHzqbT\r\n" \
"qTN7rbFZRytAPk/JXMTLgO55fldm4JZTP3GQsPzwIh4wNNKhi4yWG1o2u3hAnZDv\r\n" \
"UVFV7al2zFdOfuu0KMzuLzrWrK16SPadRDd9eT17AgMBAAGjUDBOMB0GA1UdDgQW\r\n" \
"BBQrr48jv4FxdKs3r0BkmJO7zH4ALzAfBgNVHSMEGDAWgBQrr48jv4FxdKs3r0Bk\r\n" \
"mJO7zH4ALzAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQDRSjXnBc3T\r\n" \
"d9VmtTCuALXrQELY8KtM+cXYYNgtodHsxmrRMpJofsPGiqPfb82klvswpXxPK8Xx\r\n" \
"SuUUo74Fo+AEyJxMrRKlbJvlEtnpSilKmG6rO9+bFq3nbeOAfat4lPl0DIscWUx3\r\n" \
"ajXtvMCcSwTlF8rPgXbOaSXZidRYNqSyUjC2Q4m93Cv+KlyB+FgOke8x4aKAkf5p\r\n" \
"XR8i1BN1OiMTIRYhGSfeZbVRq5kTdvtahiWFZu9DGO+hxDZObYGIxGHWPftrhBKz\r\n" \
"RT16Amn780rQLWojr70q7o7QP5tO0wDPfCdFSc6CQFq/ngOzYag0kJ2F+O5U6+kS\r\n" \
"QVrcRBDxzx/G\r\n" 																											\
"-----END CERTIFICATE-----\r\n"

#define CLIENT_CA																												\
"-----BEGIN CERTIFICATE-----\r\n"																				\
"MIIDTzCCAjegAwIBAgIBADANBgkqhkiG9w0BAQsFADB5MQswCQYDVQQGEwJDTjES\r\n"	\
"MBAGA1UECAwJR3VhbmdEb25nMREwDwYDVQQHDAhTaGVuWmhlbjEQMA4GA1UECgwH\r\n"	\
"VGVuY2VudDEXMBUGA1UECwwOVGVuY2VudCBJb3RodWIxGDAWBgNVBAMMD3d3dy50\r\n"	\
"ZW5jZW50LmNvbTAeFw0xODEwMTUwOTA3NDVaFw0yODEwMTIwOTA3NDVaMBwxGjAY\r\n"	\
"BgNVBAMMEVlDNEFUVTg2WDFJb1RfVExTMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\r\n"	\
"MIIBCgKCAQEAvHzsA+oqSm7FGz3VWzWKaHCF/4+RU5AUqBz7OflRp3Ybkmd+7G5y\r\n"	\
"3SjHDqnqTZNJDPRekeM+JlKv006W8z7pVitPfa2AmjSBafFa28zSUv2plkJ1q++g\r\n"	\
"uwPM3iHTbZxzTd2nejespuxoYDaA9FcV7o+j2mz9BGsQjQrs7GDID7qi9WkmfjO4\r\n"	\
"vvNYLYaPQ6vRd/sLyLHrAkTY7RNbNN884lWNOMc599TIZjpskzu8scNjRgWJEoKb\r\n"	\
"riKRJ+Mlq4itZKfEF2HtE4H9lB0EZXy2LGL0IrP5zjBHp2Lv+AUNyLcZ6GYKo073\r\n"	\
"LijkAh8if20R9dHaWQJXsyTNNbJNV1VzSQIDAQABoz8wPTAMBgNVHRMBAf8EAjAA\r\n"	\
"MA4GA1UdDwEB/wQEAwIHgDAdBgNVHQ4EFgQU2PRMZ5ZBgvFwqsNR4M+J3uiTxfIw\r\n"	\
"DQYJKoZIhvcNAQELBQADggEBACl95yR8ByzjWrq/d3Wl3AU9gNxYbZWIFKeCMVJp\r\n"	\
"cDdMYewkJe5D2afoD2aWA0rq6bEc8+TPJC59V2PGvAizsKWiFmNI4pPUU9u8nuoX\r\n"	\
"Z/8emDEKjbEsTvFp0Gagu3zNenKNTzbdgdA6V5W8mcw+yQl/K8yTqx5usozeAeKy\r\n"	\
"dt/EloPbXc87CmeDaPCqNHWQW1dFzbSB2lEtoSYYPaWxjcmpx6mi6vs4Wcvqn6+c\r\n"	\
"sUyiE0rv7t8ynpqwpOjpwuQ4MoWkRayKVCIoSOFY/XWktUUYr7oLVYS6QsRQhYde\r\n"	\
"qAIyd+daZsGRNKjM3ADRao7kqW/B5xltun9HYnfH0YyNGuM=\r\n"									\
"-----END CERTIFICATE-----\r\n"

#define PRIVATE_KEY																											\
"-----BEGIN PRIVATE KEY-----\r\n"																				\
"MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQC8fOwD6ipKbsUb\r\n"	\
"PdVbNYpocIX/j5FTkBSoHPs5+VGndhuSZ37sbnLdKMcOqepNk0kM9F6R4z4mUq/T\r\n"	\
"TpbzPulWK099rYCaNIFp8VrbzNJS/amWQnWr76C7A8zeIdNtnHNN3ad6N6ym7Ghg\r\n"	\
"NoD0VxXuj6PabP0EaxCNCuzsYMgPuqL1aSZ+M7i+81gtho9Dq9F3+wvIsesCRNjt\r\n"	\
"E1s03zziVY04xzn31MhmOmyTO7yxw2NGBYkSgpuuIpEn4yWriK1kp8QXYe0Tgf2U\r\n"	\
"HQRlfLYsYvQis/nOMEenYu/4BQ3ItxnoZgqjTvcuKOQCHyJ/bRH10dpZAlezJM01\r\n"	\
"sk1XVXNJAgMBAAECggEAHopzR5MatSLLp0VWm8fKHHAXFjiyCIMxBahkgJlIRMUb\r\n"	\
"ZJaGd1LSI8mmr1aS020JCiNSa3sRp+3/9VInnVRVRJQR9kA1tUbKrVrhKPpAHhhh\r\n"	\
"8SnlnS1fss3D1T5P3tPZzwUaTPBWdlmZ8/UA4hZ2REhb+Vltidi7Ps9FYmuwHvQM\r\n"	\
"nJCKvIWHJoFQcLqrWnjir5x61dOplvNkNwhZTLvCCyiwckL5irlAAAPc5xXevsw2\r\n"	\
"JBiXq8/vZ3hsW4c2fSvnbUmVJpY7hx9GIxPQ/IO1VUZsXXQh6USogydg1WnScIuc\r\n"	\
"o64D8nFdNMh2eNsNKzk2FRaubZZvVPM+PgW7inOHHQKBgQDhC1iucZ/iWc57oju5\r\n"	\
"Ki0zBxw0pBWo00YfCiL5WfcKaNpeEmLCdMsHOBcodIo9WSFLsywDm+PmVbgn6VAf\r\n"	\
"LA3+aagaTFtLCZLA/LDtWrrIS0EEED9fKRlbFmFJAx8ExLD1ieAXvq6IoSZG/SmK\r\n"	\
"dyz5S9sd0jB2CzXkFrEQajcnvwKBgQDWakomx8qwwxrPVP/DSsoGKhw7ZlDpbaCb\r\n"	\
"BLNJBaw49iYRvV54AFS8fzciAjpDwoobOwgaZCOJaUuU20LFNHh5rHSZa3pMkX9A\r\n"	\
"AZ4vMKNdQe5U8Ro9UG7+szGgegee7s7VoFh9mRDIRrhUzx059Q0sZqL5RYoXMKJj\r\n"	\
"V0Xb77hm9wKBgQCFnxgT1BGL86j/jc+KXTmjP8iGlRrrOC89hOZJShq+Co4GJ0KO\r\n"	\
"lYxrh+lwMupWDNMRQJyUs/44qJME16qkJ6NTeWa/3rCiOFfz0NY4yJN5E/3RrNgD\r\n"	\
"58JPbFVtZbYIPcWYtTniv2ygYZb5tZ3xwlto1pqO9Vui63sfKD4QMhD3vwKBgE9q\r\n"	\
"mwB2K9sAyONWZk0dXGqgGkjeAsYS9tO0D/vSABIxCbS0YVMtPDV9IRyVmqMSYTjo\r\n"	\
"yn2L7aWtvHLsK3UDlEPs3lsc9fV6Rpmcf8yWxUt7ah1IGCHxssrf8FeyGzSA21dA\r\n"	\
"xsXudcrYdtnixL58x15UvbR6T0V7nAoTfyqRRcnNAoGBAJxlepUmirnpboTrpbkj\r\n"	\
"iTbhVisJa4TnWy8WTGq3IQaKONbI+JPNv0qCzxNmq8iimYFdLFjieVLdVVHzFNSl\r\n"	\
"Wj8h2LKmYB5cmqKJ0H/my3zF8GhYI49tg4tBVLXdQNXHcp6YNOFU9wBjnJSpg/4b\r\n"	\
"hYfea8W18eO+tGG9BMeXBkbv\r\n"																					\
"-----END PRIVATE KEY-----\r\n"

static const char   root_ca_pem[]       = ROOT_CA;
static const size_t root_ca_pem_len     = sizeof(ROOT_CA);

static const char   client_ca_pem[]     = CLIENT_CA;
static const size_t client_ca_pem_len   = sizeof(CLIENT_CA);

static const char   client_key_pem[]    = PRIVATE_KEY;
static const size_t client_key_pem_len  = sizeof(PRIVATE_KEY);
#endif

#endif

