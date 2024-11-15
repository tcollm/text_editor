/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

// handle errors
void die(const char *s)
{
  perror(s); // print err desc
  exit(1);
}

// create original terminal attr,
// so that they can be reset when we're done with raw mode
void disableRawMode()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
  {
    die("tcsetattr");
  }
}

// turn off while using raw mode:
// - disable auto handling of carriage returns as newlines (ICRNL, make ctrl-m produce 13 instead of 10)
// - software flow control (IXON, ctrl-s stops data transmitted until ctrl-q pressed)

// - disable output processing (OPOST, so that multiple carriage returns and newlines are not output when enter is pressed)

// - echo (output user input)
// - canonical mode (read byte by byte instead of line by line)
// - input str literals (IEXTEN, some systems allow you to type ctr-v to type str literal chars)
// - signal interrupts (ctrl-c, ctrl-z)

// - Misc flags: BRKINT, INPCK, ISTRIP, CS8 typically do not apply to modern terminal emulators but we turn them off in case
void enableRawMode()
{
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
  {
    die("tcgetattr");
  }
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  // read() timeout:
  raw.c_cc[VMIN] = 0;  // min chars input before read() can return
  raw.c_cc[VTIME] = 1; // read() waits 100ms for input

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
  {
    die("tcsetattr");
  }
}

/*** init ***/

int main()
{
  enableRawMode();

  while (1)
  {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) // ignore EAGAIN, an errno from read() timing out
    {
      die("read");
    }
    if (iscntrl(c))
    {
      printf("%d\r\n", c);
    }
    else
    {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q')
      break;
  };
  return 0;
}