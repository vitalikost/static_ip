#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

String str = "";

boolean conf = false;
 
String html_header = "<html>\
 <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\
 <head>\
   <title>ESP8266 Settings</title>\
   <style>\
     body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
   </style>\
 </head>";
 
ESP8266WebServer server(80);

void setup(void)
{
    byte len_ssid, len_pass;
   
    delay(3000);
    Serial.begin(115200);
    Serial.println();  
   
    EEPROM.begin(98);
 
    len_ssid = EEPROM.read(96);
    len_pass = EEPROM.read(97);
    if(len_pass > 64) len_pass = 0;
   
  pinMode(4, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  if((len_ssid < 33) && (len_ssid != 0)){
    // Режим STATION
      WiFi.mode( WIFI_STA);
      unsigned char* buf_ssid = new unsigned char[32];
      unsigned char* buf_pass = new unsigned char[64];
      for(byte i = 0; i < len_ssid; i++) buf_ssid[i] = char(EEPROM.read(i));
      buf_ssid[len_ssid] = '\x0';
      const char *ssid  = (const char*)buf_ssid;
      for(byte i = 0; i < len_pass; i++) buf_pass[i] = char(EEPROM.read(i + 32));
      const char *pass  = (const char*)buf_pass;
      buf_pass[len_pass] = '\x0';
      delay(2000);
      Serial.print("SSID: ");
      Serial.print(ssid);
      Serial.print("   ");
      Serial.print("Password: ");
      Serial.println(pass);


      int ip1 = 192;
      int ip2 = 168;
      int ip3 = 49;
      int ip4 = 164; 
      
      int gateway1 = 192;
      int gateway2 = 168;
      int gateway3 = 49;
      int gateway4 = 1; 
      
      int subnet1 = 255;
      int subnet2 = 255;
      int subnet3 = 255;
      int subnet4 = 0; 

     
     //IPAddress ip(192, 168, 49, 164);
     IPAddress ip(ip1, ip2, ip3, ip4);
     //IPAddress gateway(192, 168, 49, 1);
     IPAddress gateway(gateway1, gateway2, gateway3, gateway4);
     //IPAddress subnet(255, 255, 255, 0); 
     IPAddress subnet(subnet1, subnet2, subnet3, subnet4);


     //IPAddress ip_dns1(8, 8, 8, 8);
     //IPAddress ip_dns2(8, 8, 4, 4);

     //WiFi.mode(WIFI_STA);
     //WiFi.config(ip,gateway,subnet,ip_dns1,ip_dns2);     
     WiFi.config(ip,gateway,subnet);



     WiFi.begin(ssid, pass);


      // Wait for connection
      while ( WiFi.status() != WL_CONNECTED ) {
       delay ( 500 );
        Serial.print ( "." );
      }
      Serial.println();
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }
    else // Режим SoftAP
      {
        const char *ssid_ap = "goolooESP"; 
        WiFi.mode(WIFI_AP);
        Serial.print("Configuring access point...");
        /* You can remove the password parameter if you want the AP to be open. */
        WiFi.softAP(ssid_ap);
        delay(2000);
        Serial.println("done");
        IPAddress myIP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(myIP);
        server.on("/", handleRoot);
        server.on("/ok", handleOk);
        server.begin();
        Serial.println("HTTP server started");  
      }  
   
}
void loop() {
     server.handleClient();
  
  // Перевод модуля в режим конфигурации путем замыкания GPIO5 на массу

    if((digitalRead(4) == LOW) && !conf){
    EEPROM.write(96,255);
    EEPROM.commit();
    EEPROM.end();
    conf = true; 
    Serial.println("Please reboot module for coniguration --gpio5--");
    Serial.println(conf);
//    Serial.println(f_state);
  }
  
}
 
void handleRoot() {
  String str = "";
  str += html_header;
  str += "<body>\
   <form method=\"POST\" action=\"ok\">\
     <input name=\"ssid\"> WIFI Net</br>\
     <input name=\"pswd\"> Password</br>\     
     <input name=\"ip1\" minlength=\"1\" maxlength=\"3\" size=\"3\">.\
     <input name=\"ip2\" minlength=\"1\" maxlength=\"3\" size=\"3\">.\
     <input name=\"ip3\" minlength=\"1\" maxlength=\"3\" size=\"3\">.\
     <input name=\"ip4\" minlength=\"1\" maxlength=\"3\" size=\"3\">\
     ip</br>\
     <input name=\"gateway1\" minlength=\"1\" maxlength=\"3\" size=\"3\">.\
     <input name=\"gateway2\" minlength=\"1\" maxlength=\"3\" size=\"3\">.\
     <input name=\"gateway3\" minlength=\"1\" maxlength=\"3\" size=\"3\">.\
     <input name=\"gateway4\" minlength=\"1\" maxlength=\"3\" size=\"3\">\
      gateway</br>\
     <input name=\"subnet1\" minlength=\"1\" maxlength=\"3\" size=\"3\">.\
     <input name=\"subnet2\" minlength=\"1\" maxlength=\"3\" size=\"3\">.\
     <input name=\"subnet3\" minlength=\"1\" maxlength=\"3\" size=\"3\">.\
     <input name=\"subnet4\" minlength=\"1\" maxlength=\"3\" size=\"3\">\
      subnet</br></br>\
     <input type=SUBMIT value=\"Save settings\">\
   </form>\
 </body>\
</html>";
server.send ( 200, "text/html", str );
}
 
void handleOk(){
  String ssid_ap;
  String pass_ap;
  unsigned char* buf = new unsigned char[64];
 
  String str = "";
  str += html_header;
  str += "<body>";
 
  EEPROM.begin(98);
 
  ssid_ap = server.arg(0);
  pass_ap = server.arg(1);
 
  if(ssid_ap != ""){
    EEPROM.write(96,ssid_ap.length());
    EEPROM.write(97,pass_ap.length());
    ssid_ap.getBytes(buf, ssid_ap.length() + 1);
    for(byte i = 0; i < ssid_ap.length(); i++)
      EEPROM.write(i, buf[i]);
      
    pass_ap.getBytes(buf, pass_ap.length() + 1);
    for(byte i = 0; i < pass_ap.length(); i++)
      EEPROM.write(i + 32, buf[i]);
      
    EEPROM.commit();
    EEPROM.end();
   
    str +="Configuration saved in FLASH</br>\
   Changes applied after reboot</p></br></br>\
   <a href=\"/\">Return</a> to settings page</br>";
  }
  else {
    str += "No WIFI Net</br>\
   <a href=\"/\">Return</a> to settings page</br>";
  }
  str += "</body></html>";
  server.send ( 200, "text/html", str );
}
