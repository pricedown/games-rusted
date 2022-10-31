#include <cstdint>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

using std::string, std::cout, std::cin, std::endl;

struct winsize size;

uint32_t move = 0;
int lives = 6;
string phrase = "welcome to black space";
string guessedLetters;
string revealedPhrase;
int gameState;

void init();
void display();
string getMove();
bool validateMove(string move);
void playMove(string move);
int gameResult();

int main(int argc, char *argv[]) {
  init();

  do { // Game loop
    move++;
    display();
    playMove(getMove());
  } while (gameState == 0);

  if (gameState == -1)
    cout << "Better luck next time!" << endl;
  else if (gameState == 1)
    cout << "Congradulations!" << endl;
  cout << "The phrase was '" << phrase << "'." << endl;

  return 0;
}

void init() {
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
  move = 0;
  revealedPhrase = "";
  for (int i = 0; i < phrase.length(); i++) {
    if (phrase[i] == ' ')
      revealedPhrase += ' ';
    else
      revealedPhrase += '?';
  }
}

void display() {
  for (int i = 0; i < size.ws_col; i++)
    cout << '-';

  cout << endl;
  cout << "Move: " << move << endl;
  cout << "Word: " << revealedPhrase << endl;
  cout << "Lives: " << lives << endl;

  for (int i = 0; i < size.ws_col; i++)
    cout << '-';
}

string getMove() {
  string attempt;

  cout << endl;
  do {
    cout << "Guess new letters: ";
    cin >> attempt;
  } while (validateMove(attempt) == false);

  return attempt;
}

bool validateMove(string attempt) {
  if (attempt.length() < 1) {
    cout << "Enter atleast 1 character" << endl;
    return false;
  }
  for (char aChar : attempt) {
    if (aChar < 96 || aChar > 122) {
      cout << "Only use lowercase a-z characters." << endl;
      return false;
    }
  }
  return true;
}

void playMove(string move) {
  for (char guessChar : move) {
    bool hasCharacter = false;
    for (int i = 0; i < phrase.length(); i++) {
      if (guessChar == phrase[i]) {
        hasCharacter = true;
        revealedPhrase[i] = guessChar; // Reveal the character
      }
    }
    if (!hasCharacter)
      lives--;
    if (gameState == 0)
      gameState = gameResult(); // Update the game state with each character
  }
  guessedLetters += move;
}

int gameResult() {
  if (lives < 1)
    return -1;
  for (int i = 0; i < phrase.length(); i++) {
    if (phrase[i] != revealedPhrase[i])
      return 0;
  }
  return 1;
}
