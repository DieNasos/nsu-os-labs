#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	int fd; // file descriptor
	char ch;    // character
	struct termios terminal;
	struct termios original;

	// getting the file descriptor corresponding to the terminal device
    // opening it == opening the control terminal of the current session
	fd = open("/dev/tty", O_RDWR);

	tcgetattr(fd, &terminal);   // getting the original parameters of terminal
	original = terminal;    // saving the original terminal

	terminal.c_lflag &= ~ICANON;    // field c_lflag is using for control of lines managing
	// ICANON: this mask bit, if set, enables canonical input processing mode
	// ~ == inversion => disabling option

	// c_cc array defines control characters
	// VMIN - the minimum number of characters for non-canonical input
	// => read request will not be satisfied until 1 character is read
	terminal.c_cc[VMIN] = 1;

	tcsetattr(fd, TCSANOW, &terminal);
	// setting new parameters of terminal
	// TCSANOW == changes start working immediately

	while (1) {
	    // writing in colsole
	    write(fd, "Do you want this program to finish?\n", 36);
	    write(fd, "Enter 'y' for 'yes' or 'n' for 'no': ", 37);
	    read(fd, &ch, 1);   // reading character

	    if (ch == 'y') {
	        // 'y' == 'yes' == finish
	        write(fd, "\nOkay, bye-bye!\n", 16);
	        break;
	    }

	    // else
	    write(fd, "\nOK, I'll ask you again...\n", 27);
	}

	tcsetattr(fd, TCSAFLUSH, &original);    // setting the original parameters of terminal
	/*
	 * TCSAFLUSH == changes begin to work after all the data intended for writing
	 * is transferred to the object pointed to by fd; all data that has been accepted
	 * (but not read) will be canceled before the change
	 */

	return 0;
}