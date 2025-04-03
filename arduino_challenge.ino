#include <SD.h>  // Including SD library

// Pin Definitions
const int PIN_BTN_TEST = 2;         // Example pin for test button
const int PIN_UBX_TIMEPULSE = 4;    // Example pin for GPS timepulse
const int PIN_UBX_SAFEBOOT = 3;     // Example pin for GPS safe boot
const int PIN_UBX_RST_N = 5;        // Example pin for GPS reset
const int SD_CS = 11;               // Chip select for SD card

// Serial initialization
#define DBG_SERIAL Serial        // Using standard Serial for debugging
#define UBX_SERIAL Serial1       // Assuming you use Serial1 for GPS (adjust if needed)

// Status definitions (example, you need to define how you want the status values to work)
#define BUSY 0
#define GOOD 1
#define FAILED 2

// Variables
bool time_fixed = false;
char nmea_buf[256];  // Buffer to store GPS NMEA sentences
int nmea_idx = 0;

// Function prototypes
void set_status(int status);
void process_nmea(char* nmea, int len);
void log_result(bool passed);

void setup() {
  DBG_SERIAL.begin(115200);  // Initialize serial port for debugging output
  UBX_SERIAL.begin(9600);    // Initialize serial communication with GPS module

  // Initialize pins
  pinMode(PIN_BTN_TEST, INPUT);
  pinMode(PIN_UBX_TIMEPULSE, INPUT);
  pinMode(PIN_UBX_SAFEBOOT, OUTPUT);
  pinMode(PIN_UBX_RST_N, OUTPUT);

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    DBG_SERIAL.println("SD card initialization failed!");
    return;
  }

  // Set up GPS module
  digitalWrite(PIN_UBX_SAFEBOOT, LOW);  // Disable GPS safe boot
  digitalWrite(PIN_UBX_RST_N, HIGH);    // Reset GPS module

  set_status(BUSY); // Set status to busy while setup is ongoing
}

void loop() {
  while (UBX_SERIAL.available()) {
    nmea_buf[nmea_idx++] = UBX_SERIAL.read();  // Read GPS data byte-by-byte
    if (nmea_buf[nmea_idx - 1] == '\n') {     // End of NMEA sentence
      process_nmea(nmea_buf, nmea_idx);        // Process the NMEA sentence
      nmea_idx = 0;  // Reset buffer index
    }
  }

  if (!time_fixed) return;  // If time isn't fixed, return

  // Perform test if the button is pressed
  if (digitalRead(PIN_BTN_TEST) == LOW) {
    set_status(BUSY);  // Set status to busy

    // Simulate pin testing (can be replace this with actual test code)
    for (int i = 0; i < 10; i++) {
      DBG_SERIAL.print("Pin ");
      DBG_SERIAL.print(i);
      DBG_SERIAL.println(": Testing...");
    }

    set_status(GOOD);  // Set status to good if test passes
    log_result(true);   // Log result (true for pass, false for fail)
  }
}

// Set status function
void set_status(int status) {
  switch (status) {
    case BUSY:
      DBG_SERIAL.println("Status: BUSY");
      break;
    case GOOD:
      DBG_SERIAL.println("Status: GOOD");
      break;
    case FAILED:
      DBG_SERIAL.println("Status: FAILED");
      break;
    default:
      DBG_SERIAL.println("Unknown status");
  }
}

// Process NMEA sentence function
void process_nmea(char* nmea, int len) {
  DBG_SERIAL.print("Received NMEA: ");
  for (int i = 0; i < len; i++) {
    DBG_SERIAL.print(nmea[i]);
  }
  DBG_SERIAL.println();
  
  // Added parsing logic for NMEA sentences here
  time_fixed = true;  // For example, assuming the time is fixed after receiving NMEA data
}

// Log result to SD card (example)
void log_result(bool passed) {
  File logFile = SD.open("result.txt", FILE_WRITE);
  if (logFile) {
    logFile.println(passed ? "Harness passed!" : "Harness failed!");
    logFile.close();
  } else {
    DBG_SERIAL.println("Failed to write to SD card.");
  }
}
