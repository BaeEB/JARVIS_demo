#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>

#include "tty.h"

#include "../config.h"

void tty_reset(tty_t *tty) {
	tcsetattr(tty->fdin, TCSANOW, &tty->original_termios);
}

void tty_close(tty_t *tty) {
    (void)tty_reset(tty); // Cast to void to indicate we intentionally ignore the return value
    /* fclose(tty->fout);   // Removed to comply with MISRA_C_2012_21_06 */
    (void)close(tty->fdin); // Cast to void to indicate we intentionally ignore the return value
}

static void handle_sigwinch(int sig){
	(void)sig;
}

/* Assuming necessary header files are included and tty_t is defined appropriately */

static void tty_exit_failure(tty_t *tty) {
	if (tty->fdin >= 0) {
		close(tty->fdin);
	}
	if (tty->fout) {
		fclose(tty->fout);
	}
	exit(EXIT_FAILURE);
}

void tty_init(tty_t *tty, const char *tty_filename) {
	tty->fdin = open(tty_filename, O_RDONLY);
	if (tty->fdin < 0) {
		perror("Failed to open tty");
		tty_exit_failure(tty);
	}

	tty->fout = fopen(tty_filename, "w");
	if (!tty->fout) {
		perror("Failed to open tty");
		tty_exit_failure(tty);
	}

	if (setvbuf(tty->fout, NULL, _IOFBF, 4096)) {
		perror("setvbuf");
		tty_exit_failure(tty);
	}

	if (tcgetattr(tty->fdin, &tty->original_termios)) {
		perror("tcgetattr");
		tty_exit_failure(tty);
	}

	struct termios new_termios = tty->original_termios;

	/* Disable all of
	 * ICANON  Canonical input (erase and kill processing).
	 * ECHO    Echo.
	 * ISIG    Signals from control characters
	 * ICRNL   Conversion of CR characters into NL
	 */
	new_termios.c_iflag &= ~(ICRNL);
	new_termios.c_lflag &= ~(ICANON | ECHO | ISIG);

	if (tcsetattr(tty->fdin, TCSANOW, &new_termios)) {
		perror("tcsetattr");
		tty_exit_failure(tty);
	}

	tty_getwinsz(tty);

	tty_setnormal(tty);

	/* Using signal function directly violates MISRA rules, proper error handling is required */
	if (signal(SIGWINCH, handle_sigwinch) == SIG_ERR) {
		tty_exit_failure(tty);
	}
}

void tty_getwinsz(tty_t *tty) {
	struct winsize ws;
	if (ioctl(fileno(tty->fout), TIOCGWINSZ, &ws) == -1) {
		tty->maxwidth = 80;
		tty->maxheight = 25;
	} else {
		tty->maxwidth = ws.ws_col;
		tty->maxheight = ws.ws_row;
	}
}

#include <errno.h> // Imported for errno

char tty_getchar(tty_t *tty) {
  char ch;
  int size = read(tty->fdin, &ch, 1);
  if (size < 0) {
    // perror("error reading from tty");
    // Handling the error without aborting the program
    // Could set a global error variable or take other appropriate action
    ch = -1; // Using -1 to indicate the read error
  } else if (size == 0) {
    // EOF
    // Instead of exiting, we can return a special value to indicate EOF.
    // This could be 0, if not valid data, or another special character.
    ch = 0;  // Assuming 0 is not a valid character in the stream
  }
  // The function will now return in all cases, without calling exit()
  return ch;
}

int tty_input_ready(tty_t *tty, long int timeout, int return_on_signal) {
    fd_set readfs;
    FD_ZERO(&readfs);
    FD_SET(tty->fdin, &readfs);

    struct timespec ts = {timeout / 1000, (timeout % 1000) * 1000000};

    sigset_t mask;
    sigemptyset(&mask);
    if (!return_on_signal) {
        sigaddset(&mask, SIGWINCH);
    }

    int err = pselect(
            tty->fdin + 1,
            &readfs,
            NULL,
            NULL,
&ts,
&mask);

    if (err < 0) {
        if (errno == EINTR) {
            return 0;
        } else {
            // perror function can be used for error logging if allowed; otherwise, an alternative approach should be used.
            perror("select");

            // Instead of exiting, return a predefined error code indicating an error occurred.
            // The choice of `-1` is conventional for a function returning an integer to indicate an error.
            return -1; 
        }
    } else {
        return FD_ISSET(tty->fdin, &readfs);
    }
}

static void tty_sgr(tty_t *tty, int code) {
	tty_printf(tty, "%c%c%im", 0x1b, '[', code);
}

void tty_setfg(tty_t *tty, int fg) {
	if (tty->fgcolor != fg) {
		tty_sgr(tty, 30 + fg);
		tty->fgcolor = fg;
	}
}

void tty_setinvert(tty_t *tty) {
	tty_sgr(tty, 7);
}

void tty_setunderline(tty_t *tty) {
	tty_sgr(tty, 4);
}

void tty_setnormal(tty_t *tty) {
	tty_sgr(tty, 0);
	tty->fgcolor = 9;
}

void tty_setnowrap(tty_t *tty) {
	tty_printf(tty, "%c%c?7l", 0x1b, '[');
}

void tty_setwrap(tty_t *tty) {
	tty_printf(tty, "%c%c?7h", 0x1b, '[');
}

void tty_newline(tty_t *tty) {
	tty_printf(tty, "%c%cK\n", 0x1b, '[');
}

void tty_clearline(tty_t *tty) {
	tty_printf(tty, "%c%cK", 0x1b, '[');
}

void tty_setcol(tty_t *tty, int col) {
	tty_printf(tty, "%c%c%iG", 0x1b, '[', col + 1);
}

void tty_moveup(tty_t *tty, int i) {
	tty_printf(tty, "%c%c%iA", 0x1b, '[', i);
}

void tty_printf(tty_t *tty, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(tty->fout, fmt, args);
	va_end(args);
}

void tty_putc(tty_t *tty, char c) {
    (void)fputc(c, tty->fout);
}

void tty_flush(tty_t *tty) {
    (void)fflush(tty->fout); // Cast to void to indicate return value is intentionally unused
}

size_t tty_getwidth(tty_t *tty) {
	return tty->maxwidth;
}

size_t tty_getheight(tty_t *tty) {
	return tty->maxheight;
}
