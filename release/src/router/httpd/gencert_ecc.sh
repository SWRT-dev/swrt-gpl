#!/bin/sh
SECS=1262278080

cd /etc

NVCN=`nvram get local_domain`
if [ "$NVCN" == "" ]; then
	NVCN="www.asusrouter.com"
fi

cp -L openssl.cnf openssl.config

I=0
for CN in $NVCN; do
        echo "$I.commonName=CN" >> openssl.config
        echo "$I.commonName_value=$CN" >> openssl.config
        I=$(($I + 1))
done

# 2022/09/07 ECC key
openssl ecparam -name prime256v1 -genkey -noout -out /etc/key.pem

# 2020/01/03 import the self-certificate
OPENSSL_CONF=/etc/openssl.config RANDFILE=/dev/urandom openssl req -x509 -new -key /etc/key.pem -out /etc/cert.pem -days 365

# server.pem for WebDav SSL
cat key.pem cert.pem > server.pem

# 2020/01/03 import the self-certificate
cp cert.pem cert.crt

rm -f openssl.config
