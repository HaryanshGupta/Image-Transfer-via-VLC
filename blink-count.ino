/*
 * Half-Duplex Optical Communication System
 * Arduino Uno + Laser Diode + BPW34 Photodiode
 * 
 * Hardware Connections:
 * - Laser Diode: Pin 9 (with 220Ω resistor in series)
 * - BPW34 Photodiode: A0 (with 10kΩ pull-down resistor)
 * - Mode Switch Button: Pin 2 (with pull-up resistor)
 * - LED Indicator: Pin 6
 * 
 * Encoding: 
 * - Letters (a-z): 1-26 blinks
 * - Space: 27 blinks
 * - Numbers (0-9): 28-37 blinks
 * - Period (.): 38 blinks
 * - Comma (,): 39 blinks
 * - Question (?): 40 blinks
 * - Exclamation (!): 41 blinks
 */

// Pin definitions
#define LASER_PIN 9
#define RECEIVER_PIN A0
#define MODE_BUTTON 2
#define LED_INDICATOR 6

// Communication parameters
#define BAUD_RATE 9600
#define BLINK_DURATION 200    // Duration of each blink (ms)
#define BLINK_PAUSE 150       // Pause between blinks (ms)
#define LETTER_PAUSE 500      // Pause between letters (ms)
#define THRESHOLD 30         // Analog threshold for light detection
#define ALIGNMENT_TIME 10000   // 10 seconds for alignment

// Communication modes
enum Mode {
  TRANSMIT,
  RECEIVE
};

Mode currentMode = RECEIVE;
unsigned long lastModeChange = 0;
const unsigned long debounceDelay = 200;
bool systemReady = false;

void setup() {
  // Initialize pins
  pinMode(LASER_PIN, OUTPUT);
  pinMode(RECEIVER_PIN, INPUT);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(LED_INDICATOR, OUTPUT);
  
  // Initialize serial communication
  Serial.begin(BAUD_RATE);
  
  digitalWrite(LASER_PIN, LOW);
  digitalWrite(LED_INDICATOR, LOW);
  
  // Alignment sequence
  
  Serial.println("Optical Communication System ");
  
  Serial.println();
  Serial.println("Alignment Mode");

  digitalWrite(LASER_PIN, HIGH);
  digitalWrite(LED_INDICATOR, HIGH);
  
  for (int i = 10; i > 0; i--) {
    Serial.print(i);
  
    delay(1000);
  }
  
  digitalWrite(LASER_PIN, LOW);
  digitalWrite(LED_INDICATOR, LOW);
  
  Serial.println();
  Serial.println("complete!");
  Serial.println("ready");
  Serial.println();
  Serial.println("mode: RX");
  Serial.println("Press button to change mode");
  

  
  systemReady = true;
  
  // Attach interrupt for mode button
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), toggleMode, FALLING);
}

void loop() {
  if (!systemReady) return;
  
  if (currentMode == TRANSMIT) {
    handleTransmit();
  } else {
    handleReceive();
  }
}

// Toggle between transmit and receive modes
void toggleMode() {
  if (!systemReady) return;
  
  unsigned long currentTime = millis();
  if (currentTime - lastModeChange > debounceDelay) {
    currentMode = (currentMode == TRANSMIT) ? RECEIVE : TRANSMIT;
    lastModeChange = currentTime;
    
    digitalWrite(LASER_PIN, LOW);
    
    if (currentMode == TRANSMIT) {
      digitalWrite(LED_INDICATOR, HIGH);
      Serial.println();
      Serial.println("Mode TX");
      Serial.println("Type message...");
    } else {
      digitalWrite(LED_INDICATOR, LOW);
      Serial.println();
      Serial.println("Mode RX");
      Serial.println("Listening");
    }
    Serial.println();
  }
}

// Handle transmit mode
void handleTransmit() {
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    message.trim();
    message.toLowerCase(); // Convert to lowercase
    
    if (message.length() > 0) {
      Serial.print("TX: ");
      Serial.println(message);
      transmitData(message);
      Serial.println("TX complete.");
      Serial.println();
    }
  }
}

// Transmit data via laser using blink encoding
void transmitData(String data) {
  for (int i = 0; i < data.length(); i++) {
    char c = data[i];
    int blinkCount = 0;
    
    // Handle letters (a-z)
    if (c >= 'a' && c <= 'z') {
      blinkCount = c - 'a' + 1; // a=1, b=2, ... z=26
    }
    // Handle space
    else if (c == ' ') {
      blinkCount = 27;
    }
    // Handle numbers (0-9)
    else if (c >= '0' && c <= '9') {
      blinkCount = 28 + (c - '0'); // 0=28, 1=29, ... 9=37
    }
    // Handle punctuation
    else if (c == '.') {
      blinkCount = 38;
    }
    else if (c == ',') {
      blinkCount = 39;
    }
    else if (c == '?') {
      blinkCount = 40;
    }
    else if (c == '!') {
      blinkCount = 41;
    }
    
    // Transmit the blinks if valid character
    if (blinkCount > 0) {
      // Blink the laser
      for (int j = 0; j < blinkCount; j++) {
        digitalWrite(LASER_PIN, HIGH);
        delay(BLINK_DURATION);
        digitalWrite(LASER_PIN, LOW);
        
        if (j < blinkCount - 1) {
          delay(BLINK_PAUSE);
        }
      }
      
      // Pause between characters
      delay(LETTER_PAUSE);
    }
  }
  
  digitalWrite(LASER_PIN, LOW);
}

// Handle receive mode
void handleReceive() {
  String receivedMessage = "";
  unsigned long lastBlinkTime = millis();
  int blinkCount = 0;
  bool wasLight = false;
  bool messageStarted = false;
  
  while (currentMode == RECEIVE) {
    int lightLevel = analogRead(RECEIVER_PIN);
    bool isLight = (lightLevel > THRESHOLD);
    unsigned long currentTime = millis();
    
    // Detect rising edge (light turned on)
    if (isLight && !wasLight) {
      blinkCount++;
      lastBlinkTime = currentTime;
      messageStarted = true;
    }
    
    wasLight = isLight;
    
    // Check if character is complete
    if (messageStarted && (currentTime - lastBlinkTime > LETTER_PAUSE)) {
      if (blinkCount > 0) {
        char receivedChar = ' ';
        
        // Convert blink count to character
        if (blinkCount >= 1 && blinkCount <= 26) {
          // Letters a-z
          receivedChar = 'a' + (blinkCount - 1);
        }
        else if (blinkCount == 27) {
          // Space
          receivedChar = ' ';
        }
        else if (blinkCount >= 28 && blinkCount <= 37) {
          // Numbers 0-9
          receivedChar = '0' + (blinkCount - 28);
        }
        else if (blinkCount == 38) {
          receivedChar = '.';
        }
        else if (blinkCount == 39) {
          receivedChar = ',';
        }
        else if (blinkCount == 40) {
          receivedChar = '?';
        }
        else if (blinkCount == 41) {
          receivedChar = '!';
        }
        
        receivedMessage += receivedChar;
        blinkCount = 0;
      }
      
      // Check if message is complete (long pause)
      if (receivedMessage.length() > 0 && (currentTime - lastBlinkTime > LETTER_PAUSE * 3)) {
        Serial.print("RX: ");
        Serial.println(receivedMessage);
        Serial.println();
        receivedMessage = "";
        messageStarted = false;
      }
    }
    
    delay(10); // Small delay to avoid overwhelming the system
  }
}
