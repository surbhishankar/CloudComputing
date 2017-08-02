//File:         VehicleParking.cpp
//Project name: To simulate the datacenter for a vehicle parking lot in an Airport.
//Description:  The goal of the project is to simulate a datacenter implemented on the cars in the parking lot of a medium sized airport.
//              As cars arrive and depart randomly, the challenge facing the implementation of the datacenter is to maintain high availability
//              and reliability in the face of the dynamically changing resources. Tradeoffs will be identified and analyzed and several
//              possible solutions will be contrasted. The project is deliberately open ended, allowing each student to add performance-enhancing
//              "bells and whistles".
//Author1:      Aida Ghazizadeh
//Email:        aghaziza@cs.odu.edu
//Author2:      Srinivas Havanur
//Email:        shavanur@cs.odu.edu
//Submitted to: Dr. Stephen Olariu
//Submitted on: 04 December 2015
//Language:     C++


#include<iostream>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
using namespace std;

void RegionCenter1();
void RegionCenter2();
void RegionCenter3();
void RegionCenter4();

int jobs_completed =0;
int count_migrated_successfully = 0;
int count_failed_to_migrate = 0;
int Array_first_spot_of_cluster[65];
int turned_away_count =0;
const int END_TIME = 1440;                                                    // The total time for which our simulation runs in minutes. In other words, the simulation will run for
int spot_var;
const double HD_bandwidth = 12000;                                            // The disk bandwidth 200MB/s is expressed in minutes. This disk bandwidth is considered from hennessy-patterson text book.
                                                                              //Disk bandwidth is useful to calculate the time it takes to store the data to the local disk of car.
int jobfailCounter=0, jobPassCounter=0;


class spot_location                                                          //This class includes all information about the parking spots and also it keeps track of whether parking spot is occupied or not.
{


    public: int spot_number;
		 int cluster_number;
		 int region_number;
		 int group_center_number;
		 int occupied;
		 int occupied_by_car_number;

	public: void spot_locations(int sno, int cno, int rno, int gcnumber, int occup)
		{
			spot_number = sno;
			cluster_number = cno;
			region_number = rno;
			group_center_number = gcnumber;
			occupied = occup;
	        }

		void display_spot_information(ofstream& outFile)                    // This displays all information about the parking spots.
		{
			outFile<<"\n";
			outFile<<"-----------------------------------------"<<"\n";
			outFile<<"		Spot Details		        "<<"\n";
			outFile<<"-----------------------------------------"<<"\n";
			outFile<<"Spot number : "<<spot_number<<"\n";
			outFile<<"Cluster number : "<<cluster_number<<"\n";
			outFile<<"Region number : "<<region_number<<"\n";
			outFile<<"Group Center number : "<<group_center_number<<"\n";
			outFile<<"Occupied : "<<occupied<<"\n";
		}


};

class cars                                                                // This includes information about all the cars including which spot the car is currently parked, along with arrival and departure time.
{
	public: int car_number;
		 int car_spot_number;
		 int arrival_time_of_car;
		 int departure_time_of_flight;
		 int arrival_time_of_flight;
		 int departure_time;
		 int job_number;
		 int residency_time;
		 int turned_away;
		 bool busy;                                                       // to check if this car has a job or doesn't have a job
         bool nojobsmigrated;
		 cars()
		 {
		     turned_away=-1;                                             // This is to keep track how many cars turned away when they try to occupy the parking space. Initially set to -1.
                                                                         // turned_away=0 - Car didn't turn away or got parking space.
                                                                         // turned_away=1 - Car turned away
             car_spot_number=-1;
             nojobsmigrated=false;
         }

        void display_car_information(ofstream& outFile)                  // Displays all car information including the spot number to which the car belongs to. Initially when the simulation starts
		{                                                                // all the parking spots will be occupied by cars.
			outFile<<"\n";
			outFile<<"-----------------------------------------"<<"\n";
			outFile<<"		Car Details		        "<<"\n";
			outFile<<"-----------------------------------------"<<"\n";
			outFile<<"car number : "<<car_number<<"\n";
			outFile<<"car spot number : "<<car_spot_number<<"\n";
			outFile<<"job number assigned to this car : "<<job_number<<"\n";
			outFile<<"departure time of this car : "<<departure_time<<"\n";
            outFile<<"arrival time of this car : "<<arrival_time_of_car<<"\n";
            outFile<<"The Residency time: "<<residency_time<<"\n";
            outFile<<"Car Turned away: "<<turned_away<<"\n";


		}






        void calculate_departure_time(){                                //Calculates the departure time of a car when simulation starts. This is between 1-2560 cars

            departure_time= rand()%(2880-0);                            // Maximum Job duration time is 2 days and Minimum Job duration time is 0
        }

        void get_arrival_time_of_car(ifstream& inFile){                 // Car arrives 60 minutes earlier to the flight departure hence the arrival time.

            inFile>>departure_time_of_flight;
            arrival_time_of_car = departure_time_of_flight - 60;
        }
                                                                        // Get the departure time of car between 2561 to 5000 cars by making use of arrival time of a flight.
                                                                        //The departure time is 60 minutes after the arrival.
        void get_departure_time_of_car(ifstream& inFile){

            inFile>>arrival_time_of_flight;
            departure_time = arrival_time_of_flight + 60;
        }

        void calculate_residency_time_of_car()                          //Calculates the residency time of a car based on the arrival and departure time of a car.
        {
            residency_time = departure_time - arrival_time_of_car;
        }

        void display_failed_job_car_details(ofstream& outFileFailed)    //This displays the details of a car which may fail if we don't do the migration.
        {
            outFileFailed<<"\n";
			outFileFailed<<"-----------------------------------------"<<"\n";
			outFileFailed<<"		Failed Job Car Details		        "<<"\n";
			outFileFailed<<"-----------------------------------------"<<"\n";
			outFileFailed<<"car number : "<<car_number<<"\n";
			//outFileFailed<<"car spot number : "<<car_spot_number<<"\n";
			outFileFailed<<"job number assigned to this car : "<<job_number<<"\n";
			outFileFailed<<"departure time of this car : "<<departure_time<<"\n";
            outFileFailed<<"arrival time of this car : "<<arrival_time_of_car<<"\n";

        }

		void display_pass_job_car_details(ofstream& outFilePassJob)    //This displays the details of a car which may pass during the simulation.
        {
            outFilePassJob<<"\n";
			outFilePassJob<<"-----------------------------------------"<<"\n";
			outFilePassJob<<"		Pass Job Car Details		        "<<"\n";
			outFilePassJob<<"-----------------------------------------"<<"\n";
			outFilePassJob<<"car number : "<<car_number<<"\n";
			//outFilePassJob<<"car spot number : "<<car_spot_number<<"\n";
			outFilePassJob<<"job number assigned to this car : "<<job_number<<"\n";
			outFilePassJob<<"departure time of this car : "<<departure_time<<"\n";
            outFilePassJob<<"arrival time of this car : "<<arrival_time_of_car<<"\n";

        }






};

class jobs                                                            // This class stores information about the jobs.
{
    public:
    int car_number;                                                 //car number that the job belongs to
    int job_number;
    double VM_size;
    bool completed;
    bool assigned;
    double job_duration;                                            // in minute
    double data_this_job_will_produce;
    double data_storage_time;
    double job_duration_remained;
    int data1_car_number;
    string replicated_data;
    int data2_car_number;
    int data3_car_number;
    int data_replication_time_to_same_cluster;
    int data_replication_time_to_same_region;
    int migrated_successfully; // 2 for success and 3 for fail

    jobs()
    {
        data_replication_time_to_same_cluster=0;
        data_replication_time_to_same_region=0;

    }


    void display_job_information(ofstream& outFile)                 //This displays the job information which is assigned to all the cars in the parking lot as soon as simulation starts.
    {
			outFile<<"\n";
			outFile<<"-----------------------------------------"<<"\n";
			outFile<<"		Job Details		        "<<"\n";
			outFile<<"-----------------------------------------"<<"\n";
			outFile<<"job number : "<<job_number<<"\n";
            outFile<<"job size : "<<VM_size<<"\n";
            outFile<<"car number that this job is assigned to : "<<car_number<<"\n";
            outFile<<"the amount of time this job takes to finish in minutes : "<<job_duration<<"\n";
            outFile<<"the amount of data that this job will produce : "<<data_this_job_will_produce<<"\n";
            outFile<<"the disk storage time: "<<data_storage_time<<endl;

    }



    void calculate_VM_size(){
        VM_size= rand()%(3000-1000)+1000;                           // our jobs are between 1 GB and 5 GB
    }


    void calculate_data_size(){
        data_this_job_will_produce= VM_size * 4;                    //we assume that a VM of size x will produce 4x data
    }


    void calculate_data_storage_time(){                             // This function is to calculate the time it takes to store it to local hard disk of a car.
        data_storage_time = data_this_job_will_produce/HD_bandwidth;
    }


    void calculate_data_replication_time_to_same_cluster(){                            // Time it takes to replicate the data within same cluster.
        data_replication_time_to_same_cluster=(data_this_job_will_produce/2)/600;     //In the Middle of the job, data will be replicated and it takes 10MB/sec link speed to transfer it to car in same cluster. The overall it takes 600MB per min.

    }


    void calculate_data_replication_time_to_same_region_different_cluster(){          //Time it takes to replicate the data to different cluster.
        data_replication_time_to_same_region= (data_this_job_will_produce/2)/600 + (data_this_job_will_produce/2)/60000 + (data_this_job_will_produce/2)/600 ;
        // car ---> AP ___> GC ___> AP -----> car
    }


    void calculate_job_duration(){                                                    // Calculates the time it takes to complete a job in a car.
        job_duration= (VM_size * 0.1)+data_storage_time+data_replication_time_to_same_cluster+data_replication_time_to_same_region; //we assume that 1 MB takes 1/10 minute to finish and we added data storage time. In simple words, 6seconds to complete 1MB
        job_duration_remained = job_duration;
    }



};


class Time
{
    public:
    int minute;

    Time(){
        minute = 0;
    }

    void increment(){
        minute++;
    }

} ;

cars cars_list[5000];

spot_location spot_loc[2560];

jobs jobs_list[5000];

void generate_array(){

    for(int i=0; i <=64; i++){

        Array_first_spot_of_cluster[i]=0;
    }

    Array_first_spot_of_cluster[1]=1;

    for(int i=2; i <=64; i++){

        Array_first_spot_of_cluster[i]=Array_first_spot_of_cluster[i-1] + 40;
    }


}

                                                                        //This function is to find the carnumber within the same cluster which has a longest job duration.
                                                                        //This will be useful to replicate the job to the car belonging to same cluster.
int max_departure (int spotnumber,int spot_number_of_this_job)
{
    int spot_num=spotnumber;
    for(int i=spotnumber+1;i<=spotnumber+39;i++)
    {
        if(i == spot_number_of_this_job)                                //This will skip the assigning the job to same car in the cluster.
        {

             continue;
        }

        if(cars_list[spot_num].departure_time <cars_list[i].departure_time)
        spot_num = i;

    }
    return cars_list[spot_num].car_number;
}

/*
int max_departure_within_cluster_for_migration (int cluster_number_of_this_car,int spot_number_of_this_job) //it should not be assigned to itself
{
    generate_array();
    int flag = false;
    int spotnumber = Array_first_spot_of_cluster[cluster_number_of_this_car];
    int spot_num=spotnumber;
    for(int i=spotnumber+1;i<=spotnumber+39;i++)
    {
        if(i == spot_number_of_this_job)                                //This will skip the assigning the job to same car in the cluster.
        {
          //  outSpot<<"Spot number that is skipped: "<<i<<endl;
             continue;
        }

        if(cars_list[spot_num].departure_time <cars_list[i].departure_time && cars_list[i].busy == false ) {
            spot_num = i;
            flag = true;
        }

    }
    if (flag == true)
        return cars_list[spot_num].car_number;
    else return -1;
}
*/

int max_residency_not_busy_car(int time){                       //This will find a car with the maximum residency time that doesn't have a job assigned to it.

    int car_num = -1;
    int residency_time_temp= -1;
    for ( int i=1; i<=5000; i++){

        if(cars_list[i].busy == false && cars_list[i].departure_time > time && cars_list[i].car_spot_number != -1 && cars_list[i].residency_time > residency_time_temp  ){

            residency_time_temp= cars_list[i].residency_time;
            car_num = cars_list[i].car_number;
        }

    }
    return car_num;

}


                                                            // This is to replicate the data to other cluster.
                                                            //This function gets the spot location of other cluster to which data should be replicated.
int generate_random_cluster_number_in_a_region(int GC_number_of_this_car, int spot_number_of_this_job, int cluster_number_of_this_car){

int car_number;
int spot_number;
int random_number;
    int random_cluster_number=0;
int GC_num;

generate_array();

random_number=1;
for(GC_num=1;GC_num<=16;GC_num++)
{
    if (GC_number_of_this_car==GC_num){

        //int random_cluster_number=cluster_number_of_this_car;
        random_cluster_number=0;

        do
        {
            random_cluster_number= rand()%4 + random_number ; //generate 1 to 4
        }while(random_cluster_number==cluster_number_of_this_car);

    car_number = max_departure(Array_first_spot_of_cluster[random_cluster_number],spot_number_of_this_job);

    }
   random_number=random_number + 4;

}


  return car_number;
}




int main()
{
    int car_participation_count=0, total_cars_count=0;
	//srand(time(0));
	RegionCenter1();                                        //This will initialize the spot locations of all clusters belonging to region center1.
	RegionCenter2();                                        //This will initialize the spot locations of all clusters belonging to region center2.
	RegionCenter3();                                        //This will initialize the spot locations of all clusters belonging to region center3.
	RegionCenter4();                                        //This will initialize the spot locations of all clusters belonging to region center4.
    ofstream outFile,outFileFailed,outFilePassJob,outFileCarParticipation,outFileTotalCarsArrived, outFileParkingLot, FailedMigratedJobs, outTurnedAway;
    ifstream inFile,inFile2;
    ofstream outTest, outTest2;
    inFile.open("input_departure_time_of_flight.txt");
    inFile2.open("input_arrival_time_of_flight.txt");
    outTurnedAway.open("TurnedAwayCars.txt");
    outFile.open("CarsAndJobDetails.txt");
    outFileFailed.open("PossibleFailedJob.txt");
	outFilePassJob.open("PossiblePassedJob.txt");
    outFileCarParticipation.open("cars_participation_simulation.txt");
    outFileTotalCarsArrived.open("total_cars_for_the_simulation.txt");
    outTest.open("replicationData.txt");
    outTest2.open("SuccessfullyMigratedJobs.txt");
    FailedMigratedJobs.open("FailedMigratedJobs.txt");
    ofstream outSpot;
    outSpot.open("SpotDetails.txt");
    outFileParkingLot.open("NewCarsAssignedtoSpot.txt");


                                                            // Assigning the carnumbers for all available 5000 cars
    for(int i=1; i<=5000; i++)
	{
		cars_list[i].car_number = i;
        cars_list[i].busy = false;
	}

                                                            //Initialize all spot numbers, job numbers, departure time to all the cars
    for(int i=1; i<=2560; i++)
	{
		cars_list[i].car_spot_number = spot_loc[i].spot_number;
		spot_loc[i].occupied_by_car_number = i ;
        cars_list[i].job_number=i;
        cars_list[i].calculate_departure_time();
		cars_list[i].calculate_residency_time_of_car();
		cars_list[i].turned_away=0;
        cars_list[i].busy = true;

	}




                                                            // Initialize -1 for cars outside the range of parking lot.
	for(int i=2561; i<=5000; i++)
	{
		cars_list[i].car_spot_number = -1;

	}
                                                            // Initialize all 5000 jobs for job class
    for(int i=1; i<=5000; i++)
	{
		jobs_list[i].job_number = i;
		jobs_list[i].calculate_VM_size();
        jobs_list[i].calculate_data_size();
        jobs_list[i].calculate_data_storage_time();
        jobs_list[i].calculate_data_replication_time_to_same_cluster();
        jobs_list[i].calculate_data_replication_time_to_same_region_different_cluster();
        jobs_list[i].calculate_job_duration();
        jobs_list[i].completed=false;
        jobs_list[i].assigned=false;

	}

                                                            // Assigning jobs to cars and setting the assigned flag for job class.
    for(int i=1; i<=2560; i++)
	{

        if(cars_list[i].job_number==jobs_list[i].job_number){
            jobs_list[i].car_number=cars_list[i].car_number;
            jobs_list[i].assigned=true;
        }
        else
        {
            jobs_list[i].car_number=-1;
            jobs_list[i].assigned=false;
        }
	}

                                                            // Getting the arrival time of car from the arrival list of flights from an input file.


    for(int i=2561; i<=5000; i++){

      cars_list[i].get_arrival_time_of_car(inFile);
      cars_list[i].get_departure_time_of_car(inFile2);
      cars_list[i].calculate_residency_time_of_car();
    }



    if(spot_loc[1].cluster_number>64)
    {
        spot_loc[1].spot_number=1;
        spot_loc[1].region_number=1;
        spot_loc[1].cluster_number=1;
        spot_loc[1].group_center_number=1;
        spot_loc[1].occupied=1;
    }


                                                            // Displaying all car and job information
    for(int i=1; i<=5000; i++)
	{

		cars_list[i].display_car_information(outFile);
		int job_number = jobs_list[i].job_number;
		jobs_list[job_number].display_job_information(outFile);
		if(cars_list[i].departure_time <= END_TIME)         // these are the cars that we can keep track of completely
		{
		    car_participation_count++;
		    cars_list[i].display_car_information(outFileCarParticipation);
		    outFileCarParticipation<<"Count of Cars that we can completely keep track of : "<<car_participation_count<<endl;
		}

		if(cars_list[i].arrival_time_of_car <= END_TIME)
        {
            total_cars_count++;
            cars_list[i].display_car_information(outFileTotalCarsArrived);
            //outFileTotalCarsArrived<<"Total Cars taken into account for the simulation: "<<total_cars_count<<endl;
        }


	}
                                                            //Displays all spot information.

for(int i=1;i<=2560;i++)
{
    spot_loc[i].display_spot_information(outSpot);
}


                                                            // Simulation time in minutes that is run for 24 hours which is equal to 1440 minutes.
    Time time;
    cout << "\nTime is: " <<time.minute;
    for (int i=1; i<=1440; i++){  //1440
        time.increment();
        cout << "\nTime is: " <<time.minute;
                                                            // If simulation time in minutes exceeds the departure time of car then it means that car
                                                            //has already left and need to initialize the spot location as -1 and job assigned should be set to false.
        for(int c=1; c<=5000; c++)
        {
                if(i>=cars_list[c].departure_time)
                {
                    spot_var = cars_list[c].car_spot_number;

                    spot_loc[spot_var].occupied_by_car_number = cars_list[c].car_number;

                    spot_loc[spot_var].occupied= -1;

                    int job_var = cars_list[c].job_number;

                    jobs_list[job_var].assigned = false; //if the car leaves we will un assign the job

                    cars_list[c].car_spot_number = -1;

                    cars_list[c].busy=false;


                }


                                                            // assigning new cars to the parking lot
                if( c>=2561 && cars_list[c].arrival_time_of_car == i && cars_list[c].car_spot_number == -1 && cars_list[c].turned_away != 1){
                    for (int s=1; s<=2560; s++){
                        if (spot_loc[s].occupied == -1 ){
                            cars_list[c].car_spot_number = spot_loc[s].spot_number;
                            spot_loc[s].occupied_by_car_number=cars_list[c].car_number;
                            spot_loc[s].occupied = 1;
                            cars_list[c].turned_away = 0;
                            cars_list[c].busy=false;
                           break;
                        }
                        else{
                            cars_list[c].turned_away = 1;

                        }

                    }
                }

                                                                //VM Migration: if residency time of the car is less than the job durationjob duration of that car; it means that the job may fail
                                                                //so we will migrate the job within 10 mins before the car leaves
                int job_num_var = cars_list[c].job_number;

                if ( cars_list[c].nojobsmigrated==false && cars_list[c].residency_time < jobs_list[job_num_var].job_duration &&  i >= cars_list[c].departure_time - 10 && cars_list[c].car_spot_number != -1){

                  //find the car with highest residency time
                  int temp_car_num_for_migration= max_residency_not_busy_car(i);

                  if (temp_car_num_for_migration != -1){
                          outTest2 << "we migrated this job number " << job_num_var <<" to car number " << temp_car_num_for_migration << " at time " << i << endl;
                          //migrate the job
                          cars_list[temp_car_num_for_migration].job_number= job_num_var;
                          cars_list[temp_car_num_for_migration].busy=true;
                          jobs_list[job_num_var].assigned=true;
                          cars_list[c].nojobsmigrated=true;
                          jobs_list[job_num_var].car_number=temp_car_num_for_migration;
                          jobs_list[job_num_var].migrated_successfully = 2;

                  }
                  else if (temp_car_num_for_migration == -1){

                         FailedMigratedJobs << "we failed to migrate this job number " << job_num_var << " at time" << i << endl;
                         jobs_list[job_num_var].migrated_successfully = 3;
                  }

                }



        }


                                                            // Set the status of the jobs if it is finished successully.
        for(int j=1; j<=2560; j++)
        {


            if (jobs_list[j].job_duration_remained <= 0){
                jobs_list[j].completed=true;
                int temp_car_number = jobs_list[j].car_number;
                cars_list[temp_car_number].busy=false;
            }

            if (jobs_list[j].assigned==true && jobs_list[j].completed==false)
                jobs_list[j].job_duration_remained--;


            int jobremained = (int)(jobs_list[j].job_duration_remained);
            int jobdur = (int)((jobs_list[j].job_duration)/2);
            if ( jobremained == jobdur  ){
                int car_number_of_this_job =  jobs_list[j].car_number;
                int spot_number_of_this_job = cars_list[car_number_of_this_job].car_spot_number;
                int cluster_number_of_this_job = spot_loc[spot_number_of_this_job].cluster_number;
               // outTest<<"Spot number of current job: "<<spot_number_of_this_job<<endl;

                outTest << "************************************cluster_number_of_this_job******************" << cluster_number_of_this_job<<endl;


                for (int cc=1 ; cc <= 5000 ; cc++){

                    int spot_number_of_this_car = cars_list[cc].car_spot_number;
                    int GC_number_of_this_car = spot_loc[spot_number_of_this_car].group_center_number;
                    int cluster_number_of_this_car = spot_loc[spot_number_of_this_car].cluster_number;
                    if((cluster_number_of_this_car == cluster_number_of_this_job) && spot_loc[spot_number_of_this_car].occupied!=-1){   /// change this if statement to find a car that is has the biggest residency time

                        //outTest <<  "spot_number_of_this_car-1:  " << spot_number_of_this_car << endl ;
                        jobs_list[j].data1_car_number =  max_departure(spot_number_of_this_car,spot_number_of_this_job);

                        //jobs_list[j].calculate_data_replication_time_to_same_cluster();
                        //jobs_list[j].calculate_data_replication_time_to_same_region_different_cluster();


                        outTest<<"Car number: "<<car_number_of_this_job<<" belonging to spot number: "<<spot_number_of_this_job<<" and cluster number: "<<cluster_number_of_this_job<<" is replicated to car "<<jobs_list[j].data1_car_number<<" belonging to same cluster"<<" at job duration "<<jobs_list[j].job_duration_remained<<endl;

                        int carnumber_another_cluster=generate_random_cluster_number_in_a_region(GC_number_of_this_car, spot_number_of_this_job, cluster_number_of_this_car);


                        outTest<<"Car number: "<<car_number_of_this_job<<" belonging to spot number: "<<spot_number_of_this_job<<" and cluster number: "<<cluster_number_of_this_job<<" is replicated to car "<<carnumber_another_cluster<< " at job duration "<<jobs_list[j].job_duration_remained<<endl;

                        break;


                    }

                }

            }

        }

        for(int sss=1 ; sss<=2560; sss++){
            int temp_occupied_by_car_number = spot_loc[sss].occupied_by_car_number;
            outFileParkingLot <<"At time: " << i << " Spot " << sss << " is occupied by Car number " << spot_loc[sss].occupied_by_car_number << endl;
                                    //" having job number " << cars_list[temp_occupied_by_car_number].job_number <<
                                    //" cars_list["<< temp_occupied_by_car_number << "].car_spot_number " <<cars_list[temp_occupied_by_car_number].car_spot_number  <<  endl;
        }

    }
                                                                                            // This is to check the jobs passed and failed for car and its respective output is generated.
        outFileFailed << "These are the jobs that might fail if we don't do the migration" << endl;
        outFilePassJob << "These are the jobs that will possibly succeed" << endl;
        for(int k=1; k<=2560;k++)
        {
			//cout<<"job duration: "<<jobs_list[k].job_duration<<"residency_time: "<<cars_list[k].residency_time<<endl;
            if(jobs_list[k].job_duration > cars_list[k].residency_time)
            {
				jobfailCounter++;
                cars_list[k].display_failed_job_car_details(outFileFailed);

            }
			else
			{
				jobPassCounter++;
				cars_list[k].display_pass_job_car_details(outFilePassJob);

			}

        }


        for (int i=1 ; i <= 5000; i++){

            if (jobs_list[i].migrated_successfully == 2){
                count_migrated_successfully++;
            }

            if (jobs_list[i].migrated_successfully == 3){
                count_failed_to_migrate++;
            }

            if (cars_list[i].turned_away == 1) {
                turned_away_count++;
            outTurnedAway << "car number " << cars_list[i].car_number << " got turned away" << endl;

            }

            if ( jobs_list[i].completed == true){
                jobs_completed++;
            }
        }

        outTurnedAway << "Total Cars turned away : "<<turned_away_count<<endl;
        outTest2 << "Count of jobs migrated successfully " << count_migrated_successfully << endl;
        FailedMigratedJobs <<  "Count of jobs failed to be migrated " << count_failed_to_migrate << endl;
		outFileFailed<< endl <<"Job failed count: "<<jobfailCounter<<endl;
		outFilePassJob<< endl << "Job Pass Count: "<<jobPassCounter<<endl;
		outFilePassJob << endl << "Total of jobs completed: " << jobs_completed << endl;


}


void RegionCenter1()
{

        //For Region1

        for(int i=1;i<=640;i++)
        {
                if( i <= 160)
                {
                        if( i <= 40 )
                        {
                                spot_loc[i].spot_locations(i,1,1,1,1);
                        }

                        if( i > 40  && i <= 80 )
                        {
                                spot_loc[i].spot_locations(i,2,1,1,1);
                        }

                        if( i > 80 && i <= 120 )
                        {
                                spot_loc[i].spot_locations(i,3,1,1,1);
                        }

                        if( i > 120 && i <= 160 )
                        {
                                spot_loc[i].spot_locations(i,4,1,1,1);

                        }
                }

                if( i > 160 && i <= 320)
                {
                        if( i > 160 && i <= 200 )
                        {
                                spot_loc[i].spot_locations(i,5,1,2,1);
                        }

                        if( i > 200  && i <= 240 )
                        {
                                spot_loc[i].spot_locations(i,6,1,2,1);
                        }

                        if( i > 240 && i <= 280 )
                        {
 				spot_loc[i].spot_locations(i,7,1,2,1);
                        }

                        if( i > 280 && i <= 320 )
                        {
                                spot_loc[i].spot_locations(i,8,1,2,1);

                        }
                }

                 if( i > 320 && i <= 480)
                {
                        if( i > 320 && i <= 360 )
                        {
                                spot_loc[i].spot_locations(i,9,1,3,1);
                        }

                        if( i > 360  && i <= 400 )
                        {
                                spot_loc[i].spot_locations(i,10,1,3,1);
                        }

                        if( i > 400 && i <= 440 )
                        {
                                spot_loc[i].spot_locations(i,11,1,3,1);
                        }

                        if( i > 440 && i <= 480 )
                        {
                                spot_loc[i].spot_locations(i,12,1,3,1);

                        }
                }

		if( i > 480 && i <= 640)
                {
                        if( i > 480 && i <= 520 )
                        {
                                spot_loc[i].spot_locations(i,13,1,4,1);
                        }

                        if( i > 520  && i <= 560 )
                        {
                                spot_loc[i].spot_locations(i,14,1,4,1);
                        }

                        if( i > 560 && i <= 600 )
                        {
                                spot_loc[i].spot_locations(i,15,1,4,1);
                        }

                        if( i > 600 && i <= 640 )
                        {
                                spot_loc[i].spot_locations(i,16,1,4,1);

                        }
                }
        }

}

void RegionCenter2()
{

        //For Region2

        for(int i=641;i<=1280;i++)
        {
                if( i <= 800)
                {
                        if( i <= 680 )
                        {
                                spot_loc[i].spot_locations(i,17,2,5,1);
                        }

                        if( i > 680  && i <= 720 )
                        {
                                spot_loc[i].spot_locations(i,18,2,5,1);
                        }

                        if( i > 720 && i <= 760 )
                        {
                                spot_loc[i].spot_locations(i,19,2,5,1);
                        }

                        if( i > 761 && i <= 800 )
                        {
                                spot_loc[i].spot_locations(i,20,2,5,1);

                        }
                }

                if( i > 800 && i <= 960)
                {
                        if( i > 800 && i <= 840 )
                        {
                                spot_loc[i].spot_locations(i,21,2,6,1);
                        }

                        if( i > 840  && i <= 880 )
                        {
                                spot_loc[i].spot_locations(i,22,2,6,1);
                        }

                        if( i > 880 && i <= 920 )
                        {
                                spot_loc[i].spot_locations(i,23,2,6,1);
                        }

                        if( i > 920 && i <= 960 )
                        {
                                spot_loc[i].spot_locations(i,24,2,6,1);

                        }
                }

                 if( i > 960 && i <= 1120)
                {
                        if( i > 960 && i <= 1000 )
                        {
                                spot_loc[i].spot_locations(i,25,2,7,1);
                        }

                        if( i > 1000  && i <= 1040 )
                        {
                                spot_loc[i].spot_locations(i,26,2,7,1);
                        }

                        if( i > 1040 && i <= 1080 )
                        {
                                spot_loc[i].spot_locations(i,27,2,7,1);
                        }

                        if( i > 1080 && i <= 1120 )
                        {
                                spot_loc[i].spot_locations(i,28,2,7,1);

                        }
                }

                if( i > 1120 && i <= 1280)
                {
                        if( i > 1120 && i <= 1160 )
                        {
                                spot_loc[i].spot_locations(i,29,2,8,1);
                        }

                        if( i > 1160  && i <= 1200 )
                        {
                                spot_loc[i].spot_locations(i,30,2,8,1);
                        }

                        if( i > 1200 && i <= 1240 )
                        {
                                spot_loc[i].spot_locations(i,31,2,8,1);
                        }

                        if( i > 1240 && i <= 1280 )
                        {
                                spot_loc[i].spot_locations(i,32,2,8,1);

                        }
                }
        }

}

void RegionCenter3()
{

        //For Region3

        for(int i=1281;i<=1920;i++)
        {
                if( i <= 1440)
                {
                        if( i <= 1320 )
                        {
                                spot_loc[i].spot_locations(i,33,3,9,1);
                        }

                        if( i > 1320  && i <= 1360 )
                        {
                                spot_loc[i].spot_locations(i,34,3,9,1);
                        }

                        if( i > 1360 && i <= 1400 )
                        {
                                spot_loc[i].spot_locations(i,35,3,9,1);
                        }

                        if( i > 1400 && i <= 1440 )
                        {
                                spot_loc[i].spot_locations(i,36,3,9,1);

                        }
                }

                if( i > 1440 && i <= 1600)
                {
                        if( i > 1440 && i <= 1480 )
                        {
                                spot_loc[i].spot_locations(i,37,3,10,1);
                        }

                        if( i > 1480  && i <= 1520 )
                        {
                                spot_loc[i].spot_locations(i,38,3,10,1);
                        }

                        if( i > 1520 && i <= 1560 )
                        {
                                spot_loc[i].spot_locations(i,39,3,10,1);
                        }

                        if( i > 1560 && i <= 1600 )
                        {
                                spot_loc[i].spot_locations(i,40,3,10,1);

                        }
                }

                 if( i > 1600 && i <= 1760)
                {
                        if( i > 1600 && i <= 1640 )
                        {
                                spot_loc[i].spot_locations(i,41,3,11,1);
                        }

                        if( i > 1640  && i <= 1680 )
                        {
                                spot_loc[i].spot_locations(i,42,3,11,1);
                        }

                        if( i > 1680 && i <= 1720 )
                        {
                                spot_loc[i].spot_locations(i,43,3,11,1);
                        }

                        if( i > 1720 && i <= 1760 )
                        {
                                spot_loc[i].spot_locations(i,44,3,11,1);

                        }
                }

                if( i > 1760 && i <= 1920)
                {
                        if( i > 1760 && i <= 1800 )
                        {
                                spot_loc[i].spot_locations(i,45,3,12,1);
                        }

                        if( i > 1800  && i <= 1840 )
                        {
                                spot_loc[i].spot_locations(i,46,3,12,1);
                        }

                        if( i > 1840 && i <= 1880 )
                        {
                                spot_loc[i].spot_locations(i,47,3,12,1);
                        }

                        if( i > 1880 && i <= 1920 )
                        {
                                spot_loc[i].spot_locations(i,48,3,12,1);

                        }
                }
        }

}

void RegionCenter4()
{

        //For Region1cars_list[i].car_spot_number

        for(int i=1921;i<=2560;i++)
        {
                if( i <= 2080)
                {
                        if( i <= 1960 )
                        {
                                spot_loc[i].spot_locations(i,49,4,13,1);
                        }

                        if( i > 1960  && i <= 2000 )
                        {
                                spot_loc[i].spot_locations(i,50,4,13,1);
                        }

                        if( i > 2000 && i <= 2040 )
                        {
                                spot_loc[i].spot_locations(i,51,4,13,1);
                        }

                        if( i > 2040 && i <= 2080 )
                        {
                                spot_loc[i].spot_locations(i,52,4,13,1);

                        }
                }

                if( i > 2080 && i <= 2240)
                {
                        if( i > 2080 && i <= 2120 )
                        {
                                spot_loc[i].spot_locations(i,53,4,14,1);
                        }

                        if( i > 2120  && i <= 2160 )
                        {
                                spot_loc[i].spot_locations(i,54,4,14,1);
                        }

                        if( i > 2160 && i <= 2200 )
                        {
                                spot_loc[i].spot_locations(i,55,4,14,1);
                        }

                        if( i > 2200 && i <= 2240 )
                        {
                                spot_loc[i].spot_locations(i,56,4,14,1);

                        }
                }

                 if( i > 2240 && i <= 2400)
                {
                        if( i > 2240 && i <= 2280 )
                        {
                                spot_loc[i].spot_locations(i,57,4,15,1);
                        }

                        if( i > 2280  && i <= 2320 )
                        {
                                spot_loc[i].spot_locations(i,58,4,15,1);
                        }

                        if( i > 2320 && i <= 2360 )
                        {
                                spot_loc[i].spot_locations(i,59,4,15,1);
                        }

                        if( i > 2360 && i <= 2400 )
                        {
                                spot_loc[i].spot_locations(i,60,4,15,1);

                        }
                }

                if( i > 2400 && i <= 2560)
                {
                        if( i > 2400 && i <= 2440 )
                        {
                                spot_loc[i].spot_locations(i,61,4,16,1);
                        }

                        if( i > 2440  && i <= 2480 )
                        {
                                spot_loc[i].spot_locations(i,62,4,16,1);
                        }

                        if( i > 2480 && i <= 2520 )
                        {
                                spot_loc[i].spot_locations(i,63,4,16,1);
                        }

                        if( i > 2520 && i <= 2560 )
                        {
                                spot_loc[i].spot_locations(i,64,4,16,1);

                        }
                }
        }

}
