
#define BUTTON_NEXT    2
#define BUTTON_SELECT  0

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHT_PIN, DHT_TYPE);

// colors

#define C_BG       0x1082
#define C_WHITE    ST77XX_WHITE
#define C_BLACK    ST77XX_BLACK
#define C_PINK     0xF81F
#define C_CYAN     0x07FF
#define C_YELLOW   0xFFE0
#define C_GREEN    0x07E0
#define C_ORANGE   0xFD20
#define C_BLUE     0x435F
#define C_PURPLE   0x901F
#define C_RED      ST77XX_RED

//Pet setting

const char* actions[] = {
  "FEED",
  "PLAY",
  "PET",
  "NAP"
};

const int ACTION_COUNT = 4;

int selectedAction = 0;

int joy = 70;
int energy = 75;
int fullness = 65;

float temperatureC = NAN;
float humidity = NAN;

bool statsView = false;
bool sleeping = false;

unsigned long sleepUntil = 0;
unsigned long lastDHTRead = 0;
unsigned long lastFrame = 0;
unsigned long lastButtonChange = 0;
unsigned long lastAnimation = 0;

int animationFrame = 0;

// Button debounce state
bool lastNextReading = HIGH;
bool stableNext = HIGH;

bool lastSelectReading = HIGH;
bool stableSelect = HIGH;

unsigned long nextChangedAt = 0;
unsigned long selectChangedAt = 0;


int clampStat(int value) {
  return constrain(value, 0, 100);
}

void updateStats(int j, int e, int f) {
  joy = clampStat(joy + j);
  energy = clampStat(energy + e);
  fullness = clampStat(fullness + f);
}

bool buttonPressed(int pin,
                   bool &lastReading,
                   bool &stableState,
                   unsigned long &changedAt) {
  bool reading = digitalRead(pin);
  unsigned long now = millis();

  if (reading != lastReading) {
    lastReading = reading;
    changedAt = now;
  }

  if (now - changedAt >= 30 && reading != stableState) {
    stableState = reading;
    if (stableState == LOW) {
      return true;
    }
  }

  return false;
}

void drawHeader() {
  tft.fillRect(0, 0, 128, 24, C_PURPLE);

  tft.setTextColor(C_WHITE);
  tft.setTextSize(2);
  tft.setCursor(8, 5);
  tft.print("STARBIE");

  tft.drawFastHLine(0, 24, 128, C_CYAN);
}

void drawHeart(int x, int y, uint16_t color) {
  tft.fillCircle(x + 3, y + 3, 3, color);
  tft.fillCircle(x + 9, y + 3, 3, color);
  tft.fillTriangle(x, y + 4, x + 12, y + 4,
                   x + 6, y + 12, color);
}

void drawPetFace(int x, int y, bool blink) {
  // Ears
  tft.fillTriangle(x + 12, y + 13,
                   x + 18, y - 1,
                   x + 27, y + 12, C_PINK);

  tft.fillTriangle(x + 42, y + 12,
                   x + 52, y - 1,
                   x + 58, y + 13, C_PINK);

  // Head
  tft.fillRoundRect(x + 10, y + 8, 50, 43, 13, C_PINK);

  // Face
  tft.fillRoundRect(x + 15, y + 15, 40, 31, 10, C_YELLOW);

  // Eyes
  if (blink) {
    tft.drawFastHLine(x + 24, y + 29, 7, C_BLACK);
    tft.drawFastHLine(x + 40, y + 29, 7, C_BLACK);
  } else {
    tft.fillCircle(x + 28, y + 29, 3, C_BLACK);
    tft.fillCircle(x + 44, y + 29, 3, C_BLACK);
    tft.drawPixel(x + 29, y + 28, C_WHITE);
    tft.drawPixel(x + 45, y + 28, C_WHITE);
  }

  // Blush
  tft.fillCircle(x + 20, y + 36, 3, C_ORANGE);
  tft.fillCircle(x + 52, y + 36, 3, C_ORANGE);

  // Smile
  tft.drawLine(x + 32, y + 37, x + 35, y + 40, C_BLACK);
  tft.drawLine(x + 35, y + 40, x + 39, y + 37, C_BLACK);

  // Body and feet
  tft.fillRoundRect(x + 19, y + 49, 32, 17, 8, C_CYAN);
  tft.fillRoundRect(x + 20, y + 60, 12, 7, 3, C_PINK);
  tft.fillRoundRect(x + 39, y + 60, 12, 7, 3, C_PINK);
}

void drawBar(int x, int y, const char* label,
             int value, uint16_t color) {
  tft.setTextSize(1);
  tft.setTextColor(C_WHITE);
  tft.setCursor(x, y);
  tft.print(label);

  tft.drawRoundRect(x + 39, y - 2, 59, 10, 3, C_WHITE);

  int width = map(value, 0, 100, 0, 55);

  if (width > 0) {
    tft.fillRoundRect(x + 41, y, width, 6, 2, color);
  }

  tft.setCursor(x + 102, y);
  tft.print(value);
}

void drawPetScreen() {
  tft.fillScreen(C_BG);
  drawHeader();

  // Animated floating pet
  int floatOffset = (animationFrame % 2 == 0) ? 0 : 2;
  bool blink = (animationFrame % 12 == 0);

  drawPetFace(29, 37 + floatOffset, blink);

  if (sleeping) {
    tft.setTextSize(1);
    tft.setTextColor(C_CYAN);
    tft.setCursor(87, 48);
    tft.print("Z");
    tft.setCursor(96, 39);
    tft.print("z");
  } else if (selectedAction == 2) {
    drawHeart(87, 43, C_PINK);
  } else if (selectedAction == 1) {
    tft.setTextColor(C_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(88, 43);
    tft.print("!");
  }

  // Stats section
  tft.drawFastHLine(4, 107, 120, C_CYAN);

  drawBar(5, 114, "JOY", joy, C_PINK);
  drawBar(5, 127, "ENERGY", energy, C_CYAN);
  drawBar(5, 140, "FULL", fullness, C_ORANGE);

  // Current selection indicator
  tft.fillRect(0, 151, 128, 9, C_PURPLE);
  tft.setTextColor(C_WHITE);
  tft.setTextSize(1);
  tft.setCursor(3, 152);
  tft.print("ACTION: ");
  tft.print(actions[selectedAction]);
  tft.setCursor(90, 152);
  tft.print("SW1>");
}

void drawStatsScreen() {
  tft.fillScreen(C_BG);
  drawHeader();

  tft.setTextSize(1);
  tft.setTextColor(C_CYAN);
  tft.setCursor(8, 33);
  tft.print("SENSOR MONITOR");

  tft.setTextColor(C_WHITE);
  tft.setCursor(8, 53);
  tft.print("Temperature:");

  tft.setCursor(91, 53);
  if (isnan(temperatureC)) {
    tft.print("-- C");
  } else {
    tft.print(temperatureC, 1);
    tft.print("C");
  }

  tft.setCursor(8, 72);
  tft.print("Humidity:");

  tft.setCursor(91, 72);
  if (isnan(humidity)) {
    tft.print("-- %");
  } else {
    tft.print(humidity, 0);
    tft.print("%");
  }

  drawBar(5, 94, "JOY", joy, C_PINK);
  drawBar(5, 107, "ENERGY", energy, C_CYAN);
  drawBar(5, 120, "FULL", fullness, C_ORANGE);

  tft.setTextColor(C_YELLOW);
  tft.setCursor(5, 146);
  tft.print("SW2: BACK");
}

//pet actions

void performAction() {
  sleeping = false;
  sleepUntil = 0;

  switch (selectedAction) {
    case 0:  // FEED
      updateStats(3, 2, 18);
      break;

    case 1:  // PLAY
      updateStats(12, -10, -5);
      break;

    case 2:  // PET
      updateStats(10, 0, 0);
      break;

    case 3:  // NAP
      updateStats(1, 18, -2);
      sleeping = true;
      sleepUntil = millis() + 15000UL;
      break;
  }
}

// ---------------- SETUP ----------------

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  // Common 128x160 ST7735 module configuration.
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(C_BG);

  dht.begin();

  drawPetScreen();

  Serial.println("Starbie Color Edition started.");
}

// ---------------- MAIN LOOP ----------------

void loop() {
  unsigned long now = millis();

  // Button 1: cycle through actions on the pet screen.
  if (buttonPressed(BUTTON_NEXT, lastNextReading,
                    stableNext, nextChangedAt)) {
    if (!statsView) {
      selectedAction = (selectedAction + 1) % ACTION_COUNT;
      drawPetScreen();
    }
  }

  // Button 2: execute selected action, or switch screens.
  if (buttonPressed(BUTTON_SELECT, lastSelectReading,
                    stableSelect, selectChangedAt)) {
    if (statsView) {
      statsView = false;
      drawPetScreen();
    } else {
      performAction();
      drawPetScreen();
    }
  }

  // Wake after a nap.
  if (sleeping && now >= sleepUntil) {
    sleeping = false;
    drawPetScreen();
  }

  // Read the DHT11 periodically; avoid reading too quickly.
  if (now - lastDHTRead >= 2500UL) {
    lastDHTRead = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h)) humidity = h;
    if (!isnan(t)) temperatureC = t;

    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.print(" C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // Animate the pet without blocking button handling.
  if (!statsView && now - lastAnimation >= 400UL) {
    lastAnimation = now;
    animationFrame++;

    // Redraw the pet periodically.
    drawPetScreen();
  }

  delay(5);
}