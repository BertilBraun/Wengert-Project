#pragma once

const char apn[]      = "internet.eplus.de";
const char gprsUser[] = "eplus";
const char gprsPass[] = "internet";

// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26

#define SerialMon Serial
#define SerialAT Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

bool post(const String& url, const String& data) {
  
  HttpClient http(client, "meinwengert.de", 80);
  http.connectionKeepAlive();
  // http.setHttpResponseTimeout(timeout);

  if (http.post(url, "application/x-www-form-urlencoded", data) == HTTP_SUCCESS) {
    SerialMon.println(http.responseBody());
    //SerialMon.println(http.responseStatusCode());
    return true;
  }
  else {
    SerialMon.println("Error on request");
    SerialMon.println(http.responseStatusCode());
    SerialMon.println(http.responseBody());
    return false;
  }
}

void InitModem() {

	SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

	pinMode(MODEM_PWKEY, OUTPUT);
	pinMode(MODEM_RST, OUTPUT);
	pinMode(MODEM_POWER_ON, OUTPUT);
}

bool ConnectModem() {
  InitModem();

	digitalWrite(MODEM_PWKEY, LOW);
	digitalWrite(MODEM_RST, HIGH);
	digitalWrite(MODEM_POWER_ON, HIGH);

	delay(1000);

	SerialMon.print("Initializing modem");
	if (!modem.restart()) {
		SerialMon.print(" FAIL!");
		while (!modem.restart());
	}
	SerialMon.println(" OK");

	SerialMon.print(String("Connecting to APN: ") + apn);

	if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
		SerialMon.println(" FAIL!");
		return false;
	}
	else 
		SerialMon.println(" OK");
	return true;
}

void DisconnectModem() {
	
    modem.gprsDisconnect();
    SerialMon.println("GPRS disconnected");
}
