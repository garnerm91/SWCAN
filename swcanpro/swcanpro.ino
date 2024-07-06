#include <SPI.h>
#include <mcp2515.h>

const int MODE0_PIN = 3;
const int MODE1_PIN = 2;
struct can_frame canMsg2;
MCP2515 mcp2515(10); // Assuming CS pin is 10

enum Command {
  STANDBY,
  RX,
  TX,
  IDN
};

Command currentCommand = STANDBY;
bool sent = false;

void setup() {
  Serial.begin(115200);

  pinMode(MODE0_PIN, OUTPUT);
  pinMode(MODE1_PIN, OUTPUT);

  // Set the TH8056 to 33.3 Kbps mode
  digitalWrite(MODE0_PIN, HIGH);
  digitalWrite(MODE1_PIN, HIGH);

  // Initialize MCP2515
  mcp2515.reset();
  mcp2515.setBitrate(CAN_33KBPS); // Set the bitrate to 33.3 Kbps
  mcp2515.setNormalMode();

  Serial.println("MCP2515 Initialized at 33.3 Kbps");
}

void checkForCommand() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.equalsIgnoreCase("rx")) {
      currentCommand = RX;
    } else if (command.equalsIgnoreCase("standby")) {
      currentCommand = STANDBY;
    } else if (command.equalsIgnoreCase("tx")) {
      currentCommand = TX;
    } else if (command.equalsIgnoreCase("idn")) {
      currentCommand = IDN;
    } else {
      Serial.println("MSG: Unknown command");
    }
  }
}

void loop() {
  switch (currentCommand) {
    case RX:
      checkForCommand();
      rxdata(); // Continuously call rxdata() in RX mode
      break;
    case STANDBY:
      checkForCommand();
     delay(25); 
      break;
    case TX:
      checkForCommand();
      Serial.println("MSG: Transmitting data...");

      // Example data to send
        canMsg2.can_id  = 0x108;
  canMsg2.can_dlc = 8;
  canMsg2.data[0] = 0x00;
  canMsg2.data[1] = 0x00;
  canMsg2.data[2] = 0x00;
  canMsg2.data[3] = 0x00;
  canMsg2.data[4] = 0x00;
  canMsg2.data[5] = 0x00;
  canMsg2.data[6] = 0x00;
  canMsg2.data[7] = 0x1E;
    mcp2515.sendMessage(&canMsg2);
  mcp2515.setNormalMode();
      currentCommand = STANDBY; // Exit TX mode at the end
      break;
        case IDN:
      // Print identifier and switch to standby mode
      Serial.println("swcan v0.1");
      currentCommand = STANDBY;
      break;
  }
}

void rxdata() {
  // Example RX function, needs proper implementation
  struct can_frame canMsg;
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print("Received CAN frame with ID: ");
    Serial.print(canMsg.can_id, HEX);
    Serial.print(" Data: ");
    for (int i = 0; i < canMsg.can_dlc; i++) {
      Serial.print(canMsg.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}