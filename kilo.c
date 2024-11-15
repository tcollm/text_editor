#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// create original terminal attr,
// so that they can be reset when we're done with raw mode
struct termios orig_termios;
void disableRawMode()
{
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// turn off while using raw mode:
// - disable auto handling of carriage returns as newlines (ICRNL, make ctrl-m produce 13 instead of 10)
// - software flow control (IXON, ctrl-s stops data transmitted until ctrl-q pressed)

// - disable output processing (OPOST, so that multiple carriage returns and newlines are not output when enter is pressed)

// - echo (output user input)
// - canonical mode (read byte by byte instead of line by line)
// - input str literals (IEXTEN, some systems allow you to type ctr-v to type str literal chars)
// - signal interrupts (ctrl-c, ctrl-z)
void enableRawMode()
{
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(ICRNL | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main()
{
  enableRawMode();

  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
  {
    // print control keypresses (arrows, backspace, etc.)
    if (iscntrl(c))
    {
      printf("%d\n", c);
    }
    else
    {
      printf("%d ('%c')\n", c, c);
    }
  };
  return 0;
}