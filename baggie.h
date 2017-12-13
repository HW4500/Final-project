#include <iostream> 
#include <windows.h> 

#define RUNNING 1
#define WAITING 0
#define FINISHED 2 
 

class baggie{
public:
	baggie(double* optimal_in, double* shift1_in, double* shift2_in, int nameinput,int N_in,int T_in,double alpha_in,double pi1_in,double pi2_in,double p1_in,double p2_in,double rho_in,int Nw_in);
	~baggie(){ printf("worker %d says goodbye\n", name); } 
  void setconsolemutex(HANDLE consolemutexinput);
  void baggiecomp();
  void setoptisectionmutex(HANDLE optisectioninput){optisectionmutex = optisectioninput;}
  void setstepdonesectionmutex(HANDLE *stepdonesectioninput){stepdonesectionmutex = stepdonesectioninput;}
  void setnowstepdonemutexesaddress(int **ppaddress){address_of_nowstepdonesection = ppaddress;}
  double *bag_dbldup(double const * src, int len);
  int bag_max(int index,int len, int **address_of_nowstepdonesection);
  int* Cut();
 private:
  double* optimal;
  double* optimal_copy;
  double* shift1;
  double* shift2;
  int name;
  int N;
  int T;
  int Nw;
  double alpha;
  double pi1;
  double pi2;
  double p1;
  double p2;
  double rho;
  int **address_of_nowstepdonesection;
  HANDLE optisectionmutex;
  HANDLE *stepdonesectionmutex;
  HANDLE consolemutex;
  void letmein(void);
  void seeya(void);
};

using namespace std;

