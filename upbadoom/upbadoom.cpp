#include <csignal>
#include <cstddef>
#include <fcntl.h>
#include <filesystem>
#include <ios>
#include <iostream>
#include <regex>
#include <string>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/wait.h>
#include <termios.h>

int getCols();
int getRows();

// Terminal color codes
// https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
// https://gist.github.com/JBlond/2fea43a3049b38287e5e9cefc87b2124
enum TerminalColor {
  COLOR_BLACK_FG = 30,
  COLOR_WHITE_BG = 47,
  COLOR_GREEN = 32,
  COLOR_MAGENTA = 35,
  COLOR_CYAN = 36,
  COLOR_DOOM_FG = 31, // red
};

// sedutil binary
const std::string SEDUTIL_CLI = "sedutil-cli";
const std::string DRIVE_PATTERN = "nvme[0-9]{0,}n[0-9]$";

// DOOM binary
const std::string DOOM_COMMAND = "fbdoom";
const std::string IWAD = "/usr/share/games/doom/doom1.wad";

// Menu
const int MENU_WIDTH = 28;
const int MENU_START = (getCols() - MENU_WIDTH) / 2;
const int MENU_END = MENU_START + MENU_WIDTH;
const int MIDDLE = (getRows() - 2) / 2;
const int GREETING_ROW = MIDDLE - 2;
const char PASSWORD_CHAR_FOCUSED = '#';
const char PASSWORD_CHAR_UNFOCUSED = '*';
const int PASSWORD_ROW = MIDDLE + 1;
const std::string DOOM_TEXT = "TAKE THE RED PILL!";
const int DOOM_ROW = MIDDLE + 2;
const std::string UNLOCK_MESSAGE_SUCCESS = "You have to see it for yourself.";
const std::string UNLOCK_MESSAGE_FAIL = "Ignorance is bliss";
const int UNLOCK_MESSAGE_ROW = DOOM_ROW + 2;

// get terminal rows
int getRows() {
  struct winsize window {};
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
  return window.ws_row;
}

// get terminal columns
int getCols() {
  struct winsize window {};
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
  return window.ws_col;
}

void configureTermios() {
  // Get current terminal settings
  struct termios term {};
  if (tcgetattr(STDIN_FILENO, &term) != 0) {
    perror("tcgetattr");
    return;
  }

  // Disable echoing
  term.c_lflag &= ~ECHO;

  // Disable buffering IO
  term.c_lflag &= ~ICANON;
  std::cout << std::unitbuf;

  // Apply the new settings
  tcsetattr(STDIN_FILENO, TCSANOW, &term);

  // Set non-blocking mode (file control operations)
  int flags = fcntl(STDIN_FILENO, F_GETFL);

  if (flags == -1) {
    perror("fcntl");
    return;
  }

  flags |= O_NONBLOCK;
  if (fcntl(STDIN_FILENO, F_SETFL, flags) == -1) {
    perror("fcntl");
  }
}

void hideCursor() {
  if (fork() == 0) {
    execlp("tput", "tput", "civis", nullptr);
  }

  wait(nullptr);
}

void showCursor() {
  if (fork() == 0) {
    execlp("tput", "tput", "cnorm", nullptr);
  }
  wait(nullptr);
}

void moveCursor(unsigned int row, unsigned int col) {
  std::cout << "\033[" << row << ";" << col << "H";
}

void printSelectArrows(int row, unsigned long int messageLength) {
  moveCursor(row, MENU_START + MENU_WIDTH / 2 - messageLength / 2 - 3);
  std::cout << "\033[" << COLOR_CYAN << "m"; // Set cyan fg
  std::cout << ">";                          // Before select arrow
  moveCursor(row, MENU_START + MENU_WIDTH / 2 + messageLength / 2 + 2);
  std::cout << "<";       // After select arrow
  std::cout << "\033[0m"; // Reset color
}

void clearSelectArrows(int row, unsigned long int messageLength) {
  moveCursor(row, MENU_START + MENU_WIDTH / 2 - messageLength / 2 - 3);
  std::cout << " ";
  moveCursor(row, MENU_START + MENU_WIDTH / 2 + messageLength / 2 + 2);
  std::cout << " ";
}

void printCentered(int row, const std::string &message) {
  moveCursor(row, (getCols() - message.length()) / 2);
  std::cout << message;
}

int unlockSED(const std::string &password) {
  int status = 0;

  std::regex regexObj(DRIVE_PATTERN);

  std::filesystem::directory_iterator dirIterator("/dev");

  for (const auto &entry : dirIterator) {
    std::string entryFilename = entry.path().filename().string();
    if (std::regex_search(entryFilename, regexObj)) {
      pid_t pid;

      pid = fork();

      if (pid == 0) {
        execlp(SEDUTIL_CLI.c_str(), SEDUTIL_CLI.c_str(), "--setLockingRange",
               "0", "rw", password.c_str(), entry.path().c_str(), nullptr);
        _Exit(EXIT_FAILURE); // exit child process immediately on failure
      }
      waitpid(pid, &status, 0);

      pid = fork();
      if (pid == 0) {
        execlp(SEDUTIL_CLI.c_str(), SEDUTIL_CLI.c_str(), "--setMBRDone", "on",
               password.c_str(), entry.path().c_str(), nullptr);
        _Exit(EXIT_FAILURE); // exit child process immediately on failure
      }

      waitpid(pid, &status, 0);
    }
  }
  return status;
}

void printMenu(int currentOption, unsigned long int passwordLength) {
  moveCursor(PASSWORD_ROW, MENU_START);

  std::cout << "\033[1m\033[" << COLOR_BLACK_FG << ";" << COLOR_WHITE_BG
            << "m"; // white bg

  for (int i = 0; i < MENU_WIDTH; i++) {
    std::cout << " ";
  }

  moveCursor(PASSWORD_ROW, MENU_START + 1);

  if (currentOption == 0) {
    showCursor();

    for (unsigned long int i = 0; i < passwordLength && i < MENU_WIDTH - 2;
         i++) {
      std::cout << PASSWORD_CHAR_FOCUSED;
    }

    std::cout << "\033[0m"; // reset

    clearSelectArrows(PASSWORD_ROW + 2, DOOM_TEXT.length());
    printSelectArrows(PASSWORD_ROW, MENU_WIDTH);
    printCentered(DOOM_ROW, DOOM_TEXT);

    if (passwordLength < MENU_WIDTH - 2) {
      moveCursor(PASSWORD_ROW, MENU_START + passwordLength + 1);
    } else {
      moveCursor(PASSWORD_ROW, MENU_END - 1);
    }

  } else {
    hideCursor();

    std::cout << "\033[1m\033[" << COLOR_BLACK_FG << ";" << COLOR_WHITE_BG
              << "m"; // white bg
    for (unsigned long int i = 0; i < passwordLength && i < MENU_WIDTH - 2;
         i++) {
      std::cout << PASSWORD_CHAR_UNFOCUSED;
    }

    std::cout << "\033[0m"; // reset

    clearSelectArrows(PASSWORD_ROW, MENU_WIDTH);
    moveCursor(PASSWORD_ROW + 2, (getCols() - DOOM_TEXT.length()) / 2);
    std::cout << "\033[1;" << COLOR_DOOM_FG << "m"; // bold red fg
    std::cout << DOOM_TEXT;
    std::cout << "\033[0m"; // reset
    printSelectArrows(DOOM_ROW, DOOM_TEXT.length());
  }
}

void playDoom(const std::string &doomCommand, const std::string &iwad) {
  std::cout << "\033c"; // clear the screen
  pid_t pid = fork();   // make the child process

  if (pid == 0) {
    // child process
    execlp(doomCommand.c_str(), doomCommand.c_str(), "-iwad", iwad.c_str(),
           nullptr);
    _Exit(EXIT_FAILURE); // exit child process immediately on failure
  }

  waitpid(pid, nullptr, 0); // wait for the child process to finish
}

void printGreeting() {
  std::cout << "\033[0;" << COLOR_GREEN << "m";
  printCentered(GREETING_ROW, "Unfortunately, no one can ");
  printCentered(GREETING_ROW + 1, "be told what the Matrix is");
  printMenu(0, 0);
  std::cout << "\033[0m";
}

int main() {
  reboot(RB_ENABLE_CAD);
  configureTermios();
  std::cout << "\033c"; // clear the screen

  // Prevent closing the program using the keyboard
  std::signal(SIGINT, [](int) {});  // ^C
  std::signal(SIGQUIT, [](int) {}); // ^D or ^backslash
  std::signal(SIGTSTP, [](int) {}); // ^Z
  std::signal(SIGTERM, [](int) {}); // you never know

  int currentOption = 0; // 0 -> password input, 1 -> play DOOM

  // capture input

  const int PASS_LENGTH = 256;
  std::string password;
  int c; // input char

  printGreeting();

  // endless menu loop
  while (1) {
    c = getchar();
    if (c == '\033') {
      if (getchar() == '[') {
        c = getchar();

        if (c == 'A') { // type of arrow: up
          currentOption = !currentOption;
          printMenu(currentOption, password.length());
        } else if (c == 'B') { // type of arrow: down
          currentOption = !currentOption;
          printMenu(currentOption, password.length());
        }
      }
    } else if (currentOption == 0 && c >= 32 && c <= 126) {
      if (password.length() < PASS_LENGTH) {
        password += (char)c;
        printMenu(currentOption, password.length());
      }
    } else if (c == '\n') {
      if (currentOption == 0) {
        moveCursor(0, 0);
        if (unlockSED(password) == 0) {
          std::cout << "\033[1;" << COLOR_GREEN << "m";
          printCentered(UNLOCK_MESSAGE_ROW, UNLOCK_MESSAGE_SUCCESS);
          reboot(RB_AUTOBOOT);
        } else {
          std::cout << "\033[1;" << COLOR_DOOM_FG << "m";
          printCentered(UNLOCK_MESSAGE_ROW, UNLOCK_MESSAGE_FAIL);
          std::cout << "\033[0m";

          if (password.length() < MENU_WIDTH - 2) {
            moveCursor(PASSWORD_ROW, MENU_START + password.length() + 1);
          } else {
            moveCursor(PASSWORD_ROW, MENU_END - 1);
          }
        }
      } else {
        moveCursor(0, 0);
        playDoom(DOOM_COMMAND, IWAD); // Delete to exit
        std::cout << "\033c";         // clear the framebuffer
        printGreeting();
        printMenu(0, password.length());
      }
    } else if (c == 127) { // backspace
      if (!password.empty()) {
        password = password.substr(0, password.size() - 1);
        printMenu(currentOption, password.length());
      }
    }
  }
}
