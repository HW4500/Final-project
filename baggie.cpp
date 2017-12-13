#include <windows.h> 
#include <process.h>
#include "baggie.h"
#include<sys\timeb.h>
#include <math.h>

double mytimecheck(void);

// implementation file for class baggie
// A AJOUTER LISTE STEPDONE FAIT READER BAGGIE.H
baggie :: baggie(double* optimal_in, double* shift1_in, double* shift2_in,
						int name_in,int N_in,int T_in,double alpha_in,double pi1_in,double pi2_in,double p1_in,double p2_in,double rho_in, int Nw_in)
{
	optimal = optimal_in; shift2 = shift2_in;shift1 = shift1_in;
	name = name_in;
	Nw=Nw_in;
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

int* baggie :: Cut(){
	int step,i;
	int *result;
	double delta;
	result = (int *)calloc((Nw+ 1), sizeof(int));
	result[0]=-1;
	for (i=1;i<Nw;i++){
		delta=1+4.*i*(N+1)*(N+2)/Nw;
		step=int(.5*(sqrt(delta)-1));
	    result[i]=step;
	}
	result[Nw]=N;
	return result;
}

int baggie :: bag_max(int index,int len, int **address_of_nowstepdonesection){
	int j,maxi=0;
	for (j=0;j<len;j++){
		if(j!=index){
			if((*address_of_nowstepdonesection)[j]>=maxi){maxi=(*address_of_nowstepdonesection)[j];}
		}
	}
	return maxi;
}

void baggie :: letmein(void)
{
	char icangoin;
	int *localstepdonesection;
	int i;

		icangoin = 0;
		while(icangoin == 0){
			for(i=0;i<Nw;i++){
				WaitForSingleObject(stepdonesectionmutex[i], INFINITE);
			}
			 
			if( (*address_of_nowstepdonesection)[name]>=bag_max(name,Nw,address_of_nowstepdonesection)){ 
				/** key logic: it checks to see if the number of workers in the heavy section is less than the
				number we want to allow **/
				icangoin = 1;
				localstepdonesection = *address_of_nowstepdonesection;
			}
			for(i=0;i<Nw;i++){
				ReleaseMutex(stepdonesectionmutex[i]);
			}
			 
		}
		WaitForSingleObject(consolemutex, INFINITE);
		cout << "******worker" << name <<": I'm in. I am at " << localstepdonesection[name] << "\n";
		// we can use localinheavysection without protecting it with a mutex, because it is a local variable to this function, i.e.
		// it is not shared with other mutexes
		ReleaseMutex(consolemutex);
}

void baggie :: seeya(void){
	--(*address_of_nowstepdonesection)[name];
	
}

void baggie :: baggiecomp(void)
{
	 
    int j, h, t;
    double  newprice, candidate, bestone;
	double timestart, runtime;
	int *NCut;

	NCut=Cut(); //mod

	for (t= T - 2; t>= 0; t--){
		letmein(); // check to see if we can become busy

		timestart = mytimecheck();

		WaitForSingleObject(optisectionmutex, INFINITE);
		optimal_copy=bag_dbldup(optimal, (N+1)*T);
		ReleaseMutex(optisectionmutex);

		for (j = NCut[name]+1; j <= NCut[name+1]; j++){ 

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



