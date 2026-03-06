/*
 * Half-Duplex Optical Communication System - Morse Code
 * Arduino Uno + Laser Diode + BPW34 Photodiode
 * 
 * Hardware Connections:
 * - Laser Diode: Pin 9 (with 220Ω resistor in series)
 * - BPW34 Photodiode: A0 (with 10kΩ pull-down resistor)
 * - Mode Switch Button: Pin 2 (with pull-up resistor)
 * - LED Indicator: Pin 6
 * 
 * Encoding: International Morse Code
 */

// Pin definitions
#define LASER_PIN 9
#define RECEIVER_PIN A0
#define MODE_BUTTON 2
#define LED_INDICATOR 6

// Communication parameters
#define BAUD_RATE 9600
#define THRESHOLD 50          // Analog threshold for light detection
#define ALIGNMENT_TIME 5000   // 5 seconds for alignment

// Morse code timing (standard timing ratios)
#define DIT_DURATION 200      // Duration of a dot
#define DAH_DURATION 600      // Duration of a dash (3x dit)
#define SYMBOL_GAP 200        // Gap between dots/dashes in same letter
#define LETTER_GAP 600        // Gap between letters (3x dit)
#define WORD_GAP 1400         // Gap between words (7x dit)

// Communication modes
enum Mode {
  NONE,
  TRANSMIT,
  RECEIVE
};

Mode currentMode = NONE;
unsigned long lastModeChange = 0;
const unsigned long debounceDelay = 200;
bool systemReady = false;

// Morse code lookup table
struct MorseChar {
  char character;
  const char* code;
};

const MorseChar morseTable[] = {
  {'a', ".-"},    {'b', "-..."},  {'c', "-.-."},  {'d', "-.."},
  {'e', "."},     {'f', "..-."},  {'g', "--."},   {'h', "...."},
  {'i', ".."},    {'j', ".---"},  {'k', "-.-"},   {'l', ".-.."},
  {'m', "--"},    {'n', "-."},    {'o', "---"},   {'p', ".--."},
  {'q', "--.-"},  {'r', ".-."},   {'s', "..."},   {'t', "-"},
  {'u', "..-"},   {'v', "...-"},  {'w', ".--"},   {'x', "-..-"},
  {'y', "-.--"},  {'z', "--.."},
  {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
  {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
  {'8', "---.."}, {'9', "----."},
  {' ', " "}
};

const int morseTableSize = sizeof(morseTable) / sizeof(MorseChar);

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

  Serial.println();
  Serial.println("Alignment Mode");

  
  digitalWrite(LASER_PIN, HIGH);
  digitalWrite(LED_INDICATOR, HIGH);
  
  for (int i = 5; i > 0; i--) {
    Serial.print(i);
    Serial.println(" seconds");
    delay(1000);
  }
  
  digitalWrite(LASER_PIN, LOW);
  digitalWrite(LED_INDICATOR, LOW);
  
  Serial.println();
  Serial.println("done");
  Serial.println("Ready");
  Serial.println();
  Serial.println("Press button to select mode");
  
  systemReady = true;
  
  // Attach interrupt for mode button
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), toggleMode, FALLING);
}

void loop() {
  if (!systemReady) return;
  
  if (currentMode == TRANSMIT) {
    handleTransmit();
  } else if (currentMode == RECEIVE) {
    handleReceive();
  }
}

// Toggle between modes
void toggleMode() {
  if (!systemReady) return;
  
  unsigned long currentTime = millis();
  if (currentTime - lastModeChange > debounceDelay) {
    
    // Cycle: NONE -> TRANSMIT -> RECEIVE -> TRANSMIT -> ...
    if (currentMode == NONE) {
      currentMode = TRANSMIT;
    } else if (currentMode == TRANSMIT) {
      currentMode = RECEIVE;
    } else {
      currentMode = TRANSMIT;
    }
    
    lastModeChange = currentTime;
    digitalWrite(LASER_PIN, LOW);
    
    if (currentMode == TRANSMIT) {
      digitalWrite(LED_INDICATOR, HIGH);
      Serial.println();
      Serial.println(" Mode TX");
      Serial.println("Type message");
    } else if (currentMode == RECEIVE) {
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
    message.toLowerCase();
    
    if (message.length() > 0) {
      Serial.print("TX: ");
      Serial.print(message);
      Serial.print(" -> ");
      transmitData(message);
      Serial.println();
      Serial.println("TX complete.");
      Serial.println();
    }
  }
}

// Get Morse code for a character
const char* getMorseCode(char c) {
  for (int i = 0; i < morseTableSize; i++) {
    if (morseTable[i].character == c) {
      return morseTable[i].code;
    }
  }
  return NULL;
}

// Transmit data via laser using Morse code
void transmitData(String data) {
  for (int i = 0; i < data.length(); i++) {
    char c = data[i];
    
    // Handle space - word gap
    if (c == ' ') {
      Serial.print("/ ");
      delay(WORD_GAP);
      continue;
    }
    
    // Get Morse code for character
    const char* morse = getMorseCode(c);
    
    if (morse != NULL) {
      Serial.print(morse);
      Serial.print(" ");
      
      // Transmit each symbol
      for (int j = 0; morse[j] != '\0'; j++) {
        if (morse[j] == '.') {
          // Dit
          digitalWrite(LASER_PIN, HIGH);
          delay(DIT_DURATION);
          digitalWrite(LASER_PIN, LOW);
        } else if (morse[j] == '-') {
          // Dah
          digitalWrite(LASER_PIN, HIGH);
          delay(DAH_DURATION);
          digitalWrite(LASER_PIN, LOW);
        }
        
        // Gap between symbols in same letter
        if (morse[j + 1] != '\0') {
          delay(SYMBOL_GAP);
        }
      }
      
      // Gap between letters
      delay(LETTER_GAP);
    }
  }
  
  digitalWrite(LASER_PIN, LOW);
}

// Get character from Morse code
char getCharFromMorse(String morse) {
  for (int i = 0; i < morseTableSize; i++) {
    if (String(morseTable[i].code) == morse) {
      return morseTable[i].character;
    }
  }
  return '?';
}

// Handle receive mode
void handleReceive() {
  String receivedMessage = "";
  String currentMorse = "";
  unsigned long pulseStart = 0;
  unsigned long pulseEnd = 0;
  unsigned long lastPulseEnd = 0;
  bool inPulse = false;
  
  while (currentMode == RECEIVE) {
    int lightLevel = analogRead(RECEIVER_PIN);
    bool isLight = (lightLevel > THRESHOLD);
    unsigned long currentTime = millis();
    
    // Detect pulse start
    if (isLight && !inPulse) {
      pulseStart = currentTime;
      inPulse = true;
    }
    
    // Detect pulse end
    if (!isLight && inPulse) {
      pulseEnd = currentTime;
      unsigned long pulseDuration = pulseEnd - pulseStart;
      
      // Determine if dit or dah
      if (pulseDuration < (DIT_DURATION + DAH_DURATION) / 2) {
        currentMorse += '.';
      } else {
        currentMorse += '-';
      }
      
      lastPulseEnd = pulseEnd;
      inPulse = false;
    }
    
    // Check for gaps to decode letters and words
    if (!inPulse && currentMorse.length() > 0) {
      unsigned long gap = currentTime - lastPulseEnd;
      
      // Letter gap detected
      if (gap > LETTER_GAP && gap < WORD_GAP) {
        char decodedChar = getCharFromMorse(currentMorse);
        receivedMessage += decodedChar;
        currentMorse = "";
      }
      // Word gap detected
      else if (gap > WORD_GAP) {
        if (currentMorse.length() > 0) {
          char decodedChar = getCharFromMorse(currentMorse);
          receivedMessage += decodedChar;
          currentMorse = "";
        }
        receivedMessage += ' ';
        lastPulseEnd = currentTime; // Reset to avoid multiple spaces
      }
    }
    
    // Output received message after long silence
    if (receivedMessage.length() > 0 && (currentTime - lastPulseEnd > WORD_GAP * 2)) {
      Serial.print("RX: ");
      Serial.println(receivedMessage);
      Serial.println();
      receivedMessage = "";
      currentMorse = "";
    }
    
    delay(10);
  }
}