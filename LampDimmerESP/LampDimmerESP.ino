#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80); 

void handleRoot();              // function prototypes for HTTP handlers
void handleLamp();
void handleNotFound();

const char* ssid = "";
const char* password = "";

int led = 2;           
int brightness = 0;   

void setup() {
  pinMode(led, OUTPUT);
  analogWriteFreq(500);
  analogWrite(led, brightness);
  Serial.begin(115200);
  while(!Serial);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/lamp", HTTP_POST, handleLamp); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");
}


void loop() {
  server.handleClient(); 
}

void handleRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200,"text/html", "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>.slider {  -webkit-appearance: none;  width: 100%;  height: 15px;  border-radius: 5px;    background: #d3d3d3;  outline: none;  opacity: 0.7;  -webkit-transition: .2s;  transition: opacity .2s;}.slider::-webkit-slider-thumb {  -webkit-appearance: none;  appearance: none;  width: 25px;  height: 25px;  border-radius: 50%;   background: #4CAF50;  cursor: pointer;}.slider::-moz-range-thumb {  width: 25px;  height: 25px;  border-radius: 50%;  background: #4CAF50;  cursor: pointer;}</style></head><body><h1>Lampka</h1><div class=\"slidecontainer\">  <input type=\"range\" min=\"100\" max=\"1024\" value=\"100\" class=\"slider\" id=\"myRange\" onmouseup=\"sliderHandler(this.value)\" ontouchend=\"sliderHandler(this.value)\">  <p>Value: <span id=\"demo\"></span></p></div><script>function sendData(data) {  var XHR = new XMLHttpRequest();  var urlEncodedData = \"\";  var urlEncodedDataPairs = [];  var name;  for(name in data) {    urlEncodedDataPairs.push(encodeURIComponent(name) + \"=\" + encodeURIComponent(data[name]));  }    urlEncodedData = urlEncodedDataPairs.join(\"&\").replace(/%20/g, \"+\");    XHR.addEventListener(\"load\", function(event) {    console.log(\"Yeah! Data sent and response loaded.\");  });    XHR.addEventListener(\"error\", function(event) {    console.log(\"Oops! Something goes wrong.\");  });    XHR.open(\"POST\", \"/lamp\");    XHR.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");  XHR.send(urlEncodedData);}var slider = document.getElementById(\"myRange\");var output = document.getElementById(\"demo\");output.innerHTML = slider.value;function sliderHandler(val) {  output.innerHTML = val;  sendData({brightness:val});}</script></body></html>");
}

void handleLamp() {                    
  if(server.hasArg("brightness")){
    brightness = server.arg("brightness").toInt();
    Serial.println(brightness);
    if(brightness < 256){
      brightness = 0;
      Serial.println("Changed to 0!");
    }
    analogWrite(led, brightness);
    return;
  }
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
