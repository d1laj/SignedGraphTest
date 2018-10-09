#include "bipgraph_v3.hpp"
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>

namespace Color {
enum Code {
  FG_RED = 31,
  FG_GREEN = 32,
  FG_BLUE = 34,
  FG_DEFAULT = 39,
  BG_RED = 41,
  BG_GREEN = 42,
  BG_BLUE = 44,
  BG_DEFAULT = 49
};
class Modifier {
  const Code code;

public:
  Modifier(const Code pCode) : code(pCode) {}
  friend std::ostream &operator<<(std::ostream &os, const Modifier &mod) {
    return os << "\033[" << mod.code << "m";
  }
};
} // namespace Color

int n= 9;
#define NB_FRAG 500000
bool stop = false;
std::mutex talk;
std::mutex m;
std::queue<int> fragments;
std::mutex md;
unsigned long long done;

void th_test(unsigned long long total, int id){
  BipGraph G(n);
  unsigned long long count =0;
  unsigned long long mydone = 0;
  while (!stop){
    m.lock();
    if (fragments.empty()){
      m.unlock();
      break;
    }
    int ind = fragments.front();
    fragments.pop();
    m.unlock();
    talk.lock();
    std::cout << "Thread " << id << " do frag " << ind << "\n";
    talk.unlock();

    unsigned long long begin = total * ind / NB_FRAG;
    unsigned long long end = total * (ind +1) /  NB_FRAG;
    while (count < begin && !stop){
      ++G;
      ++count;
    }
    while (count <= end && !stop){
      if ( /*G.as_at_least_4_deg_2() &&*/ G.is_planar() && G.verifyPropertyGadget() ) {
        stop = true;
        md.lock();
        std::cout << done << "\n" << G << "\n";
        md.unlock();
      }
      ++G;
      ++count;
      ++mydone;

      if (id != 0 && count % (1<<16) == 0){
        md.lock();
        done += mydone;
        mydone = 0;
        md.unlock();
      }
      if (id == 0 && count % (1<<14) == 0){
        md.lock();
        done += mydone;
        mydone = 0;
        talk.lock();
        std::cout << "                              Done: " << done << " Percent: " << (double)done/(double)total<< "\r" << std::flush;
        talk.unlock();
        md.unlock();
      }
    }
  }


}

int nb_threads = 8;
void test(unsigned long long total, int start, int end){
  for (int i= start; i< end;i++){
    fragments.push(i);
  }

  std::vector<std::thread> thread_list;
  for (int i=0; i<nb_threads;i++){
    thread_list.push_back(std::thread(th_test,total, i));
  }
  for (int i=0; i<nb_threads;i++){
    thread_list[i].join();
  }
}


int start = 0;
int end = NB_FRAG;

int main(int argc, char **argv) {
  std::cout << "Hello world\n";
	if (argc == 2){
		nb_threads = atoi(argv[1]);
	} else if (argc == 4){
		nb_threads = atoi(argv[1]);
		start = atoi(argv[2]);
		end = atoi(argv[3]);
	} else{
		std::cout << "Usage: <program name> nb_thread \nUsage Usage: <program name> nb_thread start end\n";
		std::cout << "Note start>=0 and end<= 500000\n";
		return 0;
	}

  BipGraph G(n);
  int count = 0;
  const Color::Modifier red(Color::FG_RED);
  const Color::Modifier green(Color::FG_GREEN);
  const Color::Modifier def(Color::FG_DEFAULT);

  unsigned long long total=0;
  std::cout << "Calcul total\n";
  while (false && !G.end){
    total++;
    ++G;
		//std::cout << G << '\n';
    if (total % (2<<14) == 0) std::cout << total << '\r' << std::flush;
//		if (total % (2<<18) == 0) std::cout << G << '\n' << std::flush;
  }
	total = 2622183133; // Hardcoded
	
  std::cout << total << "\n";
  /*
  G = BipGraph(n);
  while (!G.end) {
    if (G.verifyPropertyGadget()) {
      std::cout << green <<  G << "\n" << def;
      std::cout << count << "\n";
      //std::cout << G << def;
      //std::cout << val1 << " != " << val2;
      //std::cout << "\n############\n";
      //std::cout << def;
      break;
    }
    //std::cout << def;
    if (count % (1<<10) == 0){
      std::cout << (double)count/(double)total << "\r" <<std::flush;
    }
    // std::cout << G << def;
    count++;
    ++G;
    // std::cout << G << "here\n";
  }
  std::cout << count << '\n'<<  def;
  */
  test(total, start, end);
}
