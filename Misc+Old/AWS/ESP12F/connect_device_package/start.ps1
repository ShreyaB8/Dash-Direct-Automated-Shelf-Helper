# requires: Nodejs/NPM, PowerShell
# Permission to run PS scripts (for this session only):
# Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process

# exit if cmdlet gives error
$ErrorActionPreference = "Stop"

# Check to see if root CA file exists, download if not
If (!(Test-Path ".\root-CA.crt")) {
    "`nDownloading AWS IoT Root CA certificate from AWS..."
    Invoke-WebRequest -Uri https://www.amazontrust.com/repository/AmazonRootCA1.pem -OutFile root-CA.crt
}

# install AWS Device SDK for NodeJS if not already installed
If (!(Test-Path ".\aws-iot-device-sdk-js-v2")) {
    "`nInstalling AWS SDK..."
    git clone https://github.com/aws/aws-iot-device-sdk-js-v2.git
    cd aws-iot-device-sdk-js-v2
    npm install
    # install samples
    cd samples/node/pub_sub
    npm install
    cd ../../../../
}

"`nRunning pub/sub sample application..."
node aws-iot-device-sdk-js-v2\samples\node\pub_sub\dist\index.js --endpoint a1e0u5lwlu9sum-ats.iot.ap-south-1.amazonaws.com --ca_file root-CA.crt --key Esp12F.private.key --cert Esp12F.cert.pem --client_id sdk-nodejs-v2 --topic topic_1
