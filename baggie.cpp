#include <windows.h> 
#include <process.h>
#include "baggie.h"

double mytimecheck(void);

// implementation file for class baggie

baggie :: baggie(double* optimal_in, double* shift1_in, double* shift2_in,
						int name_in,int N_in,int T_in,double alpha_in,double pi1_in,double pi2_in,double p1_in,double p2_in,double rho_in)
{
	optimal = optimal; shift2 = shift2_in;
	name = name_in;
	N=N_in; T=T_in; alpha=alpha_in; pi1=pi1_in; pi2=pi2_in; p1=p1_in; p2=p2_in; rho=rho_in;
	result = 0;
	status = WAITING;
	iterationsdone = -1;

}

void baggie :: setconsolemutex(HANDLE consolemutexinput)
{
	consolemutex = consolemutexinput;
}

void baggie :: letmein(void)
{
	char icangoin;
	int localiamdonesection;
	
		icangoin = 0;
		while(icangoin == 0){
			Sleep(1000);
			WaitForSingleObject(iamdonesectionmutex, INFINITE);
			 
			if( (*address_of_nowiamdonesection) != 1){
				/** key logic: it checks to see if the number of workers in the heavy section is less than the
				number we want to allow **/
				icangoin = 1;
				*address_of_nowiamdonesection=0; //** increase the count
				localiamdonesection = *address_of_nowiamdonesection;  
				// so localinheavysection will have the count of busy workers
			}

			ReleaseMutex(iamdonesectionmutex);
		}
		WaitForSingleObject(consolemutex, INFINITE);
		cout << "******worker" << name <<": I'm in\n";
		// we can use localinheavysection without protecting it with a mutex, because it is a local variable to this function, i.e.
		// it is not shared with other mutexes
		ReleaseMutex(consolemutex);
}

void baggie :: seeya(void){++*address_of_nowiamdonesection;}

void baggie :: baggiecomp(void)
{

    int j, h, t,halfN;
    double  newprice, candidate, bestone;

	halfN=int(N/2);

	for (t= T - 2; t>= 0; t--){
		letmein(); // check to see if we can become busy
		for (j = name*(halfN+1); j <= (1-name)*halfN+name*N; j++){

			  bestone = 0;
			  /** enumerate possibilities **/
			  for (h = 0; h <= j; h++){
				  newprice = p1*shift1[h]+p2*shift2[h];
				  candidate= newprice*((1-rho)*optimal[(t + 1)*(N + 1) + j ] + rho*(h+optimal[(t + 1)*(N + 1) + j - h]));
				  if (candidate > bestone) bestone = candidate;
			  }
			  optimal[t*(N + 1) + j] = bestone;
			  printf("name: %d iamhere: %d\n",name,j);
		  }
		printf("name: %d iamhere\n",name);
		  WaitForSingleObject(consolemutex, INFINITE);
			cout << "******worker" << name <<": I am done with stage t = " << t <<"\n";
			ReleaseMutex(consolemutex);
			seeya();

			WaitForSingleObject(consolemutex, INFINITE);
			printf(" >> worker %d:  I am out\n", name);
			ReleaseMutex(consolemutex);
	}
	result =  optimal[N];
}
