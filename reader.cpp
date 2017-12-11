#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <sys/types.h>
#include <sys/stat.h>

#include <windows.h>


int main(int argc, char* argv[])
{
	int retcode = 0;
	FILE *in = NULL;
	char mybuffer[100];
	int N, T;
	double alpha,pi1,pi2,p1,p2,rho;
    int j, h, t;
    double pi, *optimal, newprice, candidate, bestone, *shift;

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

	printf("Numer of assets: %d, Number of periods: %d\nalpha = %g, pi1 = %g, pi2 = %g, p1 = %g, p2 = %g, rho = %g\n",
		N, T, alpha,pi1,pi2,p1,p2,rho);

	fclose(in);


  pi = pi1;

  optimal = (double *)calloc((N + 1)*T, sizeof(double));
  if (!optimal){
	  printf("cannot allocate large matrix\n"); retcode = 2; goto BACK;
  }

 shift = (double *)calloc(N + 1, sizeof(double));

 for (j = 0; j <= N; j++)
	  shift[j] = 1 - alpha*pow((double)j, pi);
	  

  /** do last stage **/
  for (j = 0; j <= N; j++){
	  newprice = shift[j];
	  optimal[(T - 1)*(N + 1) + j] = newprice*j;
	  // V[k,t] stored at optimal[t*(N+1) + k] 
  }
  for (t= T - 2; t>= 0; t--){
	  for (j = 0; j <= N; j++){

		  bestone = 0;
		  /** enumerate possibilities **/
		  for (h = 0; h <= j; h++){
			  newprice = shift[h];
			  candidate = newprice*h + newprice*optimal[(t + 1)*(N + 1) + j - h];

			  if (candidate > bestone)
				  bestone = candidate;
		  }
		  optimal[t*(N + 1) + j] = bestone;
	  }
	  printf("done with stage t = %d\n", t);
  }

  printf("optimal value for trade sequencing = %g\n", optimal[N]);

BACK:
  printf("\nran with code %d\n", retcode);
  return retcode;
}