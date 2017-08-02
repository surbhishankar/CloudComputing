#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

int main()
{
    ofstream outFile;
    ofstream outFile2;

    int numbers;
    int numbers2;
    outFile.open("input_departure_time_of_flight.txt");
    outFile2.open("input_arrival_time_of_flight.txt");


    for(int i=2561;i<=50000;i++)
    {
        numbers = rand()%(1440-60)+ 60;
        outFile<<numbers<<"\n";
        numbers2 = numbers + rand()%(1440-60) + 60;
        outFile2<<numbers2<<"\n";
    }


}
