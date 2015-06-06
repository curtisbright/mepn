#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <gmp.h>

int main(int argc, char** argv)
{
	DIR *dp;
	struct dirent *ep;

	if(argc<=2)
	{	printf("Initializes sieve files for all unsolved simple families\n");
		printf("between exponents n and m, given on the command-line\n");
		printf("which may then be processed by programs sieving and search\n");
	}

	mpz_t p;
	mpz_init(p);

	mkdir("srsieve", S_IRWXU);
	dp = opendir("./data");
	if(dp != NULL)
	{	while(ep = readdir(dp))
		{	char filename[100];
			char outfilename[100];
			strcpy(filename, ep->d_name);
			filename[8] = '\0';
			if(strcmp(filename, "unsolved")==0)
			{	strcpy(strchr(filename+9, '.'), "\0");
				int base = atoi(filename+9);
				sprintf(filename, "data/%s", ep->d_name);
				FILE* in = fopen(filename, "r");
				FILE* out;
				char line[100];
				int outopen = 0;
				if(fgets(line, 100, in)!=NULL)
				{	sprintf(outfilename, "srsieve/sieve.%d.txt", base);
					if(argc>2)
					{	out = fopen(outfilename, "w");
						outopen = 1;
					}
					rewind(in);
				}
				char start[100];
				char middle[2];
				char end[100];
				int count = 0;
				if(outopen)
					fprintf(out, "pmin=31\n");
				while(fgets(line, 100, in)!=NULL)
				{	count++;
					int l = (int)(strchr(line, '*')-line);
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
					mpz_neg(temp3, temp2);
					// Print family
					/*if((base-1)/g==1)
						gmp_printf("%s(%s)^n%s = %Zd*%d^n%+Zd\n", start, middle, end, temp, base, temp3);
					else
						gmp_printf("%s(%s)^n%s = (%Zd*%d^n%+Zd)/%d\n", start, middle, end, temp, base, temp3, (base-1)/g);*/
					
					if(outopen)
					{	gmp_fprintf(out, "%Zd*%d^n%+Zd\n", temp, base, temp3);

						char* divis = calloc(atoi(argv[2])+1, 1);
						for(int k=2; k<30; k++)
						{	if(!(k==2 || k==3 || k==5 || k==7 || k==11 || k==13 || k==17 || k==19 || k==23 || k==29))
								continue;

							int start = 0;
							int difference = 0;

							for(int num=atoi(argv[1]); num<=atoi(argv[1])+2*k; num++)
							{	mpz_ui_pow_ui(p, base, num);
								mpz_mul(p, p, temp);
								mpz_add(p, p, temp3);
								mpz_divexact_ui(p, p, (base-1)/g);
								if(mpz_divisible_ui_p(p, k)!=0)
								{	if(start == 0)
										start = num;
									else
									{	difference = num - start;
										break;
									}
								}
							}

							if(difference>0)
								for(int num=start; num<=atoi(argv[2]); num+=difference)
									divis[num] = 1;
						}
						for(int num=atoi(argv[1]); num<=atoi(argv[2]); num++)
							if(divis[num] == 0)
								fprintf(out, "%d\n", num);

						free(divis);
					}

					mpz_clears(x, y, z, temp, temp2, temp3, temp10, NULL);
				}
				fclose(in);
				if(outopen)
				{	fclose(out);
					char cpfilename[100];
					sprintf(cpfilename, "data/sieve.%d.txt", base);
					char copy[100];
					sprintf(copy, "cp %s %s", outfilename, cpfilename);
					system(copy);
				}
			}
		}
		(void)closedir(dp);
	}
	else
		perror("Couldn't open the directory");

	mpz_clear(p);

	return 0;
}
