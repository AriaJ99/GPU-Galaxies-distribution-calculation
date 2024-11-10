#include <string.h>
#include <stdio.h>
#include <bits/stdc++.h>
using namespace std;
int data_size=100000;
const int N = 101;

const int threadsInBlock = 1024;
void readInput(float input_a[100000],input_r[100000],char filePath[100]){
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
        fscanf(fptr,"%f %f",&input_a[i],&input_d[i]);

    }

    if ( fclose(fptr) != 0 )
    {
        printf( "Error closing input file.\n");
    }
}
void minuteToRad(float data[100000]){
    for(int i=0;i<data_size;i++)
        data[i]=data[i]/60*M_PI/180;

}

void histogramInit(int hist[1000]){
    //initialize a map to a histogram from 0 to 180 with 0.25 steps

    for(int i=0;i<721;i++)
        hist[i]=0;

}

__device__ float calDist(float a1,float s1,float a2,float s2){
    float theta=acos(sin(s1)*sin(s2)+cos(s1)*cos(s2)*cos(a1-a2));
    return theta*180/M_PI;
}
__device__ int binDeterminer(float num){
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
__global__ void calculateDistance(int histogram[1000],float data1_a[100000],data1_d[100000],float data2_a[100000],data2_d[100000]){
    //calculate distance between every point of inputdatasets
    int indexData1,indexData2,thread;
    thread=blockDim.x*blockIdx.x + threadIdx.x;
    indexData1=thread/data_size;
    indexData2=thread%data_size;

    float cal=calDist(data1_a[indexData1],data1_d[indexData1],data2_a[indexData2],data2_d[indexData2]);
    int bin=binDeterminer(cal);
    histogram[bin]++;


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

    float D_input_a[100000],D_input_d[100000],R_input_a[100000],R_input_d[100000];

    //histograms
    int DD[1000],RR[1000],DR[1000];
    histogramInit(DD);
    histogramInit(RR);
    histogramInit(DR);
    //read inputs
    char filePath[100]="data_100k_arcmin.dat";

    readInput(D_input_a,D_input_d,"data_100k_arcmin.dat");
    readInput(R_input_a,R_input_d,"rand_100k_arcmin.dat");

    //convert degree minutes to radian
    minuteToRad(D_input_a);
    minuteToRad(D_input_d);
    minuteToRad(R_input_a);
    minuteToRad(R_input_d);
    //cuda Arrays
    float* CD_input_a,CD_input_d,CR_input_a,CR_input_d;
    int* CDD,CRR,CDR;
    //allocate memory to cuda arrays
    size_t inputBytes=data_size*data_size*sizeof(float*),histBytes=1000*sizeof(int*);;

    int blockInGrid=data_size/threadsInBlock+2;
    cudaMalloc(&CD_input_a, inputBytes);
    cudaMalloc(&CD_input_d, inputBytes);
    cudaMalloc(&CR_input_a, inputBytes);
    cudaMalloc(&CR_input_d, inputBytes);
    cudaMalloc(&CDD,histBytes);
    cudaMalloc(&CRR,histBytes);
    cudaMalloc(&CDR,histBytes);
    cudaMemcpy(CD_input_a, D_input_a, inputBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(CD_input_d, D_input_d, inputBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(CR_input_a, R_input_a, inputBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(CR_input_d, R_input_d, inputBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(CDD, DD, histBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(CRR, RR, histBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(CDR, DR, histBytes, cudaMemcpyHostToDevice);

    //calcualte the distance between two datasets and store it in the histogram
    calculateDistanc<<<blockInGrid,threadsInBlock>>>>(CDD,CD_input_a,CD_input_d,CD_input_a,CD_input_d);
    calculateDistance<<<blockInGrid,threadsInBlock>>>(CRR,CR_input_a,CR_input_d,CR_input_a,CR_input_d);
    calculateDistance<<<blockInGrid,threadsInBlock>>>(CDR,CD_input_a,CD_input_d,CR_input_a,CR_input_d);
    //copy gpu in host
    cudaMemcpy(DD, CDD, histBytes, cudaMemcpyDeviceToHost);
    cudaMemcpy(RR, CRR, histBytes, cudaMemcpyDeviceToHost);
    cudaMemcpy(DR, CDR, histBytes, cudaMemcpyDeviceToHost);
    //store the results
    storeHistogram("DD.dat",DD);
    storeHistogram("RR.dat",RR);
    storeHistogram("DR.dat",DR);
    cudaFree(CD_input_a);
    cudaFree(CD_input_d);
    cudaFree(CR_input_a);
    cudaFree(CR_input_d);
    cudaFree(CDD);
    cudaFree(CRR);
    cudaFree(CDR);
    return 0;
}
