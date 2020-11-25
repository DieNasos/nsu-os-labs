#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int count;
int fd; // file descriptor
struct termios original;

void interruption(int sig) {
        if (sig == SIGQUIT) {
            printf("\tTOTAL %d INTERRUPTIONS\n", count);
            tcsetattr(fd, TCSAFLUSH, &original);    // setting the original parameters of terminal
            /*
             * TCSAFLUSH == changes begin to work after all the data intended for writing
             * is transferred to the object pointed to by fd; all data that has been accepted
             * (but not read) will be canceled before the change
             */
            exit(0);
        }

        ++count;    // +1 interruption
        printf("\a\t%d\n", count);   // \a == sound
	signal(SIGINT, interruption);
}

int main(){
        // binding 'interruption' function with interruption and quit signals
        signal(SIGINT, interruption);
        signal(SIGQUIT, interruption);
        // SIGQUIT is more 'rough' than SUGINT
                
        struct termios terminal;

        // getting the file descriptor corresponding to the terminal device
        // opening it == opening the control terminal of the current session
        fd = open("/dev/tty", O_RDWR);
        
        tcgetattr(fd, &terminal);   // getting the original parameters of terminal

        original = terminal;    // saving the original terminal

        terminal.c_lflag &= ~(ECHO|ICANON); // field c_lflag is using for control of lines managing
        // ECHO: if this mask bit is set, echoing of input characters back to the terminal is enabled
        // ICANON: this mask bit, if set, enables canonical input processing mode
        // ~( | ) == disjunction inversion
        // => disabling this two options

        // c_cc array defines control characters
        terminal.c_cc[VINTR] = 32;  // setting SPACE as the interruption symbol
        terminal.c_cc[VQUIT] = 27;  // setting ESC as the quit symbol

        tcsetattr(fd, TCSANOW, &terminal);
        // setting new parameters of terminal
        // TCSANOW == changes start working immediately

        // doing nothing in infinite cycle
        while (1) {
            pause;
        }

        return 0;
}
