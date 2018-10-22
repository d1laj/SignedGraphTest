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

void test_1(BipGraph& G){
  if ( /*G.as_at_least_4_deg_2() && G.is_planar() &&*/ G.verifyPropertyGadget() ) {
    //stop = true;
    md.lock();
    ms << done << "\n" << G << "\n";
    md.unlock();
  }
}

void test_2(BipGraph& G){
  if (G.is_planar()){
    //ms << id << "" total << '\r' << myflush();
    if (G.tentative_poly_test() != G.sign_to_UC4()){
      ms << "found" << '\n' <<  G << '\n' << myflush();
      md.lock();
      ms << done << "\n" << G << "\n";
      md.unlock();
    }
  }  //stop = true;
}

void th_test(unsigned long long total, int id, void (*func)(BipGraph&)){
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
    ms << "Thread " << id << " do frag " << ind << "\n";
    talk.unlock();

    unsigned long long begin = total * ind / NB_FRAG;
    unsigned long long end = total * (ind +1) /  NB_FRAG;
    while (count < begin && !stop){
      ++G;
      ++count;
    }
    while (count <= end && !stop){
      func(G);
      ++G;
      ++count;
      ++mydone;

      if (id != 0 && count % (1<<12) == 0){
        md.lock();
        done += mydone;
        mydone = 0;
        md.unlock();
      }
      if (id == 0 && count % (1<<12) == 0){
        md.lock();
        done += mydone;
        mydone = 0;
        talk.lock();
        ms << "                              Done: " << done << " Percent: " << (double)done/(double)total<< "\r" << myflush();
        talk.unlock();
        md.unlock();
      }
    }
  }
}
#define PROFILE 0
int nb_threads = 8;
void test(unsigned long long total, int start, int end, void (*func)(BipGraph&)){
  for (int i= start; i< end;i++){
    fragments.push(i);
  }
#if PROFILE
  th_test(total,0);
#else
  std::vector<std::thread> thread_list;
  for (int i=0; i<nb_threads;i++){
    thread_list.push_back(std::thread(th_test, total, i, func));
  }
  for (int i=0; i<nb_threads;i++){
    thread_list[i].join();
  }
#endif
}


int start = 0;
int end = NB_FRAG;

int test_graphs(int argc, char** argv, void (*func)(BipGraph&)){
	ms << "Hello world\n";
	if (argc == 2){
		nb_threads = atoi(argv[1]);
	} else if (argc == 4){
		nb_threads = atoi(argv[1]);
		start = atoi(argv[2]);
		end = atoi(argv[3]);
	} else{
		ms << "Usage: <program name> nb_thread \nUsage Usage: <program name> nb_thread start end\n";
		ms << "Note start>=0 and end<= 500000\n";
		return 0;
	}

  BipGraph G(n);
  int count = 0;
  const Color::Modifier red(Color::FG_RED);
  const Color::Modifier green(Color::FG_GREEN);
  const Color::Modifier def(Color::FG_DEFAULT);

  unsigned long long total=0;
  ms << "Calcul total\n";
  while (false && !G.end){
    total++;
    ++G;
		//ms << G << '\n';
    if (total % (2<<14) == 0) ms << total << '\r' << myflush();
//		if (total % (2<<18) == 0) ms << G << '\n' << myflush();
  }
	total = 2622183133; // Hardcoded

  ms << total << "\n";
  test(total, start, end, func);
}
/*
void autre_test(unsigned long long total, int id){
	BipGraph G(n);
  int count = 0;
  const Color::Modifier red(Color::FG_RED);
  const Color::Modifier green(Color::FG_GREEN);
  const Color::Modifier def(Color::FG_DEFAULT);

  unsigned long long mytotal=0;
  //ms << "Calcul total\n";
  while (!G.end){
    mytotal++;
    ++G;
		if (G.is_planar()){
			//ms << id << "" total << '\r' << myflush();
			if (G.tentative_poly_test() != G.sign_to_UC4()){
				ms << "found" << '\n' <<  G << '\n' << myflush();
			}
		}
		//ms << G << '\n';
    //if (total % (2<<14) == 0) ms << total << '\r' << myflush();
		if (total % (2<<18) == 0) ms << G << '\n' << myflush();
  }
}*/

int main(int argc, char **argv) {
 test_graphs(argc, argv, test_1);
 //	autre_test();
}
