//BASE CODE WRITTEN BY SCOTT FITZGERALD

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

char ssid[] = "YOUR_WIFI_NAME"; // your network SSID (name)
char pass[] = "YOUR_WIFI_PASSWORD"; // your network password

HTTPClient client;

struct server{
   char  name[50];
   char  city[50];
   int   rgb[3];
   int   LEDnum;
} ; 
struct server rhizome = {"Rhizome", "Los Angeles", {255,0,0}, 11};
struct server forArtists = {"Solar Power for Artists", "Brooklyn", {186,184,108}, D2};
struct server serverList[] = {{rhizome},{forArtists}};



unsigned long previousMillis = 0;  // will store last time client req was updated
const long interval = 60000;       // interval at which to make a req (milliseconds)

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; };
  pinMode(rhizome.LEDnum,OUTPUT);
  pinMode(forArtists.LEDnum,OUTPUT);
  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // log on to network
  WiFi.begin(ssid, pass);

  // wait to get on
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  // print put local IP address and verify port
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // start the client with the URL we're going to call
  client.begin("https://solarprotocol.net/api/v2/opendata.php?systemInfo=name");  
  // make the request
  makeRequest();
}

void loop() {
  // only call it every minute
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the time you made the req
    previousMillis = currentMillis;
    // make the request! 
    String currServerName= makeRequest();
    for(int i=0;i<2;i++){
      struct server currLoopServer=serverList[i];
      String currLoopName=currLoopServer.name;
      Serial.println("HERE " +currLoopName);
      if(currServerName==currLoopName){
        digitalWrite(currLoopServer.LEDnum,HIGH);
        Serial.println("LIGHT IT UP!");
      }
      else{ //turn off everything else in case of a switch of servers
        digitalWrite(currLoopServer.LEDnum, LOW);
        Serial.println("POWER IT DOWN");
      }

    }
    
  }
}

String makeRequest() {
  Serial.println("client GET...");
  // start connection and send HTTP header
  int httpCode = client.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been sent and the server response header has been handled
    Serial.print("HTTP GET... code:");
    Serial.println(httpCode);

    // if the file is found at the server
    if (httpCode == HTTP_CODE_OK) {
      // parse the data
      String payload = client.getString();
      return parseJSON(payload);
    }
  } else { // if we have a problem, report it
    Serial.print("HTTP GET... failed, error: ");
    Serial.println(client.errorToString(httpCode).c_str());
  }
  // close the connection
  client.end();
}

// fxn that parses the data
String parseJSON(String response) {
  // create a new JSON datatype with the info from the server 
  JSONVar JSONresp = JSON.parse(response);
  // print it out!
  Serial.println(JSONresp);

  // print the keys
  Serial.print("keys: ");
  Serial.println(JSONresp.keys());

  /*
    ******** 
    this JSON has the following structure :
    people : [craft : String, name : String]
    number : int
    message : String
    ******** 
  */

  // get the type of the key "message"
  Serial.println(JSON.typeof(JSONresp["name"]));

  // no need to cast strings to print it out
  String s = JSONresp["name"];
  Serial.print("name is: ");
  Serial.println(s);
  return s;
  // // get the total number of ppl in space
  // int num = int(JSONresp["number"]); // need to cast the data to an int
  // Serial.print("number in space: ");
  // Serial.println(num);

  // // parse the array inside of the "people" key
  // // create a new JSON varaiable
  // JSONVar people;
  // // store the values in the var
  // people = JSONresp["people"];
  // // get the length of the array
  // int peopleLength = people.length();

  // // check the first value in the array
  // Serial.println(people[0]);

  // // using the length of the array, access every bit of info in there
  // for (int x = 0; x < peopleLength; x++) {
  //   Serial.print("value in array at index ");
  //   Serial.print(x);
  //   Serial.print(": ");
  //   Serial.println(people[x]);
  // }

  // // get just the names
  // for (int x = 0; x < peopleLength; x++) {
  //   JSONVar name;
  //   name = people[x]["name"];
  //   Serial.println(name);
  // }
}
