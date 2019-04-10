// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "Wicos";
const char* password = "ApISS19mix";
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

int timer_end = 0;
int timer_now = 0;
byte timer_set = 0;
int timer_type = 0;
int timer_start = 0;
// Assign output variables to GPIO pins

byte output4 = 4;
byte output5 = 5;
 

void setup() {
  Serial.begin(9600);
  // Initialize the output variables as outputs

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            Serial.println(header);
            if (header.indexOf("GET /g/1") >= 0) {
              int read = digitalRead(5);
              client.println(read);
              Serial.println(read);
            } else if(header.indexOf("GET /g/2")>= 0)
            {
              client.println(digitalRead(4));
            }
            
            else if(header.indexOf("GET /get/timer/set") >= 0)
            {
              client.println(timer_set);
            }
             else if(header.indexOf("GET /get/timer/type") >= 0)
            {
              client.println(timer_set);
            }
            else if(header.indexOf("GET /get/timer/now") >= 0)
            {
              client.println(timer_now);
            } else if (header.indexOf("GET /set/pin/1/on") >= 0) {
              digitalWrite(5, HIGH);
            
              Serial.println("1/on");
            } else if (header.indexOf("GET /set/pin/1/off") >= 0) {
              digitalWrite(5, LOW);
            }
             else if (header.indexOf("GET /set/pin/2/on") >= 0) {
              digitalWrite(4, HIGH);
            } else if (header.indexOf("GET /set/pin/2/off") >= 0) {
              digitalWrite(4, LOW);
            } else if (header.indexOf("GET /set/timer/start") >= 0) {
              String arg = header.substring(header.indexOf("$")+1,header.length()-header.indexOf("$"));
              arg = arg.substring(0,arg.indexOf(" HTTP"));
              timer_start = arg.toInt();
              Serial.println(arg);
            } else if (header.indexOf("GET /set/timer/reset") >= 0) {
              timer_now = 0;
              timer_end = 0;
              timer_start = 0;
              timer_type = 0;
            } else if(header.indexOf("GET /set/timer/type") >= 0)
            {
              String arg = header.substring(header.indexOf("$")+1,header.length()-header.indexOf("$"));
              arg = arg.substring(0,arg.indexOf(" HTTP"));
              timer_type = arg.toInt();
              Serial.println("Timer_TYP: "+arg);
            } else if(header.indexOf("GET /set/timer/end") >= 0)
            {
              String arg = header.substring(header.indexOf("$")+1,header.length()-header.indexOf("$"));
              arg = arg.substring(0,arg.indexOf(" HTTP"));
              timer_end = arg.toInt();
              Serial.println("Timer_END: "+arg);
            }
         
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  delay(500);
  Serial.println(timer_start);
   // Serial.println("Timer_Start: "+timer_start);
     // Serial.println("Timer_End: "+timer_end);
     //   Serial.println("Timer_Type: "+timer_type);
  if(timer_type > 0)
  {
    if(timer_now+1 < timer_end && timer_start == 0)
    {
      timer_now++;
      if(timer_type == 3)
      {
        digitalWrite(5, HIGH);
      } else if(timer_type == 4)
      {
        digitalWrite(4, HIGH);
      }
    }
    else if(timer_start > 0)
    {
      timer_start = timer_start-1;
    }
    else
    {
      if(timer_type == 1)           // Timer - USB 1
      {
        digitalWrite(5, LOW);
        timer_type = 0;
        timer_end = 0;
        timer_now = 0;     
      }
      else if(timer_type == 2)      // Timer - USB 2
      {
        digitalWrite(4, LOW);
        timer_type = 0;
        timer_end = 0;
        timer_now = 0;    
      }      
      else if(timer_type == 3)           // Timer - USB 1
      {
        digitalWrite(5, LOW);
        timer_type = 0;
        timer_end = 0;
        timer_now = 0;     
      }
      else if(timer_type == 4)      // Timer - USB 2
      {
        digitalWrite(4, LOW);
        timer_type = 0;
        timer_end = 0;
        timer_now = 0;    
      }      
    }
  }
}
