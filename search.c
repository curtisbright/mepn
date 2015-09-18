#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <gmp.h>
#include <stdbool.h>
#include <unistd.h>
#define MAXSTRING 1200000
#define min(a,b) (((a) < (b)) ? (a) : (b))

FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);

int subword(char* prime, char* start, char middle, char* end, int* k)
{	int i=0, j=0, l=0;
	*k = 0;
	while(1)
	{	if(j!=strlen(start))
		{	if(prime[i]==start[j])
				i++;
			j++;
		}
		else if(l==0 && prime[i]==middle)
		{	(*k)++;
			i++;
		}
		else
		{	if(prime[i]==end[l])
				i++;
			l++;
		}
		if(i==strlen(prime))
			return 1;
		else if(l==strlen(end)+1)
			return 0;
	}
}

int main(int argc, char** argv)
{
	DIR *dp;
	struct dirent *ep;

	mpz_t p;
	mpz_init(p);

	if(argc<=1 || (argc==2 && argv[1][0] != '-'))
	{	printf("After sieving has been done, this program uses LLR\n");
		printf("to search for prime candidates between exponents n and m\n");
		printf("Usage: ./search n m\n");
		printf("To only search base b: ./search n m b\n");
		printf("To only search the ith family in base b: ./search n m b i\n");
		printf("To search everything: ./search -\n");
		printf("\nNOTE: The program llr must be located in the base directory\n");
		return 0;
	}

	if(access("llr", F_OK)==-1)
	{	printf("The program llr must be located in the base directory\n");
		return 0;
	}

	int minexpfound;

	for(int i=atoi(argv[1]); argv[1][0] == '-' || i<=atoi(argv[2]); i = minexpfound)
	{	bool numsleft = false;
		dp = opendir("./data");
		if(dp == NULL)
		{	perror("Couldn't open the directory");
			exit(EXIT_FAILURE);
		}

		minexpfound = 0x7FFFFFFF;

		while(ep = readdir(dp))
		{	char filename[100];
			char infilename[100];
			char outfilename[100];
			strcpy(filename, ep->d_name);
			filename[8] = '\0';
			int origi = i;
			if(strcmp(filename, "unsolved")==0)
			{	strcpy(strchr(filename+9, '.'), "\0");
				int base = atoi(filename+9);
				if(argc>=4 && base!=atoi(argv[3]))
					continue;
				sprintf(infilename, "data/%s", ep->d_name);
				FILE* in = fopen(infilename, "r");
				while(in==NULL)
					in = fopen(infilename, "r");
				tmpnam(outfilename);
				FILE* out = fopen(outfilename, "w");
				char line[100];
				char start[100];
				char middle[2];
				char end[100];
				char candidate[MAXSTRING];
				int countfam = 0;

				while(fgets(line, 100, in)!=NULL)
				{	int l = (int)(strchr(line, '*')-line);
					middle[0] = line[l-1];
					middle[1] = '\0';
					line[strlen(line)-1] = '\0';
					line[l-1] = '\0';
					strcpy(start, line);
					strcpy(end, line+l+1);

					int zlen = strlen(end);
					mpz_t x, y, z, temp, temp2, temp3, temp10;
					mpz_inits(x, y, z, temp, temp2, temp3, temp10, NULL);
					mpz_set_str(x, start, base);
					mpz_set_str(y, middle, base);
					mpz_set_str(z, end, base);
					mpz_gcd_ui(temp10, y, base-1);
					int g = mpz_get_ui(temp10);
					mpz_divexact_ui(temp, y, g);
					mpz_set(temp2, temp);
					mpz_addmul_ui(temp, x, (base-1)/g);
					mpz_ui_pow_ui(temp3, base, zlen);
					mpz_mul(temp, temp, temp3);
					mpz_mul(temp2, temp2, temp3);
					mpz_submul_ui(temp2, z, (base-1)/g);
					mpz_neg(temp2, temp2);
					mpz_div(temp3, temp, temp3);

					char family[100];
					gmp_sprintf(family, "%Zd*%d^n%+Zd\n", temp, base, temp2);

					countfam++;
					if(argc>=5 && countfam!=atoi(argv[4]) && argv[4][0]!='-')
					{	fprintf(out, "%s%c*%s\n", start, middle[0], end);
						continue;
					}

					// Find an exponent to test
					int num = -1;
					char sievefilename[100], sievetmpfilename[100];
					sprintf(sievefilename, "data/sieve.%d.txt", base);
					FILE* sieve = fopen(sievefilename, "r");
					while(sieve==NULL)
						sieve = fopen(sievefilename, "r");
					while(fgets(line, 100, sieve)!=NULL)
					{	if(strcmp(line, family)==0)
						{	while(num<i && fgets(line, 100, sieve)!=NULL && strchr(line, '*')==NULL)
								num = atoi(line);
							break;
						}
					}
					fclose(sieve);

					if(num!=-1)
					{	minexpfound = min(minexpfound, num);
						numsleft = true;
					}

					if(num==-1 || num!=i)
					{	fprintf(out, "%s%c*%s\n", start, middle[0], end);
						continue;
					}

					memset(candidate, 0, MAXSTRING);
					strcpy(candidate, start);
					memset(candidate+strlen(candidate), middle[0], num);
					strcat(candidate, end);

					char kernelfilename[100];
					sprintf(kernelfilename, "data/minimal.%d.txt", base);

					char prime[MAXSTRING];
					int hassubword = 0;
					char output[1000000] = {0};

					FILE* kernel = fopen(kernelfilename, "r");
					while(kernel==NULL)
						kernel = fopen(kernelfilename, "r");
					printf("Checking %s(%c^%d)%s (base %d)...\n", start, middle[0], num, end, base);
					while(fgets(prime, MAXSTRING, kernel)!=NULL)
					{	prime[strlen(prime)-1] = '\0';
						int k;
						if(subword(prime, start, middle[0], end, &k)==1)
						{	if(k<=num)
							{	hassubword = 1;
								break;
							}
						}
					}
					fclose(kernel);

					if(hassubword)
					{	printf("%s(%c^%d)%s (base %d) has a kernel subword %s\n", start, middle[0], num, end, base, prime);

						// Remove the family from the sieve file
						sieve = fopen(sievefilename, "r");
						while(sieve==NULL)
							sieve = fopen(sievefilename, "r");
						tmpnam(sievetmpfilename);
						FILE* sieveout = fopen(sievetmpfilename, "w");
						char sieveline[100];
						int thisfamily = 0;
						while(fgets(sieveline, 100, sieve)!=NULL)
						{	if(strchr(sieveline, '*')!=NULL)
								thisfamily = 0;
							if(strcmp(sieveline, family)==0)
								thisfamily = 1;
							if(thisfamily==0)
								fprintf(sieveout, "%s", sieveline);
						}
						fclose(sieve);
						fclose(sieveout);
						remove(sievefilename);
						rename(sievetmpfilename, sievefilename);

						continue;
					}

					char llrstr[100], tmpllr[100];
					tmpnam(tmpllr);
					FILE* llrfile = fopen(tmpllr, "w");
					fprintf(llrfile, "ABC ($a*$b^$c$d)/$e\n");
					gmp_fprintf(llrfile, "%Zd %d %d %+Zd %d\n", temp3, base, num+zlen, temp2, (base-1)/g);
					fclose(llrfile);

					if(argc>=6)
						sprintf(llrstr, "./llr %s -d -a%s", tmpllr, argv[5]);
					else
						sprintf(llrstr, "./llr %s -d", tmpllr);
					printf("%s\n", llrstr);
					FILE* llrprocess = NULL;
					while(llrprocess==NULL)
						llrprocess = popen(llrstr, "r");
					int n = fread(output, 1, 999999, llrprocess);
					output[n] = '\0';
					pclose(llrprocess);
					if(strstr(output, "(Factored")!=NULL)
						*strstr(output, "(Factored") = '\0';
					printf("%s", output);

					if(strstr(output, "PRP")!=NULL)
					{	// Add prime to set of minimal primes
						FILE* append = fopen(kernelfilename, "a");
						while(append==NULL)
							append = fopen(kernelfilename, "a");
						fprintf(append, "%s\n", candidate);
						fclose(append);

						// Remove the family from the sieve file
						sieve = fopen(sievefilename, "r");
						while(sieve==NULL)
							sieve = fopen(sievefilename, "r");
						tmpnam(sievetmpfilename);
						FILE* sieveout = fopen(sievetmpfilename, "w");
						char sieveline[100];
						int thisfamily = 0;
						while(fgets(sieveline, 100, sieve)!=NULL)
						{	if(strchr(sieveline, '*')!=NULL)
								thisfamily = 0;
							if(strcmp(sieveline, family)==0)
								thisfamily = 1;
							if(thisfamily==0)
								fprintf(sieveout, "%s", sieveline);
						}
						fclose(sieve);
						fclose(sieveout);
						remove(sievefilename);
						rename(sievetmpfilename, sievefilename);
					}
					else if(strstr(output, "not prime")!=NULL)
					{	// Family is still unsolved
						fprintf(out, "%s%c*%s\n", start, middle[0], end);

						// Remove the exponent just tested from the sieve file
						sieve = fopen(sievefilename, "r");
						while(sieve==NULL)
							sieve = fopen(sievefilename, "r");
						tmpnam(sievetmpfilename);
						FILE* sieveout = fopen(sievetmpfilename, "w");
						char sieveline[100];
						int thisfamily = 0;
						while(fgets(sieveline, 100, sieve)!=NULL)
						{	if(strchr(sieveline, '*')!=NULL)
								thisfamily = 0;
							if(strcmp(sieveline, family)==0)
								thisfamily = 1;
							if(!(thisfamily==1 && strcmp(line, sieveline)==0))
								fprintf(sieveout, "%s", sieveline);
						}
						fclose(sieve);
						fclose(sieveout);
						remove(sievefilename);
						rename(sievetmpfilename, sievefilename);
					}
					else
					{	fprintf(out, "%s%c*%s\n", start, middle[0], end);
					}
				}
				fclose(out);
				fclose(in);
				remove(infilename);
				rename(outfilename, infilename);
			}
		}
		closedir(dp);

		if(numsleft==false && argv[1][0] == '-')
			break;
	}

	mpz_clear(p);

	return 0;
}
