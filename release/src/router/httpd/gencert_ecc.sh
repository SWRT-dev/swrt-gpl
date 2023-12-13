#!/bin/sh
SECS=1262278080

cd /etc

NVCN=`nvram get local_domain`
LANIP=$(nvram get lan_ipaddr)

if [ "$NVCN" == "" ]; then
	NVCN="www.asusrouter.com"
fi

cp -L openssl.cnf openssl.config

add_san() {
	sed -i "/\[alt_names\]/a$1" openssl.config
}

I=18
echo "$I.commonName=CN" >> openssl.config
echo "$I.commonName_value=$CN" >> openssl.config
I=$(($I + 1))
add_san "IP.1 = $LANIP"
add_san "DNS.$I = $LANIP"

# add startdate option for certificate
echo "default_startdate=`date +%Y%m%d%H%M%S%Z`" >> openssl.config

# 2022/09/07 ECC key
openssl ecparam -name prime256v1 -genkey -noout -out /etc/key.pem

# 2020/01/03 import the self-certificate
OPENSSL_CONF=/etc/openssl.config RANDFILE=/dev/urandom openssl req -x509 -new -key /etc/key.pem -out /etc/cert.pem -days 3653

# server.pem for WebDav SSL
cat key.pem cert.pem > server.pem

# 2020/01/03 import the self-certificate
cp cert.pem cert.crt

rm -f openssl.config
