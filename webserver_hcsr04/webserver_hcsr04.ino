#include <ESP8266WiFi.h> 
#include <Ultrasonic.h>
#include <Arduino.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

Ultrasonic ultrasonic(5, 4);
long distancia;

int RED = D0;
int GREEN = D4;
int BLUE = D3;

const char* ssid = "GOMES";
const char* password = "00888050267"; 

AsyncWebServer server(8091);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Servidor HC-SR04</h2>
  <p>
    <i class="fas fa-person-booth" style="color:#059e8a;"></i> 
    <span class="dht-labels">Distancia em cm </span> 
    <span id="distance">%DISTANCE%</span>
  </p>
  <p id="isDistance"></p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      console.log(this.responseText);
      document.getElementById("distance").innerHTML = this.responseText;
      if(this.responseText <= 10){
        document.getElementById("isDistance").innerHTML = "Muito perto!!";
      } else if (this.responseText > 10 && this.responseText <= 20 ){
        document.getElementById("isDistance").innerHTML = "Nem muito perto e nem muito longe!!";
      }else{
        document.getElementById("isDistance").innerHTML = "Muito longe!!";
      }
    }
  };
  xhttp.open("GET", "/distance", true);
  xhttp.send();
}, 500 ) ;
</script>
</html>)rawliteral";


String processor(const String& var){
  Serial.println(var);
  if(var == "DISTANCE"){
    return String(distancia);
  }
  return String();
}


void setup() {
  Serial.begin(115200); 
  delay(10); 
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  Serial.println("");
  Serial.println("");
  Serial.print("Conectando a "); 
  Serial.print(ssid);

  WiFi.begin(ssid, password); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI FAZER A CONEXÃO COM A REDE SEM FIO

  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print(".");
  }
  Serial.println(""); 
  Serial.print("Conectado a rede sem fio ");
  Serial.println(ssid);
  server.begin(); //INICIA O SERVIDOR PARA RECEBER DADOS NA PORTA DEFINIDA EM "WiFiServer server(porta);"
  Serial.println("Servidor iniciado");
  
  Serial.print("IP para se conectar ao NodeMCU: "); 
  Serial.print("http://"); 
  Serial.println(WiFi.localIP()); //ip dinamico


// Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/distance", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(distancia).c_str());
  });

  // Start server
  server.begin();
  
}
void loop() {
  distancia = ultrasonic.Ranging(CM);         
  Serial.print(distancia);
  Serial.println("cm");

  if (distancia <= 10){
    analogWrite(RED, 255);
    analogWrite(GREEN, 0);
    analogWrite(BLUE, 0);
  } else if ( distancia > 10 && distancia <= 20 ){
     analogWrite(RED, 255);
     analogWrite(GREEN, 255);
     analogWrite(BLUE, 0); 
  }else{
    analogWrite(RED, 0);
    analogWrite(GREEN, 255);
    analogWrite(BLUE, 0);
  }
  delay(100);
}
