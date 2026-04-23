



<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Robot Controller</title>
  <link rel="icon" href="favicon.ico">
  <link rel="stylesheet" href="/style.css">
  <script src="/script.js" defer></script> <!-- Ensure the JavaScript file is loaded after the DOM is ready -->
</head>
<body>
  <h1>Robot Control Panel</h1>
  
  <!-- Motor Control Buttons -->
  <h2>Motor Control</h2>
  <div class="container">
    <button class="button" onclick="sendCommand('/forwards5')">FORWARDS<br>5cm</button>
    <button class="button" onclick="sendCommand('/forwards20')">FORWARDS<br>20cm</button>
    <button class="button button2" onclick="sendCommand('/backwards5')">BACKWARDS<br>5cm</button>
    <button class="button button2" onclick="sendCommand('/backwards20')">BACKWARDS<br>20cm</button>
  </div>

  <!-- Compass Control -->
  <h2>Compass Control</h2>
  <div>
    <input 
      type="range" 
      min="-360" 
      max="360" 
      step="5" 
      id="compassSlider" 
      oninput="updateCompass(this.value)" 
      onchange="sendCompassValue(this.value)" 
      value="0"
    />
    <div id="compassValue">0°</div>
  </div>

  <!-- New Find North Button -->
  <h2>Find North</h2>
  <button class="button" onclick="sendCommand('/findNorth')">Find North</button>
</body>
</html>


------------------------------------

// Function to send a specific command to the server
function sendCommand(route) {
	fetch(route)
	  .then(response => {
		if (response.ok) {
		  console.log("Command sent:", route); // Log success
		} else {
		  console.error("Failed to send command:", route); // Log error
		}
	  })
	  .catch(error => console.error("Error sending command:", error)); // Handle network errors
  }
  
  // Function for "Find North" functionality
  function findNorth() {
	sendCommand('/findNorth'); // Send a request to the server to find north
	console.log("Find North command sent"); // Log for debugging
  }
  
  // Compass Slider Functions
  const compassSlider = document.getElementById('compassSlider');
  if (compassSlider) {
	const middleValue = 0;
	compassSlider.value = middleValue; // Reset slider to 0
	updateCompass(middleValue);       // Update initial value display
  }
  
  function updateCompass(pos) {
	const compassDisplay = document.getElementById('compassValue');
	if (compassDisplay) {
	  compassDisplay.innerText = `${pos}°`;
	}
  }
  
  function sendCompassValue(pos) {
	fetch(`/compass?value=${pos}`)
	  .then(response => {
		if (response.ok) {
		  console.log("Compass value sent:", pos);
		} else {
		  console.error("Failed to send compass value:", pos);
		}
	  })
	  .catch(error => console.error("Error sending compass value:", error));
  }
  
  // Motor Movement Functions
  function move(dir, dis) {
	const command = `/${dir}${dis}`;
	fetch(command)
	  .then(response => {
		if (response.ok) {
		  console.log("Drive command sent:", dir, dis);
		} else {
		  console.error("Failed to send drive command:", dir, dis);
		}
	  })
	  .catch(error => console.error("Error sending drive command:", error));
  }
  
  function forwards5() { move('forwards', 5); }
  function forwards20() { move('forwards', 20); }
  function backwards5() { move('backwards', 5); }
  function backwards20() { move('backwards', 20); }
  

--------------------------------------------------------

#include <ESP8266WiFi.h>        // Library for Wi-Fi functionality
#include <ESP8266WebServer.h>   // Library to create and manage a web server
#include <FS.h>                 // Library for working with file systems (SPIFFS)

const char* ssid = "SOURCE";         // Wi-Fi network name (SSID)
const char* password = "Pelle!23";  // Wi-Fi network password

ESP8266WebServer server(80);    // Create an instance of the WebServer on port 80 (default HTTP port)

void setup() {
  Serial.begin(9600); 

  // Initialize the file system (SPIFFS) on the ESP8266
  if (!SPIFFS.begin()) {        
    Serial.println("Error while mounting SPIFFS"); 
    return; 
  }

  // Connect to the Wi-Fi network
  Serial.print("\nConnecting to " + (String)ssid);
  WiFi.begin(ssid, password);               
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    Serial.print(".");  
  } 
  Serial.println("\nIP address: " + WiFi.localIP().toString()); 

  // Serve static files
  server.serveStatic("/", SPIFFS, "/index.html");             
  server.serveStatic("/style.css", SPIFFS, "/style.css");     
  server.serveStatic("/script.js", SPIFFS, "/script.js");     
  server.serveStatic("/favicon.ico", SPIFFS, "/favicon.png"); 

  // Movement routes
  server.on("/forwards5", [](){ handleMove(5); });      
  server.on("/forwards20", [](){ handleMove(20); });    
  server.on("/backwards5", [](){ handleMove(-5); });    
  server.on("/backwards20", [](){ handleMove(-20); });  

  // Compass control
  server.on("/compass", handleCompass);    

  // Find North route
  server.on("/findNorth", handleFindNorth);  // New route for finding north

  // Handle undefined routes
  server.onNotFound(handleNotFound);

  // Start the web server
  server.begin();
}

void loop() {
  server.handleClient();  // Handle incoming HTTP requests
}

// Handle undefined routes
void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found"); 
}

// Handle movement commands
void handleMove(int distance) {
  Serial.println("Move: " + String(distance));  // Log movement distance
  server.send(200);                             // Send response
}

// Handle compass commands
void handleCompass() {
  if (server.hasArg("value")) {                     
    String valueString = server.arg("value");       
    Serial.println("Turn: " + valueString);         
  }
  server.send(200);
}

// Handle "Find North" command
void handleFindNorth() {
  Serial.println("FindingNorth");  // Log the command
  // Add specific logic to find north if needed (e.g., controlling hardware, sending I2C commands, etc.)
  server.send(200, "text/plain", "FindingNorth");  // Send response to the client
}


----------------------------------------------

#include <Wire.h>
#include <LiquidCrystal.h>

// Motor and encoder pin definitions
const int motorL_pwm = 9;    
const int motorR_pwm = 10;   
const int motorL_dir = 7;    
const int motorR_dir = 8;    
const int encoderL_pin = 2;  
const int encoderR_pin = 3;  

// LCD screen pins
const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Encoder pulse and distance variables
volatile long pulseCountL = 0;   
volatile long pulseCountR = 0;   
const float pulsesPerCm = 10;    // Pulses per cm of movement

// Compass variables
#define SLAVE_ADDRESS 0x60
#define REGISTER_NUMBER 0x01
const int TOLERANCE = 2;         

// Function prototypes
void driveForward();
void driveBackward();
void stopCar();
void turnLeft();
void turnRight();
void countLeftPulses();
void countRightPulses();
void handleTurnCommand(int targetHeading);
void handleDistanceCommand(int distance);
int get_bearing();
void displayBearingAndDirection(int bearing);
int calculateShortestRotation(int currentBearing, int targetHeading);
void findNorth();

void setup() {
  lcd.begin(20, 4);  // Initialize the LCD
  Wire.begin();      // Start I2C for compass
  Serial.begin(9600);

  // Set motor and encoder pins
  pinMode(motorL_pwm, OUTPUT);
  pinMode(motorR_pwm, OUTPUT);
  pinMode(motorL_dir, OUTPUT);
  pinMode(motorR_dir, OUTPUT);
  pinMode(encoderL_pin, INPUT);
  pinMode(encoderR_pin, INPUT);

  // Attach interrupts to encoder pins
  attachInterrupt(digitalPinToInterrupt(encoderL_pin), countLeftPulses, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderR_pin), countRightPulses, RISING);

  lcd.print("Ready for input");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    lcd.clear();
    lcd.print(command);

    // Handling commands sent from the ESP8266
    if (command.startsWith("Move:")) {
      int distance = command.substring(5).toInt();
      handleDistanceCommand(distance);
    } 
    else if (command.startsWith("Turn:")) {
      int targetHeading = command.substring(5).toInt();
      handleTurnCommand(targetHeading);
    } 
    else if (command.startsWith("FindingNorth")) {  // New "Find North" command
      findNorth();
    } 
    else {
      lcd.clear();
      lcd.print("Invalid command");
      Serial.println("Invalid command. Use 'Move:number', 'Turn:number', or 'FindNorth'");
    }
  }
}

// Handles the Turn command
void handleTurnCommand(int targetHeading) {
  int currentBearing = get_bearing();
  int diff = calculateShortestRotation(currentBearing, targetHeading);

  while (abs(diff) > TOLERANCE) {
    if (diff > 0) {
      turnLeft();
    } else {
      turnRight();
    }
    delay(100);

    currentBearing = get_bearing();
    diff = calculateShortestRotation(currentBearing, targetHeading);
    displayBearingAndDirection(currentBearing);
  }
  stopCar();
}

// Handles the Move command
void handleDistanceCommand(int distance) {
  long targetPulses = abs(distance) * pulsesPerCm;
  pulseCountL = 0;
  pulseCountR = 0;

  // Determine direction based on distance
  if (distance > 0) {
    driveForward();
  } else if (distance < 0) {
    driveBackward();
  } else {
    Serial.println("Distance is zero, no movement.");
  }

  // Wait until the required distance is covered
  while (pulseCountL < targetPulses && pulseCountR < targetPulses) {
    lcd.setCursor(0, 1);
    lcd.print("L-Pulses: ");
    lcd.print(pulseCountL);
    lcd.setCursor(0, 2);
    lcd.print("R-Pulses: ");
    lcd.print(pulseCountR);
    lcd.setCursor(0, 3);
    lcd.print("Dist: ");
    lcd.print(pulseCountL / pulsesPerCm, 1);

    Serial.print("Left Pulse Count: ");
    Serial.println(pulseCountL);
    Serial.print("Right Pulse Count: ");
    Serial.println(pulseCountR);

    delay(200);
  }

  stopCar();
}

// "Find North" functionality
void findNorth() {
  Serial.println("Finding North...");
  lcd.clear();
  lcd.print("Finding North");

  handleTurnCommand(0);  // Align to north (0 degrees)

  lcd.clear();
  lcd.print("North Found");
  Serial.println("North Found");
}

// Motor control functions
void driveForward() {
  Serial.println("Driving forward");
  digitalWrite(motorL_dir, HIGH);
  digitalWrite(motorR_dir, HIGH);
  analogWrite(motorL_pwm, 128);  // Set motor speed to 50%
  analogWrite(motorR_pwm, 128);
}

void driveBackward() {
  Serial.println("Driving backward");
  digitalWrite(motorL_dir, LOW);
  digitalWrite(motorR_dir, LOW);
  analogWrite(motorL_pwm, 128);  // Set motor speed to 50%
  analogWrite(motorR_pwm, 128);
}

void turnLeft() {
  digitalWrite(motorL_dir, HIGH);
  digitalWrite(motorR_dir, LOW);
  analogWrite(motorL_pwm, 50);   // Set motor speed to 20%
  analogWrite(motorR_pwm, 50);
}

void turnRight() {
  digitalWrite(motorL_dir, LOW);
  digitalWrite(motorR_dir, HIGH);
  analogWrite(motorL_pwm, 50);   // Set motor speed to 20%
  analogWrite(motorR_pwm, 50);
}

void stopCar() {
  Serial.println("Stopping car");
  analogWrite(motorL_pwm, 0);    // Stop both motors
  analogWrite(motorR_pwm, 0);
}

// Interrupt service routines for counting encoder pulses
void countLeftPulses() { pulseCountL++; }
void countRightPulses() { pulseCountR++; }

// Functions for compass bearing and direction
int get_bearing() {
  uint8_t rawData = readFromRegister(REGISTER_NUMBER);
  int bearing = map(rawData, 0, 255, 0, 360);
  Serial.print("Raw compass data: ");
  Serial.print(rawData);
  Serial.print(" -> Mapped bearing: ");
  Serial.println(bearing);
  return bearing;
}

uint8_t readFromRegister(uint8_t reg) {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(SLAVE_ADDRESS, 1);
  if (Wire.available()) {
    return Wire.read();
  } else {
    Serial.println("Error reading compass data");
    return 0;
  }
}

void displayBearingAndDirection(int bearing) {
  String direction = getDirection(bearing);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Degrees: ");
  lcd.print(bearing);
  lcd.setCursor(0, 1);
  lcd.print("Direction: ");
  lcd.print(direction);
  Serial.print("Compass Bearing: ");
  Serial.print(bearing);
  Serial.print(" Direction: ");
  Serial.println(direction);
}

int calculateShortestRotation(int currentBearing, int targetHeading) {
  int diff = targetHeading - currentBearing;
  if (diff > 180) diff -= 360;
  else if (diff < -180) diff += 360;
  return diff;
}

String getDirection(int bearing) {
  if (bearing >= 0 && bearing < 23) return "N";
  else if (bearing >= 23 && bearing < 68) return "NE";
  else if (bearing >= 68 && bearing < 113) return "E";
  else if (bearing >= 113 && bearing < 158) return "SE";
  else if (bearing >= 158 && bearing < 203) return "S";
  else if (bearing >= 203 && bearing < 248) return "SW";
  else if (bearing >= 248 && bearing < 293) return "W";
  else if (bearing >= 293 && bearing < 338) return "NW";
  else return "N";
}

