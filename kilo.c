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
// - echo (output user input)
// - canonical mode (read byte by byte instead of line by line)
// - signal interrupts (ctrl-c, ctrl-z)
// - software flow ctrl (IXON, ctrl-s stops data transmitted until ctrl-q pressed)
void enableRawMode()
{
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(IXON);
  raw.c_lflag &= ~(ECHO | ICANON | ISIG);

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