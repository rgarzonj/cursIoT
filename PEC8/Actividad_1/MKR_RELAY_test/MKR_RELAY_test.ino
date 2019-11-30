/*
  WiFi Web Server to set an alarm on the RTC to drive a relay

  A simple web server that shows a form to insert an alarm time
  on a MKR1000 + MKR Relay Shield to drive relays on MKR Relay Shield.
  
  At alarm match the two outputs will be driven HIGH for 5 minutes

  Circuit:
   - MKR1000 + MKR Relay Proto Shield Shield

  created 05 Feb 2017
  by Arturo Guadalupi
*/

#include <SPI.h>
#include <WiFi101.h>
#include <RTCZero.h>
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
/////// Wifi Settings ///////
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

const int relay1 = 1;
const int relay2 = 2;
const int onMinutes = 5; //stay on for 5 minutes

RTCZero rtc; // create an RTC object

const int GMT = 1; //change this to adapt it to your time zone

bool driveOutputs = false;

void setup() {
  //initialize relays pins and drive them low
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);

  Serial.begin(9600);  //initialize serial communication

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();  // initialize the server
  printWiFiStatus(); // you're connected now, so print out the status:

  rtc.begin(); // initialize the RTC library
  setRTCwithNTP(); // set the RTC time/date using epoch from NTP
  printTime(); // print the current time
  printDate(); // print the current date
  rtc.attachInterrupt(alarmMatch); // call alarmMatch() function when interrupt occur
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\n');  // receive the request
    request.trim();

    if ((request == "GET / HTTP/1.1") || (request == "GET /post_time?usr_time= HTTP/1.1")) {
      // show the selection time page
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.println("<!DOCTYPE HTML>");
      client.println("<html>");
      client.println("<body>");
      client.println("<p><strong>MKR Relay Proto Shield control page</strong></p>");
      client.println("<form action=\"post_time\">");
      client.println("Select a time to turn relays ON at:");
      client.println("<input type=\"time\" name=\"usr_time\">");
      client.println("<input type=\"submit\">");
      client.println("</form>");
      client.println("</body>");
      client.println("</html>");
      client.stop();
    }

    else {
      bool requestFound = false;  // flag used to check if the request has been received
      //search for the sent time
      for (int hours = 0; (hours < 24) && (requestFound == false); hours++) { // stop the for if request is found
        for (int minutes = 0; (minutes < 60) && (requestFound == false); minutes++) { // stop the for if request is found
          String requestString = "GET /post_time?usr_time=";  // fixed request part
          // add a zero if number 
          if (hours < 10) {
            requestString += "0";
          }

          requestString += hours;
          requestString += "%3A";

          if (minutes < 10) {
            requestString += "0";
          }

          requestString += minutes;
          requestString += " HTTP/1.1";

          if (requestString == request) {
            requestFound = true;
            String timeSet = "<p>Alarm correctly set to ";
            if (hours < 10)
              timeSet += "0";
            timeSet += hours;
            timeSet += ":";
            if (minutes < 10)
              timeSet += "0";
            timeSet += minutes;
            timeSet += "</p>";
            // show the success page
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<p><strong>Success!</strong></p>");
            client.println(timeSet);
            client.println("</body>");
            client.println("</html>");
            client.stop();

            //reset outputs
            digitalWrite(relay1, LOW);
            digitalWrite(relay2, LOW);
            
            Serial.println();
            Serial.print("Alarm set to: ");
            print2digits(hours);
            Serial.print(":");
            print2digits(minutes);
            Serial.println();
            
            rtc.setAlarmTime(hours, minutes, 00); // set alarm
            rtc.enableAlarm(rtc.MATCH_HHMMSS); // enable the RTC interrupt to match hours, minutes and seconds
          }
        }
      }

      if (requestFound == false) {
        //flush the client
        while (client.available()) {
          client.read();
        }

        // print the not found error code
        client.print("HTTP/1.1 ");
        client.print(404);
        client.print(" ");
        client.println("Not Found");
        client.println("Connection: close");
        client.println();
        client.stop();
      }
    }
  }
}

void alarmMatch() {
  //togle the drive output value
  Serial.println("Alarm match!");
  if (driveOutputs == false) {
    driveOutputs = true;
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    
    // set next alarm in 5 minutes to drive outputs HIGH for this time
    int thisMinutes = rtc.getMinutes() + onMinutes;
    if (onMinutes >= 60) {
      rtc.setAlarmTime(rtc.getHours() + 1, thisMinutes - 60, 00);
    }
    else {
      rtc.setAlarmTime(rtc.getHours(), thisMinutes, 00);
    }
  }
  else {
    driveOutputs = false;
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
  }
}

void setRTCwithNTP() {
  unsigned long epoch;
  int numberOfTries = 0, maxTries = 6;
  do {
    epoch=1575061708;
    //epoch = WiFi.getTime();
    Serial.println(epoch);
    numberOfTries++;
  }
  while ((epoch == 0) || (numberOfTries > maxTries));

  if (numberOfTries > maxTries) {
    Serial.print("NTP unreachable!!");
    while (1);
  }
  else {
    Serial.print("Epoch received: ");
    Serial.println(epoch);
    rtc.setEpoch(epoch);

    Serial.println();
  }
  rtc.setHours(rtc.getHours() + GMT);
}

void printTime() {
  print2digits(rtc.getHours());
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());
  Serial.println();
}

void printDate() {
  print2digits(rtc.getDay());
  Serial.print("/");
  print2digits(rtc.getMonth());
  Serial.print("/");
  print2digits(rtc.getYear());

  Serial.print(" ");
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
