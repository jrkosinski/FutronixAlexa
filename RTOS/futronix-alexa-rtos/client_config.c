// AWS IoT client endpoint
const char *client_endpoint = "a21jd7gud1swyd.iot.us-east-1.amazonaws.com";

// AWS IoT device certificate (ECC)
const char *client_cert =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDFTCCAf2gAwIBAgIUYuiKI3pB1OHyMvUjNv/SnskSR08wDQYJKoZIhvcNAQEL\r\n"
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\r\n"
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE3MTAzMDA5MjQx\r\n"
"MVoXDTQ5MTIzMTIzNTk1OVowgaQxCzAJBgNVBAYTAlRIMRMwEQYDVQQIDApDaGlh\r\n"
"bmcgTWFpMRMwEQYDVQQHDApDaGlhbmcgTWFpMRkwFwYDVQQKDBBGb3JtaW5hIFNv\r\n"
"ZnR3YXJlMRAwDgYDVQQLDAdJVCBEZXB0MRYwFAYDVQQDDA1Kb2huIEtvc2luc2tp\r\n"
"MSYwJAYJKoZIhvcNAQkBFhdqb2huLmtvc2luc2tpQGdtYWlsLmNvbTBZMBMGByqG\r\n"
"SM49AgEGCCqGSM49AwEHA0IABLSXtzFfRthJCQMJ9syJLU2uAH4EI6DxoylW7Fb8\r\n"
"B7JAmFVunOcFpE388qM6DkVF9HwhtEPBhSFpK3W4yRcYTnWjYDBeMB8GA1UdIwQY\r\n"
"MBaAFIcvND4YEnwNRLP0RUeYrDHDbPsgMB0GA1UdDgQWBBRuLn4ULtnM+HlkxPIw\r\n"
"TW/HyewtJDAMBgNVHRMBAf8EAjAAMA4GA1UdDwEB/wQEAwIHgDANBgkqhkiG9w0B\r\n"
"AQsFAAOCAQEAdcjbrDGvMn21KOcSYiOj6PgERdDBj2FMdFW7l7go9Y+ggE4lQXtP\r\n"
"C3l9Sqh28nc8U5ZU/Pi7UfYjuqbs2X2oFqEs8Jg0ANdA77xnsnmSKnU5/jAJVORi\r\n"
"knP/kd+6EHPiRBsjeGyPWowZWfALQaOpz76tMxG/7xNH6l0EMba2DgtVsLPfqVjA\r\n"
"a03tfGM+9lCeyAZPK0husVYxUri2Q1LciINSrLh6zGw42/WY36S/jMEdyyZ0j8KI\r\n"
"MEupvwgCKT+ZSVtmFwUC4M28B1DdaDkvo+HxL2ByuyPQFyNaYHVaIq2BDDLu1Uqr\r\n"
"zkpD+1ZetpB7gZv9Ec9fZ/eAJC6H7+NvPQ==\r\n"
"-----END CERTIFICATE-----\r\n"; 

// AWS IoT device private key (ECC)
const char *client_key = 
"-----BEGIN EC PARAMETERS-----\r\n"
"BggqhkjOPQMBBw==\r\n"
"-----END EC PARAMETERS-----\r\n"
"-----BEGIN EC PRIVATE KEY-----\r\n"
"MHYCAQEEH+Ki3lwjEp++3TMcPugyboBGuulaLl+z/NRtxiKfIIOgCgYIKoZIzj0D\r\n"
"AQehRANCAAS0l7cxX0bYSQkDCfbMiS1NrgB+BCOg8aMpVuxW/AeyQJhVbpznBaRN\r\n"
"/PKjOg5FRfR8IbRDwYUhaSt1uMkXGE51\r\n"
"-----END EC PRIVATE KEY-----\r\n"; 


