#include <string.h>
#include <stdio.h>
#include <bits/stdc++.h>
using namespace std;
int data_size;
void readInput(float input[100000][2],char filePath[100]){
    //read the input files and store them

    FILE *fptr;
    char doubleDot[20]="";
    if(fptr = fopen(filePath, "r"))
        printf("input file opened successfully\n");
    else {
        perror("Error");
        printf("fptr Null\n");
    }

    fscanf(fptr,"%d",&data_size);
    //cout<<size;
    for(int i=0;i<data_size;i++){
        fscanf(fptr,"%f %f",&input[i][0],&input[i][1]);

    }
    
    if ( fclose(fptr) != 0 )
    {
        printf( "Error closing input file.\n");
    }
}
void minuteToRad(float data[100000][2]){
    for(int i=0;i<data_size;i++){
        data[i][0]=data[i][0]/60*M_PI/180;
        data[i][1]=data[i][1]/60*M_PI/180;
    }
}

void histogramInit(int hist[1000]){
    //initialize a map to a histogram from 0 to 180 with 0.25 steps

    for(int i=0;i<721;i++)
        hist[i]=0;

}

float calDist(float a1,float s1,float a2,float s2){
    float theta=acos(sin(s1)*sin(s2)+cos(s1)*cos(s2)*cos(a1-a2));
    return theta*180/M_PI;
}
int binDeterminer(float num){
    int integerPart=(int)num;
    float diff=num-integerPart;
    if(diff<0.25){
        return integerPart;
    }
    else if(diff<0.5){
        return integerPart*4+1;
    }
    else if(diff<0.75){
        return integerPart*4+2;
    }
    else{
        return integerPart*4+3;
    }
}
void calculateDistance(int histogram[1000],float data1[100000][2],float data2[100000][2]){
    //calculate distance between every point of inputdatasets
    for(int i=0;i<data_size;i++)
        for(int j=0;j<data_size;j++){
            float cal=calDist(data1[i][0],data1[i][1],data2[j][0],data2[j][1]);
            int bin=binDeterminer(cal);
            histogram[bin]++;
        }

}

void storeHistogram(char filePath[100],int data[1000]){

    FILE *fptr;
    if(fptr = fopen(filePath, "w"))
        printf("output file opened successfully\n");
    for(int i=0;i<720;i++)
        fprintf(fptr,"%f : %d\n",i/4+(i%4)*0.2500,data[i]);

}
int main(){
    //data vectors
    
    float D_input[100000][2],R_input[100000][2];

    //histograms
    int DD[1000],RR[1000],DR[1000];
    histogramInit(DD);
    histogramInit(RR);
    histogramInit(DR);
    //read inputs
    char filePath[100]="data_100k_arcmin.dat";

    readInput(D_input,"data_100k_arcmin.dat");
    readInput(R_input,"rand_100k_arcmin.dat");
    //printf("here");
    int n=5;
    //fflush(stdout);

    

    //convert degree minutes to radian
    minuteToRad(D_input);
    minuteToRad(R_input);


    //calcualte the distance between two datasets and store it in the histogram
    calculateDistance(DD,D_input,D_input);
    calculateDistance(RR,R_input,R_input);
    calculateDistance(DR,D_input,R_input);

    //store the results
    storeHistogram("DD_1.dat",DD);
    storeHistogram("RR_1.dat",RR);
    storeHistogram("DR_1.dat",DR);
    return 0;
}
