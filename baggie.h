#include <iostream> 
#include <windows.h> 

#define RUNNING 1
#define WAITING 0
#define FINISHED 2 
 

class baggie{
public:
	baggie(double* optimal_in, double* shift1_in, double* shift2_in, int nameinput,int N_in,int T_in,double alpha_in,double pi1_in,double pi2_in,double p1_in,double p2_in,double rho_in);
	~baggie(){ printf("worker %d says goodbye\n", name); } 
  void setconsolemutex(HANDLE consolemutexinput);
  void baggiecomp();
  double getmeits(void){return iterationsdone;}
  void setstatustofinished(void){status = FINISHED;}
  int getstatus(void){ return status; }
  void setstepdone1sectionmutex(HANDLE stepdone1sectioninput){stepdone1sectionmutex = stepdone1sectioninput;}
  void setstepdone2sectionmutex(HANDLE stepdone2sectioninput){stepdone2sectionmutex = stepdone2sectioninput;}
  void setnowstepdone1mutexesaddress(int *paddress){address_of_nowstepdone1section = paddress;}
  void setnowstepdone2mutexesaddress(int *paddress){address_of_nowstepdone2section = paddress;}
 private:
  double* optimal;
  double* shift1;
  double* shift2;
  int name;
  int N;
  int T;
  double alpha;
  double pi1;
  double pi2;
  double p1;
  double p2;
  double rho;
  double iterationsdone;
  int status;
  int *address_of_nowstepdone1section;  /** this is the address of the integer keeping track of how many workers are done **/
  int *address_of_nowstepdone2section;  /** this is the address of the integer keeping track of how many workers are busy **/
  HANDLE stepdone1sectionmutex;
  HANDLE stepdone2sectionmutex;
  HANDLE consolemutex;
  void letmein(void);
  void seeya(void);
};

using namespace std;

