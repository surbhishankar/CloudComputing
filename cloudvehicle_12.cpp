#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const double bandwidth = 12000;  
const int End_time = 1440;
int first_spot_access_point[65], temp_spot, failcount = 0, jobcomplete = 0, migration_count = 0, failure_migration_count = 0, assign_count = 0, count_job_complete = 0;
void R1();
void R2();
void R3();
void R4();
void Generate_Array_for_access_point();





int timefile(){
	ofstream arr_outputfile;
	//ofstream dep_outputfile;
	int arr_time, dep_time;
	int again = 1;
	
	arr_outputfile.open("arr_time_input.txt");
	//dep_outputfile.open("dep_time_input.txt");
	for(int i = 2561; i < 7651; i++){
		arr_time = rand()%(1440-60) + 60;
	//	dep_time = arr_time + rand()%(1440-480) + 480;
		arr_outputfile << arr_time << endl;
		//dep_outputfile << dep_time << endl;
	}
	
	cout << "Would you like to continue?(0/1)" << endl;
	cin >> again;
	cout << endl;
	
	if(again == 1)
		return 0;
	else {
		cout << "Terminating the simulation" << endl;
		exit(1);
	}
}

class parking_lot_info
{
	public:
	int center_id, region_id, access_point_id, spot_id, spot_flag, car_num_in_spot;
	void parking_info(int cid, int rid, int clid, int sid, int flag);
	void show_parking_info(ofstream& outputfile, int i); 
} spot_num[2560];




// Information about the car which enters the parking lot.
class car
{
	public:
	int car_id, parking_spot, car_arr_time, car_dep_time, job_id, stay_time, success;
	bool occupied, migrate;
	car();
	void departure_after_simulation();
	void intial_departure();
	void show_car_info(ofstream& outputfile);
	void departure_time(ifstream& inputfile);
	void arrival_time(ifstream& inputfile);
	void stay_time_calculation();
	void show_job_complete_car_info(ofstream& CompletedOutput);
	void show_failure_car_info(ofstream& FailureOutput);
	
} cars[7560];

// This class is to hold the information about jobs that are assigned to cars in the parking lot.
class jobs_assigned
{
	public:
	int car_id, job_number; 
	int car_id_data1, car_id_data2, car_id_data3, time_taken_to_replicate_within_access_point, time_taken_to_replicate_within_region, success_migration;
	bool complete, assign;
	double VM_size, job_duration, basic_job_duration, time_taken_to_store_data, remaining_time_job;
	string data_replicated;
	double data_produced;
	// success_migration will have 2 for successful migration and 3 for failure
	
	jobs_assigned();
	//all calculations are carried out in these member functions
	void show_job_info(ofstream& outputfile);
	//void VM_size_calculation();
	void data_size();
	void data_storage_time();
	void data_replication_time_SameAccessPoint();
	void data_replication_time_DiffAccessPoint_SameRegion();
	void job_duration_calculation();
	
} jobs[7560];

// This class is created to keep track of time each car executes a job.
class Time_track
{
    public:
    int time_minutes;
	Time_track();
	void increment_time();
};

// member functions of class "parking_lot_info"
void parking_lot_info :: parking_info(int cid, int rid, int clid, int sid, int flag)
{
	center_id = cid;
	region_id = rid;
	access_point_id = clid;
	spot_id = sid;
	spot_flag = flag;
}


void parking_lot_info :: show_parking_info(ofstream& outputfile, int i)
{
	outputfile << endl << "////////////////////////////////// Parking lot " << i << " ////////////////////////////////// " << endl;
	outputfile <<" Center id of the parking spot :- " << center_id << endl;
	outputfile <<" Region id of the parking spot :- " << region_id << endl;
	outputfile <<" Cluster (Access Point) id of the parking spot :- " << access_point_id << endl;
	outputfile <<" Spot id of the parking spot :- " << spot_id << endl;
	outputfile <<" Spot availabililty (0/1) :- " << spot_flag << endl;
}		



// member function declarations of class "car"

car :: car()
{
	 success = -1; // Initiating this value with -1. This is used to keep track of number of cars which get a parking spot and assigned with a job successfully.
    // success = 0 - Car got a parking spot and success = 1 - Car did not get a parking spot
	parking_spot = -1;
    migrate = false; 
	// Migration happens only when a car is assigned to a job and it leaves before the completion of the job.
}


void car :: show_car_info(ofstream& outputfile)                  
{                                                                 
	outputfile << endl << "////////////////////////////////// Information about Car ////////////////////////////////// " << endl;

	outputfile << " Car ID: " << car_id << endl;
	outputfile << " Spot_id : " << parking_spot << endl;
	outputfile << " Job ID assigned : " << job_id << endl;
	outputfile << " Departure time: " << car_dep_time << endl;
	outputfile << " Arrival time: " << car_arr_time << endl;
	outputfile << " Stay time: " << stay_time << endl;
	outputfile << " Parking spot allotment: " << success << endl;


}
		
		
void car :: arrival_time(ifstream& inputfile)
{
	inputfile >> car_arr_time;	
}
void car :: show_job_complete_car_info(ofstream& CompletedOutput)
{
	CompletedOutput << endl;
	CompletedOutput << "--------------------- Cars which completed jobs successfully ---------------------" << endl;
	CompletedOutput << " Car - "<< car_id << endl;
	CompletedOutput << " Parking spot : " << parking_spot << endl;
	CompletedOutput << " Arrival time : " << car_arr_time << endl;
	CompletedOutput << " Job ID assigned : " << job_id << endl;
	CompletedOutput << " Departure time : " << car_dep_time << endl;
}

void car :: show_failure_car_info(ofstream& FailureOutput)
{
	FailureOutput <<"-------------------- Cars which failed to completed jobs assigned -------------------- " << endl;
	FailureOutput << "car number : " << car_id << endl;
	FailureOutput << " Parking spot : " << parking_spot << endl;
	FailureOutput << " Job ID assigned : "<< job_id << endl;
	FailureOutput << " Departure time : " << car_dep_time <<endl;
    FailureOutput << " Arrival time : " << car_arr_time << endl;
}


void car :: departure_after_simulation()
{
	// Calculating departure timings of the car based on the job durations. 
	// Assuming that job duration can be anywhere between 0 to 1440 minutes, that is 1 day.
	//car_dep_time = rand()%(1440 - 0);
	car_dep_time = car_arr_time + 480;
}

/*
void car :: departure_time(ifstream& inputfile)
{
	// Calculating departure time based on arrival time of the car.
    inputfile >> car_arr_time;
    car_dep_time = car_arr_time + 480;
}
*/

void car :: stay_time_calculation()
{
	stay_time = car_dep_time - car_arr_time; // Amount of time the car stays in the parking lot.
}


//member functions of class "jobs_assigned"

jobs_assigned :: jobs_assigned()
{
	VM_size = 1000;
	data_produced = VM_size * 2;
	time_taken_to_replicate_within_region = 0;
	time_taken_to_replicate_within_access_point = 0;
	
	basic_job_duration = rand()%(24)+3;
}

void jobs_assigned :: show_job_info(ofstream& outputfile)
{
	outputfile << "------------------------- JOB Information -------------------------" << endl;
	outputfile << " Job ID : " << job_number << endl;
    outputfile << " Size of the Job assigned : " << VM_size << endl;
    outputfile << " This job is assigned to Car ID : "<< car_id << endl;
    outputfile << " Time taken to complete the job (minutes) : "<< job_duration << endl;
    outputfile << " Amount of data produced by this job : "<< data_produced << endl;
    outputfile << " Time taken for disk storage : "<< time_taken_to_store_data << endl;	
}

void jobs_assigned :: data_size()
{
	data_produced = rand()%(2000)+500;
}

void jobs_assigned :: data_storage_time()
{
	time_taken_to_store_data = data_produced/bandwidth;
}

void jobs_assigned :: data_replication_time_SameAccessPoint()
{
	time_taken_to_replicate_within_access_point = (data_produced/2)/600;
	// Data replication happens during the execution of a job. According to our assumption, it takes 10MB/sec link speed to transfer the data to a car located in the same access_point and hence 600MB/minute is considered for calculation.
}

void jobs_assigned :: data_replication_time_DiffAccessPoint_SameRegion()
{
	time_taken_to_replicate_within_region = (data_produced/2)/600 + (data_produced/2)/60000 +  (data_produced/2)/600;
	// This calculates the time taken to replicate the data while the job is being carried out and the car is located in a different access_point but lpcated in the same region.
}

void jobs_assigned :: job_duration_calculation()
{
	job_duration = basic_job_duration + time_taken_to_store_data + time_taken_to_replicate_within_access_point + time_taken_to_replicate_within_region;
	//According to our assumption, 10MB takes 1 minute to finish and hence we multiply 0.1 with VM_size. We are also adding the data storage time to this.
}


//member functions of class "Time_track". 
Time_track :: Time_track()
{
    time_minutes = 0;
}

void Time_track :: increment_time()
{
    time_minutes++;
}



// Cars are let into different parking lots based on regions, centers, access_points and a particular parking lot which is available. This is the initialization phase.
// Every region is divided into 4 equal centers and then into access_points. Each access_point contains 40 parking lots 
// There are 16 center and 64 access_points and 2560 parking lots in total	
void R1() {
int rid = 1, flag = 1; //region_id
//spot_id = i 
for(int i = 1; i < 641; i++)
{
	int cid1 = 1; // center_id = 1
		if( i < 161 )
		{
			if ( i < 41 )
			{
				// access_point_id = 1 
				spot_num[i].parking_info(cid1,rid,1,i,1);
			}
			if ( i > 40  && i < 81 )
			{
				// access_point_id = 2 
				spot_num[i].parking_info(cid1,rid,2,i,1);
			}
			if ( i > 80 && i < 121 )
			{
				// access_point_id = 3
				spot_num[i].parking_info(cid1,rid,3,i,1);
			}
			if ( i > 120 && i < 161 )
			{
				// access_point_id = 4
				spot_num[i].parking_info(cid1,rid,4,i,1);
			}
		}
		
		int cid2 = 2;// center_id = 2
		if( i > 160 && i < 321 )
		{
			if ( i > 160 && i < 201 )
			{
				// access_point_id = 5
				spot_num[i].parking_info(cid2,rid,5,i,1);
			}
			if ( i > 200  && i < 241 )
			{
				// access_point_id = 6 
				spot_num[i].parking_info(cid2,rid,6,i,1);
			}
			if ( i > 240 && i < 281 )
			{
				// access_point_id = 7 
				spot_num[i].parking_info(cid2,rid,7,i,1);
			}
			if ( i > 280 && i < 321 )
			{
				// access_point_id = 8 
				spot_num[i].parking_info(cid2,rid,8,i,1);
			}
		}
		
		int cid3 = 3;// center_id = 3
		if( i > 320 && i < 481 )
		{
			if ( i > 320 && i < 361 )
			{
				// access_point_id = 9
				spot_num[i].parking_info(cid3,rid,9,i,1);
			}
			if ( i > 360  && i < 401 )
			{
				// access_point_id = 10
				spot_num[i].parking_info(cid3,rid,10,i,1);
			}
			if ( i > 400 && i < 441 )
			{
				// access_point_id = 11
				spot_num[i].parking_info(cid3,rid,11,i,1);
			}
			if ( i > 440 && i < 481 )
			{
				// access_point_id = 12
				spot_num[i].parking_info(cid3,rid,12,i,1);
			}
		}
		
		int cid4 = 4;// center_id = 4
		if( i > 480 && i < 641 )
		{
			if ( i > 480 && i < 521 )
			{
				// access_point_id = 13
				spot_num[i].parking_info(cid4,rid,13,i,1);
			}
			if ( i > 520  && i < 561 )
			{
				// access_point_id = 14
				spot_num[i].parking_info(cid4,rid,14,i,1);
			}
			if ( i > 560 && i < 601 )
			{
				// access_point_id = 15
				spot_num[i].parking_info(cid4,rid,15,i,1);
			}
			if ( i > 600 && i < 641 )
			{
				// access_point_id = 16
				spot_num[i].parking_info(cid4,rid,16,i,1);
			}
		}
}
}

void R2() {
int rid = 2, flag = 1; // region_id = 2 and flag indicates spot availability 
//spot_id = i
for( int i=641; i < 1281; i++ )
{
	int cid1 = 5;// center_id = 5
		if( i < 801 )
		{
			if ( i < 681 )
			{
				// access_point_id = 17
				spot_num[i].parking_info(cid1,rid,17,i,1);
			}
			if ( i > 680  && i < 721 )
			{
				// access_point_id = 18 
				spot_num[i].parking_info(cid1,rid,18,i,1);
			}
			if ( i > 720 && i < 761 )
			{
				// access_point_id = 19
				spot_num[i].parking_info(cid1,rid,19,i,1);
			}
			if ( i > 760 && i < 801 )
			{
				// access_point_id = 20
				spot_num[i].parking_info(cid1,rid,20,i,1);
			}
		}
	int cid2 = 6;// center_id = 6
		if( i > 800 && i < 961 )
		{
			if ( i > 800 && i < 841 )
			{
				// access_point_id = 21
				spot_num[i].parking_info(cid2,rid,21,i,1);
			}
			if ( i > 840  && i < 881 )
			{
				// access_point_id = 22
				spot_num[i].parking_info(cid2,rid,22,i,1);
			}
			if ( i > 880 && i < 921 )
			{
				// access_point_id = 23 
				spot_num[i].parking_info(cid2,rid,23,i,1);
			}
			if ( i > 920 && i < 961 )
			{
				// access_point_id = 24
				spot_num[i].parking_info(cid2,rid,24,i,1);
			}
		}
		
		int cid3 = 7;// center_id = 7
		if( i > 960 && i < 1120 )
		{
			if ( i > 960 && i < 1001 )
			{
				// access_point_id = 25
				spot_num[i].parking_info(cid3,rid,25,i,1);
			}
			if ( i > 1000  && i < 1041 )
			{
				// access_point_id = 26
				spot_num[i].parking_info(cid3,rid,26,i,1);
			}
			if ( i > 1040 && i < 1081 )
			{
				// access_point_id = 27
				spot_num[i].parking_info(cid3,rid,27,i,1);
			}
			if ( i > 1080 && i < 1120 )
			{
				// access_point_id = 28
				spot_num[i].parking_info(cid3,rid,28,i,1);
			}
		}
		
		int cid4 = 8;// center_id = 8
		if( i > 1120 && i < 1281 )
		{
			if ( i > 1120 && i < 1161 )
			{
				// access_point_id = 29
				spot_num[i].parking_info(cid4,rid,29,i,1);
			}
			if ( i > 1160  && i < 1201 )
			{
				// access_point_id = 30
				spot_num[i].parking_info(cid4,rid,30,i,1);
			}
			if ( i > 1200 && i < 1241 )
			{
				// access_point_id = 31
				spot_num[i].parking_info(cid4,rid,31,i,1);
			}
			if ( i > 1240 && i < 1281 )
			{
				// access_point_id = 32
				spot_num[i].parking_info(cid4,rid,32,i,1);
			}
		}
}
}


void R3() {
int rid = 3, flag = 1; // region_id = 2 and flag indicates spot availability 
//spot_id = i
for( int i = 1281; i < 1921; i++ )
{
	int cid1 = 9;// center_id = 9
		if( i < 1441 )
		{
			if ( i < 1321 )
			{
				// access_point_id = 33
				spot_num[i].parking_info(cid1,rid,33,i,1);
			}
			if ( i > 1320  && i < 1361 )
			{
				// access_point_id = 34 
				spot_num[i].parking_info(cid1,rid,34,i,1);
			}
			if ( i > 1360 && i < 1401 )
			{
				// access_point_id = 35
				spot_num[i].parking_info(cid1,rid,35,i,1);
			}
			if ( i > 1400 && i < 1441 )
			{
				// access_point_id = 36
				spot_num[i].parking_info(cid1,rid,36,i,1);
			}
		}
	int cid2 = 10; // center_id = 10
		if( i > 1440 && i < 1601 )
		{
			if ( i > 1440 && i < 1481 )
			{
				// access_point_id = 37
				spot_num[i].parking_info(cid2,rid,37,i,1);
			}
			if ( i > 1480  && i < 1521 )
			{
				// access_point_id = 38
				spot_num[i].parking_info(cid2,rid,38,i,1);
			}
			if ( i > 1520 && i < 1561 )
			{
				// access_point_id = 39 
				spot_num[i].parking_info(cid2,rid,39,i,1);
			}
			if ( i > 1560 && i < 1601 )
			{
				// access_point_id = 40
				spot_num[i].parking_info(cid2,rid,40,i,1);
			}
		}
		
		int cid3 = 11;// center_id = 11
		if( i > 1600 && i < 1761 )
		{
			if ( i > 1600 && i < 1641 )
			{
				// access_point_id = 41
				spot_num[i].parking_info(cid3,rid,41,i,1);
			}
			if ( i > 1640  && i < 1681 )
			{
				// access_point_id = 42
				spot_num[i].parking_info(cid3,rid,42,i,1);
			}
			if ( i > 1680 && i < 1721 )
			{
				// access_point_id = 43
				spot_num[i].parking_info(cid3,rid,43,i,1);
			}
			if ( i > 1720 && i < 1761 )
			{
				// access_point_id = 44
				spot_num[i].parking_info(cid3,rid,44,i,1);
			}
		}
		
		int cid4 = 12; // center_id = 12
		if( i > 1760 && i < 1921 )
		{
			if ( i > 1760 && i < 1801 )
			{
				// access_point_id = 45
				spot_num[i].parking_info(cid4,rid,45,i,1);
			}
			if ( i > 1800  && i < 1841 )
			{
				// access_point_id = 46
				spot_num[i].parking_info(cid4,rid,46,i,1);
			}
			if ( i > 1840 && i < 1881 )
			{
				// access_point_id = 47
				spot_num[i].parking_info(cid4,rid,47,i,1);
			}
			if ( i > 1880 && i < 1921 )
			{
				// access_point_id = 48
				spot_num[i].parking_info(cid4,rid,48,i,1);
			}
		}
}
}


void R4() {
int rid = 4, flag = 1; // region_id = 4 and flag indicates spot availability 
//spot_id = i
for( int i = 1921; i < 2561; i++ )
{
	int cid1 = 13;// center_id = 13
		if( i < 2081 )
		{
			if ( i < 1961 )
			{
				// access_point_id = 49
				spot_num[i].parking_info(cid1,rid,49,i,1);
			}
			if ( i > 1960  && i < 2001 )
			{
				// access_point_id = 50 
				spot_num[i].parking_info(cid1,rid,50,i,1);
			}
			if ( i > 2000 && i < 2041 )
			{
				// access_point_id = 51
				spot_num[i].parking_info(cid1,rid,51,i,1);
			}
			if ( i > 2040 && i < 2081 )
			{
				// access_point_id = 52
				spot_num[i].parking_info(cid1,rid,52,i,1);
			}
		}
	int cid2 = 14; // center_id = 10
		if( i > 2080 && i < 2241 )
		{
			if ( i > 2080 && i < 2121 )
			{
				// access_point_id = 53
				spot_num[i].parking_info(cid2,rid,53,i,1);
			}
			if ( i > 2120  && i < 2161 )
			{
				// access_point_id = 54
				spot_num[i].parking_info(cid2,rid,54,i,1);
			}
			if ( i > 2160 && i < 2201 )
			{
				// access_point_id = 55 
				spot_num[i].parking_info(cid2,rid,55,i,1);
			}
			if ( i > 2200 && i < 2241 )
			{
				// access_point_id = 56
				spot_num[i].parking_info(cid2,rid,56,i,1);
			}
		}
		
		int cid3 = 15;// center_id = 11
		if( i > 2240 && i < 2400 )
		{
			if ( i > 2240 && i < 2281 )
			{
				// access_point_id = 57
				spot_num[i].parking_info(cid3,rid,57,i,1);
			}
			if ( i > 2280  && i < 2321 )
			{
				// access_point_id = 58
				spot_num[i].parking_info(cid3,rid,58,i,1);
			}
			if ( i > 2320 && i < 2361 )
			{
				// access_point_id = 59
				spot_num[i].parking_info(cid3,rid,59,i,1);
			}
			if ( i > 2360 && i < 2401 )
			{
				// access_point_id = 60
				spot_num[i].parking_info(cid3,rid,60,i,1);
			}
		}
		
		int cid4 = 16; // center_id = 12
		if( i > 2400 && i < 2561 )
		{
			if ( i > 2400 && i < 2441 )
			{
				// access_point_id = 61
				spot_num[i].parking_info(cid4,rid,61,i,1);
			}
			if ( i > 2440  && i < 2481 )
			{
				// access_point_id = 62
				spot_num[i].parking_info(cid4,rid,62,i,1);
			}
			if ( i > 2480 && i < 2521 )
			{
				// access_point_id = 63
				spot_num[i].parking_info(cid4,rid,63,i,1);
			}
			if ( i > 2520 && i < 2561 )
			{
				// access_point_id = 64
				spot_num[i].parking_info(cid4,rid,64,i,1);
			}
		}
}
}

void Generate_Array_for_access_point()
{
	int i;
	for( i = 0; i <65; i++)
	{
		first_spot_access_point[i] = 0;
	}
	first_spot_access_point[1] = 1;
	
	for( i = 0; i < 65; i++)
	{
		first_spot_access_point[i] = first_spot_access_point[i-1] + 40;
	}
}

// This function finds the car that stays for longest duration within a access_point. 
int max_dep(int spot, int parking_spot_of_job) 
{
	int i, spotnumber = spot;
	
	for( i = spot + 1; i <= spot + 40; i++)
	{
		if(i == parking_spot_of_job)
			continue;
		
		if (cars[spotnumber].car_dep_time < cars[i].car_dep_time)
		{
			spotnumber = i;
		}
	}
	
	return cars[spotnumber].car_id; // Returns the car ID of the car that has maximum departure time in the access_point.
}

// This function helps us identify cars which stay in the parking lot for maximum time without any jobs assigned, so that we can assign a new job or transfer the unfinished job when we migrate.
int max_stay_no_job_assigned(int time)
{
	int i, car_number = -1, temp_stay_time = -1;
	
	for(i = 1; i <= 7560; i++)
	{
		if(cars[i].occupied != true && cars[i].car_dep_time > time)
		{
			if(cars[i].stay_time > temp_stay_time && cars[i].parking_spot != -1)
			{
				temp_stay_time = cars[i].stay_time;
				car_number = cars[i].car_id;
			}
		}
	}
	
	return car_number;
}

// Data replication process
int random_access_point_id_generation( int car_cid, int car_spot_id, int car_access_point_number)
{
	int random_num = 1, rand_num_access_point = 0, car_num, spot_num, cid_of_car;
	
	Generate_Array_for_access_point();
	
	for(cid_of_car = 1; cid_of_car <= 16; cid_of_car++)
	{
		if(cid_of_car == car_cid)
		{
			rand_num_access_point = 0;
		
			do
			{
				rand_num_access_point = rand()%4 + random_num;
			} while(rand_num_access_point == car_access_point_number);
			
			car_num = max_dep(first_spot_access_point[rand_num_access_point], car_spot_id);
		}
		random_num = random_num + 4;
	}
	
	return car_num;
}


int main()
{
	timefile(); // creating input files for arrival and departure timings.
	
	
	ofstream outputfile, FailureOutput, CompletedOutput, arr_outputfile, dep_outputfile, Testoutput1, Testoutput2, outputNoParkingAllotted, Cars_in_simulation, TotalCars, outputSpot, outputParking, MigrationFailure, outSpot;
	ifstream inputfile, arr_inputfile1, dep_inputfile1;
	arr_inputfile1.open("arr_time_input.txt");
	outputNoParkingAllotted.open("NoParkingAllotted.txt");
	outputfile.open("Car_Info.txt");
	FailureOutput.open("Failed_Job_Info.txt");
	CompletedOutput.open("Completed_Job_Info.txt");
	Cars_in_simulation.open("Car_simulation_Info.txt");
	TotalCars.open("TotalCars_simulation.txt");
	Testoutput1.open("DataReplication.txt");
	Testoutput2.open("MigrationSuccessful.txt");
	MigrationFailure.open("MigrationFailure_Info.txt");
	outputSpot.open("ParkingDetails.txt");
	outputParking.open("NewCar.txt");
	
	int i, car_count, total_cars, p, s;
	//initializing Regions
	R1(); 
	R2();
	R3();
	R4();
	
	
	 for(i=1; i <= 7560; i++)
	{
		cars[i].car_id = i;
        cars[i].occupied = false;
	}
	
	//Initialize all the parking spots, jobs assigned and departure time is calculated.
	for( i = 1; i < 2561; i++)
	{
		cars[i].occupied = true;
		cars[i].success = 0;
		cars[i].parking_spot = spot_num[i].spot_id;
		spot_num[i].car_num_in_spot = i;
		cars[i].job_id = i;
		cars[i].departure_after_simulation();
		cars[i].stay_time_calculation();
	}
	
	for(int i = 2561; i < 7561; i++)
	{
		cars[i].parking_spot = -1;
	}
	
	
	// Calling all functions related to jobs. This will help assign jobs to all cars that are available in the parking lot
	for( i = 1; i < 7561; i++)
	{
		jobs[i].job_number = i;
		//jobs[i].car_id = spot_num[i].parking_lot_info;
		jobs[i].data_size();
		jobs[i].data_storage_time();
		jobs[i].data_replication_time_SameAccessPoint();
		jobs[i].data_replication_time_DiffAccessPoint_SameRegion();
		jobs[i].job_duration_calculation();
		jobs[i].assign = false;
		jobs[i].complete = false;
	}
	
	//function to assign jobs
	i = 1;
	while( i <= 2560)
	{
        if(cars[i].job_id==jobs[i].job_number)
		{
			jobs[i].assign=true;
            jobs[i].car_id = cars[i].car_id;    
			i++;
        }
        else
        {
			jobs[i].assign = false;
            jobs[i].car_id = -1;
			i++;
        }
	}
	
	
	//Calculating stay time depending on the arrival time of the car.
	for( i = 2561; i < 7561; i++ )
	{
		cars[i].arrival_time(arr_inputfile1);
		cars[i].stay_time_calculation();
	}
	
	
	// When we reach the last access_point, we have to reset
	if(spot_num[1].access_point_id > 64)
	{
		spot_num[1].center_id = 1;
		spot_num[1].access_point_id = 1;
		spot_num[1].region_id = 1;
		spot_num[1].spot_id = 1;
		spot_num[1].spot_flag = 1;
	}
	
	
	// After all the jobs are assigned, we show the details
	car_count = 0;
	total_cars = 0;
	for ( i = 1; i < 7561; i++)
	{
		cars[i].show_car_info(outputfile);
		
		int temp_job_number = jobs[i].job_number;
		jobs[temp_job_number].show_job_info(outputfile);
		if (End_time >= cars[i].car_dep_time)
		{
			car_count++;
			cars[i].show_car_info(Cars_in_simulation);
			Cars_in_simulation << "Count of cars that are involved in the simulation: " << car_count << endl;
		}
		
		if(End_time >= cars[i].car_arr_time)
			{
				total_cars++;
				cars[i].show_car_info(TotalCars);
				TotalCars << " Total cars in simulation: " << total_cars << endl;
			}
	}
	
	
	
	for(int i=1; i < 2561; i++)
	{
		spot_num[i].show_parking_info(outputSpot,i);
		spot_num[i].show_parking_info(outputSpot,i);
	}
	
	Time_track time;
	cout << " \nTime: " << time.time_minutes;
	
	for( i = 1; i <= 1440; i++)
	{
		time.increment_time();
		cout << " \nTime: " << time.time_minutes;
		
		// When the car departure time is more than the simulation time then everything has to be reset. The purpose of this function is the same
		for ( int j = 1; j < 7561; j++)
		{
			if( cars[j].car_dep_time <= i)
			{
				int temp_job;
				temp_spot = cars[j].parking_spot;
				spot_num[temp_spot].car_num_in_spot = cars[j].car_id;
				spot_num[temp_spot].spot_flag = -1;
				
				// when the car leaves then job must be migrated or if the whole simulation is over then everything has to be reset
				temp_job = cars[j].job_id;
				jobs[temp_job].assign = false;
				cars[j].occupied = false;
				cars[j].parking_spot = -1;				
			}
			
			// New cars in the parking lot are given spots
			if( j > 2560 && cars[j].parking_spot == -1 && i == cars[j].car_arr_time && cars[j].success != 1)
			{
				int k = 1;
				while( k < 2561 )
				{
					if( spot_num[k].spot_flag == -1)
					{
						cars[j].parking_spot = spot_num[k].spot_id;
						spot_num[k].car_num_in_spot = cars[j].car_id;
						cars[j].occupied = false;
						cars[j].success = 0;
						spot_num[k].spot_flag = 1;
						k++;
						break;
					}
					else
					{
						cars[j].success = 1;
					}
				}
			}
			
			
			int temp_job_num = cars[j].job_id;
			int temp_car_id_migrate; 
			
			if (cars[j].migrate == false && cars[j].stay_time < jobs[temp_job_num].job_duration && i >= cars[j].car_dep_time - 10 && cars[j].parking_spot != -1)
			{
				temp_car_id_migrate = max_stay_no_job_assigned(i);
				
				if( temp_car_id_migrate != -1)
				{
					Testoutput2 << "Migrated to Car ID: " << temp_car_id_migrate << endl << " Migrated from Job ID: " << temp_job_num << endl << "Time of migration:" << i << endl; 
					cars[temp_car_id_migrate].job_id = temp_job_num;
					cars[j].migrate = true;
					jobs[temp_job_num].assign = true;
					cars[temp_car_id_migrate].occupied = true;
					jobs[temp_job_num].car_id = temp_car_id_migrate;
					jobs[temp_job_num].success_migration = 2;
				}
				else if( temp_car_id_migrate == -1)
				{
					FailureOutput << "Migration failure Time: " << i << endl << " Job ID: " << temp_job_num << endl;
					jobs[temp_job_num].success_migration = 3;
				}					
			}
		}
		
		// Jobs successfully completed
		
	
		for ( int m = 1; m < 2561; m++ )
		{
			int temp_car_id, temp_remaining_job, temp_job_time, n;
			if( jobs[m].remaining_time_job <= 0)
			{
				temp_car_id = jobs[m].car_id;
				jobs[m].complete = true;
				cars[temp_car_id].occupied = false;
			}
			
			if(jobs[m].complete == false && jobs[m].assign == true )
			{
				jobs[m].remaining_time_job = jobs[m].remaining_time_job - 1;
			}
			
			temp_remaining_job = (int)(jobs[m].remaining_time_job);
			temp_job_time = (int)((jobs[m].job_duration)/2);
			
			if ( temp_job_time == temp_remaining_job)
			{
				int this_car_id, this_spot_id, this_access_point_id;
				
				this_car_id = jobs[m].car_id;
				this_spot_id = cars[this_car_id].parking_spot;
				this_access_point_id = spot_num[this_spot_id].access_point_id;
				
				Testoutput1 << " Current job's Spot ID: " << this_spot_id << endl << " Current job's Cluster (Access Point) ID: " << this_access_point_id << endl;
				
				for ( n = 1; n < 7561; n++ )
				{
					int this_spot_id_car, this_center_id_car, this_access_point_id_car, diff_access_point_car_id;
					
					this_spot_id_car = cars[n].parking_spot;
					this_center_id_car = spot_num[this_spot_id].center_id;
					this_access_point_id_car = spot_num[this_spot_id].access_point_id;
					
					
					if ( this_access_point_id_car == this_access_point_id && spot_num[this_spot_id_car].spot_flag != -1)
					{
						jobs[m].car_id_data1 =  max_dep(this_spot_id_car, this_spot_id);
						
						Testoutput1 << " Car ID: " << this_car_id << endl << " Spot ID: " << this_spot_id << endl << " AccessPoint_id : " << this_access_point_id << endl << " (Belongs to same access_point) Data replication to the car: " << jobs[m].car_id_data1 << " Job duration: " << jobs[m].remaining_time_job << endl;
						
						random_access_point_id_generation( this_center_id_car, this_spot_id, this_access_point_id_car );
						
						Testoutput1 << " Car ID: " << this_car_id << endl << " Spot ID: " << this_spot_id << endl << " AccessPoint_id : " << this_access_point_id << endl << " Cluster (Access Point) ID: " << this_access_point_id << endl << " Job duration: " << jobs[m].remaining_time_job << endl;
						
						break;
					}
				}
			}
		}
		
		int temp_car_num_in_spot;
		for ( p = 1; p < 2561; p++ )
		{
			temp_car_num_in_spot = spot_num[p].car_num_in_spot;

			outputParking << "Car ID: " << spot_num[p].car_num_in_spot << endl << "Spot ID: " << p << endl << "Time: " << i << endl; 
		}
	}
	
	// Jobs successfully completed jobs or not?
	
	CompletedOutput << "Possible completion of jobs - " << endl;
	FailureOutput << "Possible failure of jobs - " << endl;
	
	for ( s = 1; s < 2561; s++ )
	{
		if ( jobs[s].job_duration > cars[s].stay_time )
		{
			failcount++;
			cars[s].show_failure_car_info(FailureOutput);
		}
		
		else
		{
			jobcomplete++;
			cars[s].show_job_complete_car_info(FailureOutput);
		}
	}
	
	for (int i = 1 ; i < 7561; i++ )
	{

        if (jobs[i].success_migration == 2)
		{
            migration_count++;
        }

        if (jobs[i].success_migration == 3)
		{
            failure_migration_count++;
        }

        if (cars[i].success == 1) 
		{
            assign_count++;
			outputNoParkingAllotted << "Car ID which did not get a parking spot: " << cars[i].car_id << endl;
        }

        if ( jobs[i].complete == true)
		{
            count_job_complete++;
        }
    }
	
	outputNoParkingAllotted << "Number of cars for which jobs were not assigned : " << assign_count << endl;
    Testoutput2 << " Number of jobs successfully migrated: " << migration_count << endl;
    MigrationFailure << " Count of jobs failed to be migrated: " << failure_migration_count << endl;
	FailureOutput << " Number of job failures: "<< failcount <<endl;
	CompletedOutput << " Number of jobs completed successfully: " << jobcomplete <<endl;
	CompletedOutput << " Total number of jobs completed: " << count_job_complete << endl;
	cout << " Count of jobs failed to be migrated: " << failure_migration_count << endl;
}
