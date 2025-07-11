#include "utils.h"

//    Implemented this because RAII is kinda cool..
//    Resource Aqcuitsition Is Initialization, meaning that making the thing is also initializing the thing.
//    This also immediately cleans up upon destruction / when leaving the scope. In my case, thus resets the
//    terminal if anything were to go wrong. Pretty neat!
class RawTerminal {
   private:
      struct termios _oldTTY;

   public:
      RawTerminal(){
         if (tcgetattr(STDIN_FILENO, &_oldTTY) != 0) {
            std::cerr << "Error: " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
            return;     // Throwing an exception, might be better..
         }

         struct termios rawTTY = _oldTTY;
         rawTTY.c_lflag &= ~ICANON; // Turns off Canonical Mode
         rawTTY.c_lflag &= ~ECHO;   // Disable echo, echoing of input characters.
         rawTTY.c_cc[VMIN] = 1;
         rawTTY.c_cc[VTIME] = 0;

         if (tcsetattr(STDIN_FILENO, TCSANOW, &rawTTY)){
            std::cerr << "Error: " << errno << " from tcsetattr: " << strerror(errno) << std::endl;
            return;     // Throwing an exception, might be better..
         }
      }

      ~RawTerminal() {
         if (tcsetattr(STDIN_FILENO, TCSANOW, &_oldTTY) != 0){
            std::cerr << "Error: " << errno << " from tcsetattr: " << strerror(errno) << std::endl;
            return;      // Throwing an exception, might be better..
         }
      }

};

// Do think I need to figure out a way to return a value and use it, as realtime bpm changes might be added, cus it's cool.. :D
// Should this then be a seperate process instead?? Because I also want to print/show the current bpm in terminal
// but it only needs to be printed once and updated if I change something, you know.. cool...

void getKey(std::atomic<bool>& running) {
   RawTerminal rawTTY;

   while (running) {
      char c;
      int bytesRead = read(STDIN_FILENO, &c, 1);
      if (bytesRead < 0) {
         std::cerr << "read() error, back to school we goo.." << std::endl;
         return;
      }
      if (c == 'q') {
         running = false;
         break;
      }
   }
   return;
}

bool inputCheck(int argc, char** argv){
   if (argc != 2){
      std::cout << RED << "Not enough parameters\n";
      std::cout << "\tCorrect usage: " << argv[0] << " [bpm]" << END << std::endl;
      return ( false );
   }
   double bpm = atoi(argv[1]);
   if (bpm < 20 || bpm >= 200)
   {
      std::cout << "Given BPM is outside of the current range.." << std::endl;
      return ( false );
   }
   return ( true );
}

int main( int argc, char* argv[] ){
   // An atomic variable, think of a bool, int or ptr; can use .store(), .load(), .exchange()
   // Below I am using constructor style, because the assign operator doesn't work, not when initializing.
   std::atomic<bool> running(true);
   if (!inputCheck(argc, argv))
      return ( 1 );

   std::thread inputListener(getKey, std::ref(running));

   float bpm = atoi(argv[1]);
   std::chrono::milliseconds interval((int)(60.0 / bpm * 1000));        // Please fix something for this at a later time.

   while (running){
      std::cout << "\a" << std::flush;
      std::this_thread::sleep_for(interval);
   }
   inputListener.join();
   return ( 0 );
}
