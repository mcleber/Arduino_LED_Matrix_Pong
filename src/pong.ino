#include <LedControl.h>

/* ==================================================
   HARDWARE CONFIGURATION
   ================================================== */

// Pin connections to MAX72XX
constexpr byte PIN_DIN  = 12;
constexpr byte PIN_CS   = 11;
constexpr byte PIN_CLK  = 10;
constexpr byte NUM_DEV  = 1;

// Create LED matrix control object
LedControl matrix(PIN_DIN, PIN_CLK, PIN_CS, NUM_DEV);


/* ==================================================
   GAME CONFIGURATION
   ================================================== */

constexpr byte MATRIX_SIZE   = 8;      // 8x8 LED matrix
constexpr byte PADDLE_PIN    = A1;     // Potentiometer pin
constexpr byte PADDLE_COLUMN = 7;      // Paddle position (right side)
constexpr byte BALL_START_X  = 1;      // Initial ball column

int ballX, ballY;          // Ball position
int dirX = 1;              // Ball horizontal direction
int dirY = 1;              // Ball vertical direction

int paddlePosition = 0;    // Paddle top LED position

unsigned int ballSpeed = 300;     // Ball delay (ms)
unsigned long frameCount = 0;     // Frame counter


/* ==================================================
   INITIALIZATION
   ================================================== */

void setup() {

  // Wake up MAX72XX from power saving mode
  matrix.shutdown(0, false);

  // Set LED brightness (0–15)
  matrix.setIntensity(0, 4);

  // Clear display
  matrix.clearDisplay(0);

  // Seed random generator
  randomSeed(analogRead(A0));

  // Initialize game
  resetGame();
}


/* ==================================================
   MAIN LOOP
   ================================================== */

void loop() {

  readPaddle();        // Read potentiometer
  moveBall();          // Update ball position
  detectCollisions();  // Check walls and paddle
  drawScene();         // Render frame
  updateSpeed();       // Increase difficulty

  delay(ballSpeed);    // Control game speed
  frameCount++;
}


/* ==================================================
   GAME FUNCTIONS
   ================================================== */

// Read paddle position from potentiometer
void readPaddle() {
  int rawValue = analogRead(PADDLE_PIN);
  paddlePosition = map(rawValue, 0, 1023, 0, MATRIX_SIZE - 3);
}

// Update ball position
void moveBall() {
  ballX += dirX;
  ballY += dirY;
}

// Detect collisions with walls and paddle
void detectCollisions() {

  // Left wall
  if (ballX <= 0)
    dirX = 1;

  // Top wall
  if (ballY <= 0)
    dirY = 1;

  // Bottom wall
  if (ballY >= MATRIX_SIZE - 1)
    dirY = -1;

  // Paddle collision
  if (ballX == PADDLE_COLUMN - 1 && dirX > 0) {
    if (ballY >= paddlePosition && ballY <= paddlePosition + 2) {
      dirX = -1;
    }
  }

  // If ball reaches right side → Game Over
  if (ballX >= MATRIX_SIZE - 1) {
    gameOverAnimation();
    resetGame();
  }
}

// Draw ball and paddle
void drawScene() {

  matrix.clearDisplay(0);

  // Draw ball
  matrix.setLed(0, ballX, ballY, true);

  // Draw paddle (3 LEDs)
  for (int i = 0; i < 3; i++) {
    matrix.setLed(0, PADDLE_COLUMN, paddlePosition + i, true);
  }
}

// Gradually increase game speed
void updateSpeed() {
  if (frameCount % 20 == 0 && ballSpeed > 60) {
    ballSpeed -= 10;
  }
}

// Reset game state
void resetGame() {
  ballX = BALL_START_X;
  ballY = random(0, MATRIX_SIZE);

  dirX = 1;
  dirY = random(0, 2) ? 1 : -1;

  ballSpeed = 300;
  frameCount = 0;
}


/* ==================================================
   GAME OVER ANIMATION
   ================================================== */

void gameOverAnimation() {

  for (int repeat = 0; repeat < 4; repeat++) {

    matrix.clearDisplay(0);

    // Draw X pattern
    for (int i = 0; i < MATRIX_SIZE; i++) {
      matrix.setLed(0, i, i, true);
      matrix.setLed(0, i, MATRIX_SIZE - 1 - i, true);
    }

    delay(250);
    matrix.clearDisplay(0);
    delay(250);
  }
}
