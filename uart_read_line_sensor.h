#include "esphome.h"

class UartReadLineSensor : public Component, public UARTDevice, public TextSensor {
 public:
  UartReadLineSensor(UARTComponent *parent) : UARTDevice(parent) {}

  void setup() override {
    // nothing to do here
  }

  int readline(char *buffer, int len)
  {
    static int pos = 0;
    int rpos;
    char readch;

    while (available()) {
      readch = read();
      if (readch > 0) {
        buffer[pos++] = readch;
        buffer[pos] = 0;
        if (strstr(buffer, "G01gA") != nullptr || strstr(buffer, "G02gA") != nullptr ||
            strstr(buffer, "G03gA") != nullptr || strstr(buffer, "G04gA") != nullptr) {
          rpos = pos;
          pos = 0;  // Reset position index ready for next time
          return rpos;
        }
        if (pos >= len - 1) {
          // Buffer overflow, reset position index
          pos = 0;
          return -2;
        }
      }
    }
    // No end of line has been found, so return -1.
    return -1;
  }

  void loop() override {
    const int max_line_length = 80;
    static char buffer[max_line_length];
    int result = readline(buffer, max_line_length);
    if (result > 0) {
      publish_state(buffer);
    } else if (result == -2) {
      // Handle buffer overflow
      // This could be a good place to log an error or take appropriate action
    }
  }
};
