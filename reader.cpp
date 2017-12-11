#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include<sys\timeb.h>

#include <windows.h>

double mytimecheck(void);

int main(int argc, char* argv[])
{
	int retcode = 0;
	FILE *in = NULL;
	char mybuffer[100];
	int N, T;
	double alpha,pi1,pi2,p1,p2,rho;
    int j, h, t,m;
    double pi, *optimal, newprice, candidate, bestone, *shift1, *shift2,timestart, runtime;

	
	if (argc != 2){
		printf("Usage:  arb1.exe datafilename\n"); retcode = 100; goto BACK;
	}

	in = fopen(argv[1], "r");
	if (in == NULL){
		printf("could not open %s for reading\n", argv[1]);
		retcode = 200; goto BACK;
	}

	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	N = atoi(mybuffer);
	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	T = atoi(mybuffer);
	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	alpha = atof(mybuffer);
	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	pi1 = atof(mybuffer);
	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	pi2 = atof(mybuffer);
	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	p1 = atof(mybuffer);
	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
	p2 = atof(mybuffer);
	fscanf(in, "%s", mybuffer);
	fscanf(in, "%s", mybuffer);
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
	  
printf("pi1 %g, pi2 %g\n",pi1,pi2);
  /** do last stage **/
  for (j = 0; j <= N; j++){
	  newprice = p1*shift1[j]+p2*shift2[j];
	  optimal[(T - 1)*(N + 1) + j] = newprice*j*rho;
	  // V[k,t] stored at optimal[t*(N+1) + k] 
  }
  
  
  for (t= T - 2; t>= 0; t--){
	  for (j = 0; j <= N; j++){

		  bestone = 0;
		  /** enumerate possibilities **/
		  for (h = 0; h <= j; h++){
			  newprice = p1*shift1[h]+p2*shift2[h];
			  candidate= newprice*((1-rho)*optimal[(t + 1)*(N + 1) + j ] + rho*(h+optimal[(t + 1)*(N + 1) + j - h]));
			  if (candidate > bestone) bestone = candidate;
		  }
		  optimal[t*(N + 1) + j] = bestone;
	  }
	  printf("done with stage t = %d\n", t);
  }

  runtime = mytimecheck() - timestart; 
  if (runtime < 0) runtime = 0;  
  
  printf("optimal value for trade sequencing = %g in %g\n", optimal[N],runtime);



BACK:
  printf("\nran with code %d\n", retcode);
  return retcode;
}



double mytimecheck(void)
{
   double seconds, millis;
   struct timeb mytimeb;

   ftime(&mytimeb);

   seconds = (double) mytimeb.time;
   millis = ( (double) mytimeb.millitm)/1000.00;

   return seconds+millis;
}
