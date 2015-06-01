#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <gmp.h>
#define MAXSTRING 1200000
#define GMPTESTCUTOFF 1000
#define PRINTPRIMO 0

int count = 0;

int main(int argc, char** argv)
{
	struct timeval start, end;
	DIR *dp;
	struct dirent *ep;
	mpz_t p;
	mpz_init(p);

	FILE* llrfile = fopen("remainingchecks.txt", "w");
	fprintf(llrfile, "ABC ($a*$b^$c$d)/$e\n");

	dp = opendir("./data");
	if(dp != NULL)
	{	while(ep = readdir(dp))
		{	char filename[100];
			strcpy(filename, ep->d_name);
			filename[7] = '\0';
			if(strcmp(filename, "minimal")==0)
			{	strcpy(strchr(filename+8, '.'), "\0");
				int n = atoi(filename+8);
				sprintf(filename, "data/%s", ep->d_name);
				FILE* in = fopen(filename, "r");
				//printf("Checking base %d...\n", n);
				int count = 0;
				int remaining = 0;
				gettimeofday(&start, 0);
				char line[MAXSTRING];
				while(fgets(line, MAXSTRING, in)!=NULL)
				{	line[strlen(line)-1] = '\0';
					mpz_set_str(p, line, n);

					char decimal[MAXSTRING];
					mpz_get_str(decimal, 10, p);
					int len = strlen(decimal);
					int basenlen = strlen(line);
					int result = 1;

					if(len < GMPTESTCUTOFF)
					{	count++;
						result = mpz_probab_prime_p(p, 1);
						if(result==0)
							gmp_printf("%s (base %d) not prime!\n", line, n);
					}
					else
					{	remaining++;
						int midpoint = strlen(line)/2;

						char start[100];
						char middle[2] = {line[midpoint], '\0'};
						char end[100];

						int i = midpoint;
						while(line[i]==middle[0])
							i--;
						line[i+1] = '\0';
						strcpy(start, line);
						line[i+1] = middle[0];

						i = midpoint;
						while(line[i]==middle[0])
							i++;
						strcpy(end, line+i);

						int zlen = strlen(end);
						mpz_t x, y, z, temp, temp2, temp3, temp10;
						mpz_inits(x, y, z, temp, temp2, temp3, temp10, NULL);
						mpz_set_str(x, start, n);
						mpz_set_str(y, middle, n);
						mpz_set_str(z, end, n);
						mpz_gcd_ui(temp10, y, n-1);
						int g = mpz_get_ui(temp10);
						mpz_divexact_ui(temp, y, g);
						mpz_set(temp2, temp);
						mpz_addmul_ui(temp, x, (n-1)/g);
						mpz_ui_pow_ui(temp3, n, zlen);
						//mpz_mul(temp, temp, temp3);
						mpz_mul(temp2, temp2, temp3);
						mpz_submul_ui(temp2, z, (n-1)/g);
						mpz_neg(temp3, temp2);

						gmp_fprintf(llrfile, "%Zd %d %d %+Zd %d\n", temp, n, basenlen-strlen(start), temp3, (n-1)/g);

						/*if((n-1)/g>1)
							if(mpz_cmp_ui(temp, 1)!=0)
								gmp_printf("(%Zd*%d^%d%+Zd)/%d,%d\n", temp, n, basenlen-strlen(start), temp3, (n-1)/g, len);
							else
								gmp_printf("(%d^%d%+Zd)/%d,%d\n", n, basenlen-strlen(start), temp3, (n-1)/g, len);
						else
							if(mpz_cmp_ui(temp, 1)!=0)
								gmp_printf("%Zd*%d^%d%+Zd,%d\n", temp, n, basenlen-strlen(start), temp3, len);
							else
								gmp_printf("%d^%d%+Zd,%d\n", n, basenlen-strlen(start), temp3, len);*/
						//printf("%s%c^(%d)%s (base %d) skipped\n", start, middle, basenlen-strlen(start)-strlen(end), end, basenlen, n, len);
					}
					if(PRINTPRIMO && result==1)
					{	mkdir("primo", S_IRWXU);
						char outfilename[100];
						sprintf(outfilename, "primo/base%d-%d.in", n, count+remaining);
						FILE* out = fopen(outfilename, "w");
						fprintf(out, "[Candidate]\n");
						gmp_fprintf(out, "N=%Zd\n", p);
						fclose(out);
					}
				}
				fclose(in);
				gettimeofday(&end, 0);
				double elapsed = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0F);
				printf("Completed base %d: %d minimal elements checked for probable primality in %.2f sec and %d remaining to check\n", n, count, elapsed, remaining);
			}
		}
		(void)closedir(dp);
	}
	else
		perror("Couldn't open the directory");

	mpz_clear(p);

	fclose(llrfile);

	return 0;
}
