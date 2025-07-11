#include "utils.h"

char get_char() {
	char buf;
	struct termios old_tty;
	struct termios new_tty;

	if (tcgetattr(STDIN_FILENO, &old_tty) != 0) {
		std::cerr << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
		return 0;
	}

	new_tty = old_tty;
	new_tty.c_lflag &= ~ICANON;	// Turns off Canonical Mode
	new_tty.c_lflag &= ~ECHO;	// Disable echo, echoing of input characters.
	new_tty.c_cc[VMIN] = 1;
	new_tty.c_cc[VTIME] = 0;

	if (tcsetattr(STDIN_FILENO, TCSANOW, &new_tty) != 0) {
		std::cerr << "Error " << errno << " from tcsetattr: " << strerror(errno) << std::endl;
		return 0;
	}

	if(read(STDIN_FILENO, &buf, 1) < 0){
		std::cerr << "read() error, back to school it is.." << std::endl;
		return 0;
	}

	return buf;
}

void func() {
	std::cout << RED << "This Should be a seperate thread" << END << std::endl;
	for (int idx = 1; idx <= 5; idx++){
		std::cout << YELLOW << "\tcount = " << idx << END << std::endl;
	}
	std::cout << END << std::endl;

	while (1){
		if (get_char() == 'q')
			exit(0);
	}
} // Is there such a thing as a thread id in the process manager?

int main(){ 
	std::cout << "BEFORE test thread creation the Main THREAD :D" << std::endl;
	std::thread test(func);
	std::cout << "AFTER test thread creation the Main THREAD :D" << std::endl;

	test.join();
	std::cout << "In the Main THREAD :D" << std::endl;
}
