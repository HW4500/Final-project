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
  void setiamdonesectionmutex(HANDLE iamdonesectioninput){iamdonesectionmutex = iamdonesectioninput;}
  void setiamworkingsectionmutex(HANDLE iamworkingsectioninput){iamworkingsectionmutex = iamworkingsectioninput;}
  void setnowdonemutexesaddress(int *paddress){address_of_nowiamdonesection = paddress;}
  void setnowworkingmutexesaddress(int *paddress){address_of_nowiamworkingsection = paddress;}
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
  int *address_of_nowiamdonesection;  /** this is the address of the integer keeping track of how many workers are done **/
  int *address_of_nowiamworkingsection;  /** this is the address of the integer keeping track of how many workers are busy **/
  HANDLE iamdonesectionmutex;
  HANDLE iamworkingsectionmutex;
  HANDLE consolemutex;
  double result;
  void letmein(void);
  void seeya(void);
};

using namespace std;

