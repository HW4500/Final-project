#include <windows.h> 
#include <process.h>
#include "baggie.h"
#include<sys\timeb.h>
#include <math.h>

double mytimecheck(void);

// implementation file for class baggie

baggie :: baggie(double* optimal_in, double* shift1_in, double* shift2_in,
						int name_in,int N_in,int T_in,double alpha_in,double pi1_in,double pi2_in,double p1_in,double p2_in,double rho_in)
{
	optimal = optimal_in; shift2 = shift2_in;shift1 = shift1_in;
	name = name_in;
	N=N_in; T=T_in; alpha=alpha_in; pi1=pi1_in; pi2=pi2_in; p1=p1_in; p2=p2_in; rho=rho_in;
	status = WAITING;
	iterationsdone = -1;

}

void baggie :: setconsolemutex(HANDLE consolemutexinput)
{
	consolemutex = consolemutexinput;
}

double* baggie :: bag_dbldup(double const * src, int len){
	double * p = (double *)calloc(len, sizeof(double));
	memcpy(p,src,len*sizeof(double));
	return p;
}


void baggie :: letmein(void)
{
	char icangoin;
	int localstep1donesection;
	int localstep2donesection;

		icangoin = 0;
		while(icangoin == 0){
			WaitForSingleObject(stepdone1sectionmutex, INFINITE);
			WaitForSingleObject(stepdone2sectionmutex, INFINITE);
			 
			if( *address_of_nowstepdone1section*(1-name)+*address_of_nowstepdone2section*name >= *address_of_nowstepdone2section*(1-name)+*address_of_nowstepdone1section*name){
				/** key logic: it checks to see if the number of workers in the heavy section is less than the
				number we want to allow **/
				icangoin = 1;
				localstep1donesection = *address_of_nowstepdone1section;
				localstep2donesection = *address_of_nowstepdone2section;
			}

			ReleaseMutex(stepdone1sectionmutex);
			ReleaseMutex(stepdone2sectionmutex);
		}
		WaitForSingleObject(consolemutex, INFINITE);
		cout << "******worker" << name <<": I'm in. 0 is at " << localstep1donesection << " and 1 is at " << localstep2donesection << " \n";
		// we can use localinheavysection without protecting it with a mutex, because it is a local variable to this function, i.e.
		// it is not shared with other mutexes
		ReleaseMutex(consolemutex);
}

void baggie :: seeya(void){
	if(name){--*address_of_nowstepdone2section;}
	else{--*address_of_nowstepdone1section;}
	
}

void baggie :: baggiecomp(void)
{

    int j, h, t,NCut;
    double  newprice, candidate, bestone;
	double timestart, runtime;

	NCut=int(.5*(sqrt(2.0*N*(N+1)+1)-1));

	for (t= T - 2; t>= 0; t--){
		letmein(); // check to see if we can become busy

		timestart = mytimecheck();

		WaitForSingleObject(optisectionmutex, INFINITE);
		optimal_copy=bag_dbldup(optimal, (N+1)*T);
		ReleaseMutex(optisectionmutex);

		for (j = name*(NCut+1); j <= (1-name)*NCut+name*N; j++){

			  bestone = 0;
			  /** enumerate possibilities **/
			  for (h = 0; h <= j; h++){
				  newprice = p1*shift1[h]+p2*shift2[h];
				  candidate= newprice*((1-rho)*optimal_copy[(t + 1)*(N + 1) + j ] + rho*(h+optimal_copy[(t + 1)*(N + 1) + j - h]));
				  if (candidate > bestone) bestone = candidate;
			  }
			  WaitForSingleObject(optisectionmutex, INFINITE);
			  optimal[t*(N + 1) + j] = bestone;
			  ReleaseMutex(optisectionmutex);
		  }
		  WaitForSingleObject(consolemutex, INFINITE);

		    runtime = mytimecheck() - timestart; 
            if (runtime < 0) runtime = 0;  

			cout << "******worker" << name <<": I am done with stage t = " << t <<" in "<< runtime << " s\n";
			ReleaseMutex(consolemutex);
			seeya();

	}
}

