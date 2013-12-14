/*
   LED Decodrer
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// Input data description
double           samplerate=1000000;
#define BITMASK 1

int main(int argc,char**argv) 
{ 
    static int popcnt[1024];
    FILE *inputfile;
    int samples;
     
    if (!(inputfile=fopen(argv[argc-1],"r")))
    {
        printf("Fatal error: could not open input file.\n");
        return -1;
    }

    double duty, dutysum=0;
    double sumnum=0;
    while (!feof(inputfile))
    {
        int highcount,lowcount;
        double hitime,lotime,duty;
        highcount=0;
        lowcount=0;
        
        // read one period
        while (fgetc(inputfile)&BITMASK) {highcount++;if (feof(inputfile)) break;}
        while (!fgetc(inputfile)&BITMASK) {lowcount++;if (feof(inputfile)) break;}
        
        hitime=(double)highcount/samplerate;
        lotime=(double)lowcount/samplerate;
        duty=hitime/(lotime+hitime);

        samples++;
        
        if (hitime>0.01)
        {
            samples=1;
            while(hitime>0.01)
            {
                printf("%f\n",1.0);
                hitime-=0.073;
            }                     
        }
        else
        {
            dutysum+=duty;
            sumnum+=1;
        }

        if (samples==32)
        {
            double dutyav;
            dutyav=dutysum/sumnum;
            
            printf("%f\n",dutyav);
            dutysum=0;
            sumnum=0;
            samples=0;           
        }        
    }
      
    fclose(inputfile);
}

