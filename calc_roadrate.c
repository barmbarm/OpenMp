#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include<sstream>
#include<vector>
#include<map>
#include<emmintrin.h>
#include<math.h>

using namespace std;

struct unit
{
    string info;
    int Area;
    float Value;
};

int main(int argc,char *argv[])
{
    if(argc != 4)
    {
        cout<<"argument number error!";
        return 1;
    }
    char* RoadFeeRate = argv[1];
    char* RoadRate = argv[2];
    int thread_num = atoi(argv[3]);

    //Read Chart1 into a map<string,value1> 
    ifstream infile(RoadFeeRate);
    string line;
    map<string, float> Chart1;
    while(getline(infile,line))
    {
        istringstream iss(line);
        int AreaNo,RoadNo,VehTypeNo;
        float FeeRate;
        if(!(iss >> AreaNo >> RoadNo >> VehTypeNo >> FeeRate))
            break;
        //cout<<AreaNo<<endl;

        string s = std::to_string(AreaNo) + std::to_string(RoadNo) \
            + std::to_string(VehTypeNo);

        //cout<<s<<endl;
        Chart1[s] = FeeRate;
    }
    
    //Read Chart2 into a vector<string,Area,value2>
    vector<unit> Chart2;
    ifstream infile2(RoadRate);
    //map<string,int> Count;
    while(getline(infile2,line))
    {
        istringstream iss(line);
        int AreaNo,RoadNo,InStationNo,OutStationNo,VehTypeNo,Zero;
        float Money;
        if(!(iss >> AreaNo >> RoadNo >> InStationNo >> OutStationNo \
            >> VehTypeNo >> Money >> Zero))
            break;
        string s = std::to_string(AreaNo) + std::to_string(RoadNo) \
            + std::to_string(VehTypeNo);
        unit re = {s,AreaNo,Money};
        Chart2.push_back(re);
        //Count[std::to_string(AreaNo)]++;     
    }
    
    //Preapare for parallel computing
    int recordNum = Chart2.size();
    float* ary1 = new float[recordNum];
    float* ary2 = new float[recordNum];
    for(int i=0; i<recordNum;i++)
    {
        ary1[i] = Chart1[Chart2[i].info];
        ary2[i] = Chart2[i].Value/1000;
        //cout<<ary2[i]<<endl;
    }
    
    clock_t tStart = clock();
    //parallel handle data
    #pragma omp parallel num_threads(thread_num) 
    {
        __m128 x;
        __m128 y;
        __m128 z;
        #pragma omp for schedule(dynamic)
        for(int i = 0; i<recordNum; i=i+4)
        {
            x = _mm_loadu_ps(ary1+i);
            y = _mm_loadu_ps(ary2+i);
            z = _mm_mul_ps(x,y);
            _mm_storeu_ps(ary2+i,z);
        }


    }
    /*
    for(int i=0;i<recordNum;i++)
    {
        ary2[i] = ary1[i]*ary2[i];
    }*/
    cout<<"Time Taken : "<<(double) (clock()-tStart)/CLOCKS_PER_SEC<<endl;
    //handle the rest
    for(int i=0;i<(recordNum%4);i++)
    {
        ary2[recordNum-1-i] = ary1[recordNum-1-i]*ary2[recordNum-1-i];
    }

    //Get sum up
    map<int, float> count;
    map<int, float> summary;
    float sum=0;
    for(int i=0; i<recordNum; i++)
    {
        sum += ary2[i];
        count[Chart2[i].Area]++;
        summary[Chart2[i].Area] += ary2[i];   
    }
    for(auto i= count.begin(); i!=count.end(); i++)
        cout<<"Area "<<i->first<<" : "<<summary[i->first]/i->second<<endl;
    cout<<"Total Average : "<<float(sum/recordNum)<<endl;
/*
    cout<<RoadFeeRate<<"   ";
    cout<<RoadRate<<"   ";
    cout<<ResultRoadRate<<"   ";
    cout<<thread_num<<"   ";*/
    cout<<endl;
    return 0;
}
