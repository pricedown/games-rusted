#include <cstdint>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

using std::cout, std::cin, std::endl, std::string, std::pair;

struct winsize wsize;

bool inRange(int x, int low, int high) { return ((x - high) * (x - low) <= 0); }

enum tile { x = -1, o = 1, empty = 0 };
enum gameResult { xwins, owins, draw, ongoing };
tile board[3][3];
int movenum = 0;

void init();
void display();
pair<int, int> getMove();
bool validateMove(string);
void playMove(pair<int, int>);
gameResult evaluateGame();
pair<int, int> strToMove(string);

int main(int argc, char *argv[]) {
  gameResult gameState;
  init();

  do {
    movenum++;
    display();
    playMove(getMove());
    gameState = evaluateGame();
  } while (gameState == ongoing); // TODO loop logic

  if (gameState == xwins)
    cout << "X won! Congradulations!" << endl;
  else if (gameState == owins)
    cout << "O won! Congradulations!" << endl;
  else if (gameState == draw)
    cout << "Zamn that's crazy." << endl;

  return 0;
}

void init() {
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
  for (int i = 0; i < 9; i++)
    board[i / 3][i % 3] = empty; // Clear board
}

void display() {
  // Header
  for (int i = 0; i < wsize.ws_col; i++)
    cout << '-';

  cout << endl;
  if (movenum % 2 == 0)
    cout << "O's turn." << endl;
  else
    cout << "X's turn." << endl;
  cout << endl;

  for (int i = 0; i < wsize.ws_col; i++)
    cout << '-';

  // Board
  cout << "+---+\n";
  for (int y_i = 2; y_i >= 0; y_i--) {
    cout << "|";
    for (int x_i = 0; x_i <= 2; x_i++) {
      if (board[x_i][y_i] == x)
        cout << 'x';
      else if (board[x_i][y_i] == o)
        cout << 'o';
      else if (board[x_i][y_i] == empty)
        cout << '#';
    }
    cout << '|' << y_i + 1;
    cout << endl;
  }
  cout << "+---+\n 123" << endl;
}

pair<int, int> getMove() {
  string attempt;
  do {
    cout << "Input your move: ";
    cin >> attempt;
  } while (!validateMove(attempt));
  return strToMove(attempt);
}

bool validateMove(string in) {
  // TODO Color the invalid results
  // Make sure stoi is valid
  if (in.length() != 2) {
    cout << "Input only two numbers" << endl;
    return false;
  }
  for (char c : in)
    if (!inRange(c, 48, 57)) {
      cout << "Input only numbers" << endl;
      return false;
    }

  pair<int, int> move = strToMove(in);
  if (!inRange(move.first, 1, 3) || !inRange(move.second, 1, 3)) {
    cout << "Not in bounds" << endl;
    return false;
  }

  if (board[move.first - 1][move.second - 1] != empty) {
    cout << "That tile is not empty" << endl;
    return false;
  }

  return true;
}

pair<int, int> strToMove(string str) {
  return pair<int, int>(std::stoi(str) / 10, std::stoi(str) % 10);
}

void playMove(pair<int, int> move) {
  tile mover;
  if (movenum % 2 == 0)
    mover = o;
  else
    mover = x;
  board[move.first - 1][move.second - 1] = mover;
}

gameResult evaluateGame() {
  for (int i = 0; i <= 2; i++) { // Only check from diagnol is necessary
    if (board[i][i] == empty)
      continue;
    for (int dir = 1; dir <= 3; dir++) {
      int dx = dir % 2, dy = dir / 2; // This could esentially just be 2 bits

      bool connection = true;
      for (int depth = 1; depth <= 2; depth++) {
        if (board[i][i] != board[i + dx * depth % 3][i + dy * depth % 3]) {
          connection = false;
          break;
        }
      }

      if (connection) {
        if (board[i][i] == x)
          return xwins;
        else
          return owins;
      }
    }
  } // A more universal way could be to count outwards from one nonempty tile
    // along the axis until it counts 3.
  if (movenum == 9)
    return draw;
  return ongoing;
}
