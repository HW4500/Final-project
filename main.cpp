#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include<sys\timeb.h>
#include "safef.h"
#include "baggie.h"
#include <windows.h>



double mytimecheck(void);
unsigned _stdcall comp_wrapper(void *foo);
double * dbldup(double const * src, int len);

int main(int argc, char* argv[])
{
	int retcode = 0;
	FILE *in = NULL;
	char mybuffer[60];
	int N, T,Nw;
	double alpha,pi1,pi2,p1,p2,rho;
	long lt=60;
    int j;
    double *optimal, newprice, *shift1, *shift2,timestart, runtime;


		if (argc != 3){
		printf("Usage:  arb1.exe datafilename number_of_threads\n"); retcode = 100; goto BACK;
	}
	Nw=atoi(argv[2]);
	in = fopen(argv[1], "r");
	if (in == NULL){
		printf("could not open %s for reading\n", argv[1]);
		retcode = 200; goto BACK;
	}

	saferfscanf(mybuffer,in, lt);
	saferfscanf(mybuffer,in, lt);
	saferfscanf(mybuffer,in, lt);
	N = atoi(mybuffer);
	saferfscanf(mybuffer,in, lt);
	saferfscanf(mybuffer,in, lt);
	T = atoi(mybuffer);
	saferfscanf(mybuffer,in, lt);
	saferfscanf(mybuffer,in, lt);
	alpha = atof(mybuffer);
	saferfscanf(mybuffer,in, lt);
	saferfscanf(mybuffer,in, lt);
	pi1 = atof(mybuffer);
	saferfscanf(mybuffer,in, lt);
	saferfscanf(mybuffer,in, lt);
	pi2 = atof(mybuffer);
	saferfscanf(mybuffer,in, lt);
	saferfscanf(mybuffer,in, lt);
	p1 = atof(mybuffer);
	saferfscanf(mybuffer,in, lt);
	saferfscanf(mybuffer,in, lt);
	p2 = atof(mybuffer);
	saferfscanf(mybuffer,in, lt);
	saferfscanf(mybuffer,in, lt);
	rho = atof(mybuffer);

	printf("Numer of assets: %d, Number of periods: %d\nalpha = %g, pi1 = %g, pi2 = %g, p1 = %g, p2 = %g, rho = %g\n\n",
		N, T, alpha,pi1,pi2,p1,p2,rho);

	fclose(in);


  timestart = mytimecheck();

  optimal = (double *)calloc((N + 1)*T, sizeof(double));
  if (!optimal){
	  printf("cannot allocate large matrix\n"); retcode = 2; goto BACK;
  }

 shift1 = (double *)calloc(N + 1, sizeof(double));
 if (!shift1){
	  printf("cannot allocate large list\n"); retcode = 2; goto BACK;
  }
 shift2 = (double *)calloc(N + 1, sizeof(double));
 if (!shift2){
	  printf("cannot allocate large list\n"); retcode = 2; goto BACK;
  }


 for (j = 0; j <= N; j++){
	  shift1[j] = 1 - alpha*pow((double)j, pi1);
      shift2[j] = 1 - alpha*pow((double)j, pi2);
 }
	  
  /** do last stage **/
  for (j = 0; j <= N; j++){
	  newprice = p1*shift1[j]+p2*shift2[j];
	  optimal[(T - 1)*(N + 1) + j] = newprice*j*rho;
	  // V[k,t] stored at optimal[t*(N+1) + k] 
  }











	HANDLE *pThread;
	unsigned *pthreadID;
	HANDLE consolemutex;
	HANDLE *stepdonemutexes;
	HANDLE optimutexe;


	baggie **ppbaggies;
	ppbaggies = (baggie **)calloc(Nw, sizeof(baggie *));
	/** ppbaggies is an array, each of whose members is the address of a baggie, and so the type of ppbaggies is baggie ** **/
	if(ppbaggies == NULL){
		cout << "cannot allocate" << Nw <<"baggies\n";
		retcode = 1; goto BACK;
	}
	pThread = (HANDLE *)calloc(Nw, sizeof(HANDLE));
	pthreadID= (unsigned *)calloc(Nw, sizeof(unsigned));
	stepdonemutexes = (HANDLE *)calloc(Nw, sizeof(HANDLE));
	if((pThread == NULL) || (pthreadID == NULL) || (stepdonemutexes == NULL)) {
		cout << "cannot allocate" << Nw << "handles and threadids\n";
		retcode = 1; goto BACK;
	}

	optimutexe = CreateMutex(NULL, 0, NULL);
	int *nowstepdonemutexes;
	nowstepdonemutexes = (int *)calloc(N + 1, sizeof(int));


	for(int j = 0; j < Nw; j++){
		ppbaggies[j] = new baggie(optimal,dbldup(shift1, N+1),dbldup(shift2,N+1),j,N, T,alpha,pi1,pi2,p1,p2,rho,Nw);  // fake "jobs": normally we would get a list of jobs from e.g. a file
		stepdonemutexes[j] = CreateMutex(NULL, 0, NULL);
		nowstepdonemutexes[j]=T-1;
	}


	consolemutex = CreateMutex(NULL, 0, NULL);

	for(int j = 0; j < Nw; j++){
		ppbaggies[j]->setconsolemutex(consolemutex); // consolemutex shared across workers plus master
		ppbaggies[j]->setoptisectionmutex(optimutexe); 
		ppbaggies[j]->setstepdonesectionmutex(stepdonemutexes); 
		ppbaggies[j]->setnowstepdonemutexesaddress( &nowstepdonemutexes );
	}

	for(int j = 0; j < Nw; j++){
		pThread[j] = (HANDLE)_beginthreadex( NULL, 0, &comp_wrapper, (void *) ppbaggies[j], 
			0, 		&pthreadID[j] );
	}
	

	for(int j = 0; j < Nw; j++){
		WaitForSingleObject(pThread[j], INFINITE);
		printf("--> thread %d done\n", j); 
		delete ppbaggies[j]; // calls destructor
	}
	free(ppbaggies);














	

  
  


  runtime = mytimecheck() - timestart; 
  if (runtime < 0) runtime = 0;  
  
  printf("optimal value for trade sequencing = %g in %g s\n", optimal[N],runtime);



BACK:
  printf("\nran with code %d\n", retcode);
  return retcode;
}



unsigned _stdcall comp_wrapper(void *genericaddress)
{
	baggie *pbaggie = (baggie *) genericaddress;

//	comp(pbag->v1, pbag->v2, pbag->v3, &(pbag->result));
	//comp(pbag);

	pbaggie->baggiecomp();

	return 0;
}

double * dbldup(double const * src, int len){
	double * p = (double *)calloc(len, sizeof(double));
	memcpy(p,src,len*sizeof(double));
	return p;
}