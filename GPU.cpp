#include <bits/stdc++.h>
using namespace std;
void readInput(vector<pair<float,float>>& input,string filePath){
    //read the input files and store them
    ifstream file;
    file.open("../"+filePath, ios::in);
    int size=0;
    file>>size;
    //cout<<size;
    for(int i=0;i<size;i++)
        file>>input[i].first>>input[i].second;
    file.close(); 
}
void minuteToRad(vector<pair<float,float>>& data){
    for(int i=0;i<data.size();i++){
        data[i].first=data[i].first/60*M_PI/180;
        data[i].second=data[i].second/60*M_PI/180;
    }
}
map<float,int> histogramInit(){
    //initialize a map to a histogram from 0 to 180 with 0.25 steps
    map<float,int> m;
    for(int i=0;i<720;i++)
        m[(float)i/4]=0;
    return m;
}
float calDist(float a1,float s1,float a2,float s2){
    float theta=acos(sin(s1)*sin(s2)+cos(s1)*cos(s2)*cos(a1-a2));
    return theta*180/M_PI;
}
float binDeterminer(float num){
    int integerPart=(int)num;
    float diff=num-integerPart;
    if(diff<0.25){
        return integerPart; 
    }
    else if(diff<0.5){
        return integerPart+0.2500;
    }
    else if(diff<0.75){
        return integerPart+0.5000;
    }
    else{
        return integerPart+0.7500;
    }
}
void calculateDistance(map<float,int>& histogram,vector<pair<float,float>>& data1,vector<pair<float,float>>& data2){
    //calculate distance between every point of inputdatasets
    for(int i=0;i<data1.size();i++)
        for(int j=0;j<data2.size();j++){
            float cal=calDist(data1[i].first,data1[i].second,data2[j].first,data2[j].second);
            float bin=binDeterminer(cal);
            histogram[bin]++;
        }

}

void storeHistogram(string filePath,map<float,int> data){

    ofstream file;
    file.open(filePath);
    for(map<float,int>::iterator it=data.begin();it!=data.end();++it)
        file<<it->first<<" : "<<it->second<<endl;

}
int main(){
    //data vectors
    vector<pair<float,float>> D_input(100000,{0,0}),R_input(100000,{0,0});
    //histograms
    map<float,int> DD=histogramInit();
    map<float,int> RR=histogramInit();
    map<float,int> DR=histogramInit();
    //read inputs
    readInput(D_input,"data_100k_arcmin.dat");
    readInput(R_input,"rand_100k_arcmin.dat");
    //convert degree minutes to radian
    minuteToRad(D_input);
    minuteToRad(R_input);
    //calcualte the distance between two datasets and store it in the histogram
    calculateDistance(DD,D_input,D_input);
    calculateDistance(RR,R_input,R_input);
    calculateDistance(DR,D_input,R_input);
    //store the results
    storeHistogram("DD.dat",DD);
    storeHistogram("RR.dat",RR);
    storeHistogram("DR.dat",DR);
    return 0;
}
