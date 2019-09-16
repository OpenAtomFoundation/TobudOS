#ifndef _QCLOUD_TLS_H_
#define  _QCLOUD_TLS_H_

static const char *qcloud_ca_cert = \
{
    "-----BEGIN CERTIFICATE-----\r\n"
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
    "QVrcRBDxzx/G\r\n" \
    "-----END CERTIFICATE-----"
};

static const char *qcloud_https_ca_cert = \
{
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n"
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n"
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n"
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n"
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n"
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n"
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n"
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n"
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n"
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n"
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n"
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n"
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n"
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n"
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n"
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n"
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n"
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n"
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n"
    "-----END CERTIFICATE-----\r\n"
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIEaTCCA1GgAwIBAgILBAAAAAABRE7wQkcwDQYJKoZIhvcNAQELBQAwVzELMAkG\r\n"
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n"
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw0xNDAyMjAxMDAw\r\n"
    "MDBaFw0yNDAyMjAxMDAwMDBaMGYxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n"
    "YWxTaWduIG52LXNhMTwwOgYDVQQDEzNHbG9iYWxTaWduIE9yZ2FuaXphdGlvbiBW\r\n"
    "YWxpZGF0aW9uIENBIC0gU0hBMjU2IC0gRzIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\r\n"
    "DwAwggEKAoIBAQDHDmw/I5N/zHClnSDDDlM/fsBOwphJykfVI+8DNIV0yKMCLkZc\r\n"
    "C33JiJ1Pi/D4nGyMVTXbv/Kz6vvjVudKRtkTIso21ZvBqOOWQ5PyDLzm+ebomchj\r\n"
    "SHh/VzZpGhkdWtHUfcKc1H/hgBKueuqI6lfYygoKOhJJomIZeg0k9zfrtHOSewUj\r\n"
    "mxK1zusp36QUArkBpdSmnENkiN74fv7j9R7l/tyjqORmMdlMJekYuYlZCa7pnRxt\r\n"
    "Nw9KHjUgKOKv1CGLAcRFrW4rY6uSa2EKTSDtc7p8zv4WtdufgPDWi2zZCHlKT3hl\r\n"
    "2pK8vjX5s8T5J4BO/5ZS5gIg4Qdz6V0rvbLxAgMBAAGjggElMIIBITAOBgNVHQ8B\r\n"
    "Af8EBAMCAQYwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUlt5h8b0cFilT\r\n"
    "HMDMfTuDAEDmGnwwRwYDVR0gBEAwPjA8BgRVHSAAMDQwMgYIKwYBBQUHAgEWJmh0\r\n"
    "dHBzOi8vd3d3Lmdsb2JhbHNpZ24uY29tL3JlcG9zaXRvcnkvMDMGA1UdHwQsMCow\r\n"
    "KKAmoCSGImh0dHA6Ly9jcmwuZ2xvYmFsc2lnbi5uZXQvcm9vdC5jcmwwPQYIKwYB\r\n"
    "BQUHAQEEMTAvMC0GCCsGAQUFBzABhiFodHRwOi8vb2NzcC5nbG9iYWxzaWduLmNv\r\n"
    "bS9yb290cjEwHwYDVR0jBBgwFoAUYHtmGkUNl8qJUC99BM00qP/8/UswDQYJKoZI\r\n"
    "hvcNAQELBQADggEBAEYq7l69rgFgNzERhnF0tkZJyBAW/i9iIxerH4f4gu3K3w4s\r\n"
    "32R1juUYcqeMOovJrKV3UPfvnqTgoI8UV6MqX+x+bRDmuo2wCId2Dkyy2VG7EQLy\r\n"
    "XN0cvfNVlg/UBsD84iOKJHDTu/B5GqdhcIOKrwbFINihY9Bsrk8y1658GEV1BSl3\r\n"
    "30JAZGSGvip2CTFvHST0mdCF/vIhCPnG9vHQWe3WVjwIKANnuvD58ZAWR65n5ryA\r\n"
    "SOlCdjSXVWkkDoPWoC209fN5ikkodBpBocLTJIg1MGCUF7ThBCIxPTsvFwayuJ2G\r\n"
    "K1pp74P1S8SqtCr4fKGxhZSM9AyHDPSsQPhZSZg=\r\n"
    "-----END CERTIFICATE-----"
};


typedef struct qcloud_tls_option_st {
    const char     *ca_cert;
    size_t          ca_cert_len;

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_CERT)
    const char     *cert_path;
    const char     *priv_key_path;
#else
    uint8_t         psk[QCLOUD_PSK_MAX];
    char            psk_id[QCLOUD_MQTT_DEVICE_CLIENT_ID_MAX + 1];
    size_t          psk_len;
#endif

    void           *handle;

    uint32_t        timeout;
} qcloud_tls_opt_t;

__QCLOUD_INTERNAL__ qcloud_err_t qcloud_tls_init(qcloud_tls_opt_t *tls_opt, qcloud_device_t *device);

#endif

