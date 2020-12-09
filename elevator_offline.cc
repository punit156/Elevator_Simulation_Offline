#include<iostream>
#include<bits/stdc++.h>
#include<vector>
#include<sstream>
#include<fstream>
#include<string>
#include<algorithm>
#include<unordered_map>
using namespace std;

struct passengers{
        int id;
	int arr_floor;
        int dest_floor;
	int arr_time;
        int wait_time = 0;
	int served_by = -1;
	int in_elevator_time = 0;
	int in_time = 0;
	int off_time = 0;
        bool up;
        bool wait=false;
};
struct elevators{
        int id;
	int timer = -1;
        int current_floor = 1;
	int capacity=10;
        int floors_travelled = 0;
        vector<int> carrying;
        vector<int> stop_floors;
        bool up=true;
	bool any=false;
	bool dont_move = false;
	bool stay = false;
	bool is_first = true;
        bool is_stopped = false;
        int served = 0;
        int stop_time = 0;
};
int main(int argc, char *argv[])
{
	int finished = 0; //Passengers who have been dropped at their destination
	float avg_wait_time = 0;
	int max_wait_time = 0;
	int illegal = 0;
	float avg_time_to_dest = 0;
	int max_time_to_dest = 0;
	int k = stoi(argv[2]);  //k = number of elevators given in command line
	elevators elevator[k]; //Allocating space for k elevators
	vector<string> s_e[k]; 
	ifstream file(argv[3]); //Opening the file specified in argument 3, 'Input.txt'
	ofstream file1(argv[4]); //Opening the file to write.  'Schedule.txt'
	ofstream file2(argv[5]); //Opening the file to write. 's_e.txt'
	ofstream file3(argv[6]); //Opening the file to write. 's_p.txt'
	string line;          //Storing each line from passenger.txt
        int count = 0;  // For counting number of lines in the file, to allocate space for struct passengers
        vector<string> input;
//Reading the input file and storing each line in a vector called 'input', so it's easier to parse
        while(getline(file,line))  
        {
                input.push_back(line);
                count++;
        }
//Now we have the number of lines in the file so we know how much space we need for struct passengers
	passengers passenger[count];
	int index = 0;  //Index for struct passengers
	unordered_map<int,int> pass_info;  // Mapping passenger ID -> arrival_time (This is for passengers)
	unordered_map<int,int> index_info;  //Mapping passenger ID -> index in struct passengers (For passengers)
//Following is a hashmap for elevator
	unordered_map<int,int> elevator_info; //Mapping elevator ID -> index in struct elevators
//Looping through the input array,parsing each line and storing in appropriate data structures
	for(int i=0;i<input.size();i++)  
        {
                vector<string> command;
                if(input[i].size()==0) //blank line
                {
                        continue;
                }
                for(int j=0;j<input[i].size();j++) //Parsing each line character by character
                {
 //If there are multiple spaces between two arguments
                        while(j<input[i].size() && (input[i][j]==' ' || input[i][j]=='\t')) // Space removal 
			{
			        j++;
                        }
                        string tmp = "";
                        while(j<input[i].size() && input[i][j]!=' ')
                        {
                                tmp += input[i][j];
                                j++;
                        }
			command.push_back(tmp);
		} //End of for
 		//After this for command[0] will have ID, command[1] will have arr_time
		//command[2] will have arr_floor and command[3] will have dest_floor
//Converting the strings to integers
		int id = stoi(command[0]);
		int arr_time = stoi(command[1]);
		int arr_floor = stoi(command[2]);
		int dest_floor = stoi(command[3]);
//Populating the data structures
		pass_info[id] = arr_time;
		index_info[id] = index;
		passenger[index].id = id;
		passenger[index].arr_floor = arr_floor;	
		passenger[index].dest_floor = dest_floor;
		passenger[index].arr_time = arr_time;
//Determining the direction
		if((dest_floor - arr_floor)>0)  //Going up
		{
			passenger[index].up = true;
		}
		else  //Going down
		{
			passenger[index].up = false;
		}		
		index++;
	}//End of for	
//Actual simulation for a day starts
        for(int i=0;i<=86400;i++)
	{
        	// Deciding which passenger/s arrived at current time
		// And Updating the already existing ones
                vector<int> current;  //Stores the currently waiting passenger ID's
                for(auto it=index_info.begin();it!=index_info.end();it++) //Iterating over all the passengers
		 {
			//Here it->second points to the passenger's ID
                        if(passenger[it->second].wait)  //If passenger is waiting
                        {
                                current.push_back(it->second);  //Add him to currently waiting passengers list
                        }
                        if(passenger[it->second].arr_time == i) //Just arrived at this time
                        {
                                passenger[it->second].wait = true; //Update wait to be equal to true
                                current.push_back(it->second); //Add him to currently waiting passengers list
                        }
                }// End of for
                int waiting_passengers = current.size(); //Currently waiting passengers
                int j=0;
                while(j<k) // A loop on all elevators
                {
    //Update values of passengers in this elevator. If anybody gets dropped off, do adjustments accordingly
			if(!elevator[j].is_stopped) //Elevator is in motion
			{
				elevator[j].timer++;
				if(elevator[j].up) //Going up
				{
					if(elevator[j].is_first) // needs 3 secs
                                        {
						if(elevator[j].timer<3) //In between floors
						{
							//Update values of all passengers it is carrying
							for(int x=0;x<elevator[j].carrying.size();x++) //Loop over currently carrying passengers
							{
								passenger[elevator[j].carrying[x]].in_elevator_time++;
							}
							file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" goes up"<<endl;
							stringstream ss;
							ss<<i<<" "<<elevator[j].current_floor<<" goes up";
							s_e[j].push_back(ss.str());
							j++;
							continue;		
						} // In between elevator check done
					//Timer >= 3, which means elevator reached at some floor, Update necessary values
						elevator[j].timer = 0;
						elevator[j].is_first = false;
						elevator[j].current_floor++;
						elevator[j].floors_travelled++;
						int flag = 0;
					//Loop over currently carrying passengers and see if anyone wants to get dropped  at current floor
                                                for(int x=0;x<elevator[j].carrying.size();x++)
                                                {
                                                	if(passenger[elevator[j].carrying[x]].dest_floor == elevator[j].current_floor) // Drop off
							{
								finished++;
								flag = 1;
								file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" drops off passenger "<<passenger[elevator[j].carrying[x]].id<<endl;
								stringstream ss;
								ss<<i<<" "<<elevator[j].current_floor<<" drops off passenger "<<passenger[elevator[j].carrying[x]].id;
								s_e[j].push_back(ss.str());
								passenger[elevator[j].carrying[x]].off_time = i;
								elevator[j].carrying.erase(elevator[j].carrying.begin()+x); //Remove from list
								elevator[j].capacity++;
								x--;
								elevator[j].is_stopped = true;
								elevator[j].is_first = true;
								elevator[j].stop_time = 0;
							}
                                                } // Passengers in the elevator processed
                                             //check to see if anyone wants to hop in
                                                for(int x=0;x<current.size();x++) //Currently waiting passengers
                                                {
						//Anyone at same floor and elevator has capacity
                                                	if((passenger[current[x]].arr_floor == elevator[j].current_floor) && elevator[j].capacity>0)
                                                        {
								//Pasasenger getting in
								if(elevator[j].any) //Can go in any direction, i.e not carrying any passengers
								{
									flag = 1;
									file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
									stringstream ss;
									ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
									s_e[j].push_back(ss.str());
									passenger[current[x]].in_time = i;
									passenger[current[x]].served_by = j;
									elevator[j].served++;
									elevator[j].any = false;
						                  //Change elevator's direction to that of passenger's direction
									elevator[j].up = passenger[current[x]].up;
									passenger[current[x]].wait = false;
                                                                        elevator[j].capacity--;
									elevator[j].carrying.push_back(current[x]);
                                                                        elevator[j].is_stopped = true;
                                                                        elevator[j].is_first = true;
									elevator[j].stop_time = 0;
									current.erase(current.begin()+x);
									x--;
                                                                }//End of any check
							//Some passengers already inside the elevator,
							//If current passenger's direction maches the elevator's direction, take him in
                                                                else if(passenger[current[x]].up == elevator[j].up) //Same direction
                                                                 {	
									flag = 1;								
									file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
									stringstream ss;
                                                                        ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
									s_e[j].push_back(ss.str());
									passenger[current[x]].in_time = i;
									passenger[current[x]].served_by = j;
									//Hop in
									passenger[current[x]].wait = false;
                                                                        elevator[j].capacity--;
									elevator[j].served++;
                                                                        elevator[j].carrying.push_back(current[x]);
                                                                        elevator[j].is_stopped = true;
                                                                        elevator[j].is_first = true;
                                                                        elevator[j].stop_time = 0;
                                                                        current.erase(current.begin()+x);
									x--;
                                                                 }
                                                          } //Passenger_in check done
                                                 } // Currently waiting passengers processed
				//If elevator is still empty, serve the closest one
						if(elevator[j].capacity == 10 && current.size()!=0)  //No passengers inside
						{
							int distance = INT_MAX;
                                                	int passenger_index;
                                                    //Loop through waiting passengers
                                                	for(int x=0;x<waiting_passengers;x++)
                                                	{
                                                        	if((abs(elevator[j].current_floor - passenger[current[x]].arr_floor))<distance)
                                                        	{
                                                                	distance = abs(elevator[j].current_floor - passenger[current[x]].arr_floor);
                                                                	passenger_index = current[x];
                                                       	 	}
                                                	}
							if(waiting_passengers>0)
							{
                                                		distance = elevator[j].current_floor - passenger[passenger_index].arr_floor;
                                                		if(distance>0) // Go Down
                                                		{
									if(flag == 0)
									{
										stringstream ss;
										ss<<i<<" "<<elevator[j].current_floor<<" goes down";
										s_e[j].push_back(ss.str());
									}
                                                     			elevator[j].up = false;
                                                		}
                                                		else //Go Up
                                                		{
									if(flag == 0)
									{
										stringstream ss;
                                                                        	ss<<i<<" "<<elevator[j].current_floor<<" goes up";
                                                                        	s_e[j].push_back(ss.str());
									}
                                                       			elevator[j].up = true;
                                                		}
							}
							for(int x=0;x<elevator[j].carrying.size();x++) //Loop over currently carrying passengers
                                                        {
                                                                passenger[elevator[j].carrying[x]].in_elevator_time++;
                                                        }
                                                } // Floor check done. Either stop or move */
						else if(elevator[j].capacity == 10 && current.size()==0) // No passenger waiting
												//Pick up the one who is arriving soon or is close
						{
							int time1 = INT_MAX; //Finding the next arriving passenger
							int distance1 = INT_MAX; //Finding the closest passenger
							int time2 = INT_MAX; //Finding the next arriving passenger
                                                        int distance2 = INT_MAX; //Finding the closest passenger
							int index1;
							int index2;
							int final;
							bool no_one_left = true;
							//Loop over all passengers to find the one who's coming next
							for(int p=0;p<count;p++)
							{
								if(passenger[p].served_by == -1)//Not yet served, not yet arrived
								{
									no_one_left = false;
									//Shortest Distance
									if((abs(elevator[j].current_floor - passenger[p].arr_floor))<distance1)
                                                                	{
                                                                        	distance1 = abs(elevator[j].current_floor - passenger[p].arr_floor);
                                                                        	index1 = p;
										time1 = passenger[p].arr_time;
                                                                	}
									//Shortest Time
									if(passenger[p].arr_time<time2)
									{
										time2 = passenger[p].arr_time;
										index2 = p;
										distance2 = abs(elevator[j].current_floor - passenger[p].arr_floor);
									}
								}
							} //End of for
							if(no_one_left)//No one is left to come, stop this elevator forever
							{
								elevator[j].is_stopped = true;
								elevator[j].dont_move = true;
							}
							else //Someone's left
							{
								//Find a passenger who will be most optimal
								if(abs((2*distance1) - (passenger[index1].arr_time - i)) < abs((2*distance2) - (passenger[index2].arr_time - i)))
								{
									//Pick passenger 1
									if(elevator[j].current_floor - passenger[index1].arr_floor < 0)
									{
										//Go Up
										elevator[j].up = true;
									}
									else if(elevator[j].current_floor - passenger[index1].arr_floor > 0)
									{
										//Go Down
										elevator[j].up = false;
									}
									else //Passenger will come at same floor
									{
										elevator[j].is_stopped = true;
										elevator[j].stay = true;
									}
								}
								else
								{
									//Pick passenger 2
									if(elevator[j].current_floor - passenger[index2].arr_floor < 0)
                                                                        {
                                                                                //Go Up
                                                                                elevator[j].up = true;
                                                                        }
                                                                        else if(elevator[j].current_floor - passenger[index2].arr_floor > 0)
                                                                        {
                                                                                //Go Down
                                                                                elevator[j].up = false;
                                                                        }
                                                                        else //Passenger will come at same floor
                                                                        {
                                                                                elevator[j].is_stopped = true;
                                                                                elevator[j].stay = true;
                                                                        }
								}
								if(flag == 0)
                                                                {
									if(elevator[j].up)
									{
                                                                		stringstream ss;
                                                                        	ss<<i<<" "<<elevator[j].current_floor<<" goes up";
                                                                        	s_e[j].push_back(ss.str());
									}
									else
									{
										stringstream ss;
                                                                                ss<<i<<" "<<elevator[j].current_floor<<" goes down";
                                                                                s_e[j].push_back(ss.str());
									}
                                                                }
							}
						}
                                        } // 3seconds check done
                                        else // needs 2 secs
                                        {
                                        	if(elevator[j].timer<2) // in between floors
                                                {
                                                        //Update values of all passengers it is carrying
                                                        for(int x=0;x<elevator[j].carrying.size();x++) //Loop over currently carrying passengers
                                                        {
                                                                passenger[elevator[j].carrying[x]].in_elevator_time++;
                                                        }
							file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" goes up"<<endl;
							stringstream ss;
							ss<<i<<" "<<elevator[j].current_floor<<" goes up";
							s_e[j].push_back(ss.str());
                                                        j++;
                                                        continue;
                                                } // In between elevator check done
					 //Timer >= 2, which means elevator reached at some floor, Update necessary values
                                                elevator[j].timer = 0;
                                                elevator[j].is_first = false;
                                                elevator[j].current_floor++;
                                                elevator[j].floors_travelled++;
						int flag = 0;
                                        //Loop over currently carrying passengers and see if anyone wants to get dropped  at current floor
                                                for(int x=0;x<elevator[j].carrying.size();x++)
                                                {
                                                        if(passenger[elevator[j].carrying[x]].dest_floor == elevator[j].current_floor) // Drop off
                                                        {
								finished++;
								flag = 1;
								file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" drops off passenger "<<passenger[elevator[j].carrying[x]].id<<endl;
								stringstream ss;
								ss<<i<<" "<<elevator[j].current_floor<<" drops off passenger "<<passenger[elevator[j].carrying[x]].id;
								s_e[j].push_back(ss.str());
								passenger[elevator[j].carrying[x]].off_time = i;
                                                                elevator[j].carrying.erase(elevator[j].carrying.begin()+x); //Remove from list
								elevator[j].capacity++;
                                                                elevator[j].is_stopped = true;
                                                                elevator[j].is_first = true;
                                                                elevator[j].stop_time = 0;
								x--;
                                                        }
                                                } // Passengers in the elevator processed
                                             //check to see if anyone wants to hop in
                                                for(int x=0;x<current.size();x++) //Currently waiting passengers
                                                {
                                                //Anyone at same floor and elevator has capacity
                                                        if((passenger[current[x]].arr_floor == elevator[j].current_floor) && elevator[j].capacity>0)
                                                        {
                                                                //Pasasenger getting in
                                                                if(elevator[j].any) //Can go in any direction, i.e not carrying any passengers
                                                                {
									flag = 1;
									file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
									stringstream ss;
									ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
									s_e[j].push_back(ss.str());
									passenger[current[x]].in_time = i;
									passenger[current[x]].served_by = j;
                                                                        elevator[j].any = false;
									elevator[j].served++;
                                                                  //Change elevator's direction to that of passenger's direction
                                                                        elevator[j].up = passenger[current[x]].up;
                                                                        passenger[current[x]].wait = false;
                                                                        elevator[j].capacity--;
                                                                        elevator[j].carrying.push_back(current[x]);
                                                                        elevator[j].is_stopped = true;
                                                                        elevator[j].is_first = true;
                                                                        elevator[j].stop_time = 0;
                                                                        current.erase(current.begin()+x);
									x--;
                                                                }//End of any check
                                                        //Some passengers already inside the elevator,
                                                        //If current passenger's direction maches the elevator's direction, take him in
                                                                else if(passenger[current[x]].up == elevator[j].up) //Same direction
                                                                 {
									flag = 1;
									file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
									stringstream ss;
									ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
									s_e[j].push_back(ss.str());
									passenger[current[x]].in_time = i;
									passenger[current[x]].served_by = j;
                                                                        //Hop in
                                                                        passenger[current[x]].wait = false;
                                                                        elevator[j].capacity--;
									elevator[j].served++;
                                                                        elevator[j].carrying.push_back(current[x]);
                                                                        elevator[j].is_stopped = true;
                                                                        elevator[j].is_first = true;
                                                                        elevator[j].stop_time = 0;
                                                                        current.erase(current.begin()+x);
									x--;
                                                                 }
                                                          } //Passenger_in check done
                                              	} // Currently waiting passengers processed
                                //If elevator is still empty, serve the closest one
                                                 if(elevator[j].capacity == 10 && current.size()!=0)  //No passengers inside
                                                 {
                                                        int distance = INT_MAX;
                                                        int passenger_index;
                                                    //Loop through waiting passengers
                                                        for(int x=0;x<waiting_passengers;x++)
                                                        {
                                                                if((abs(elevator[j].current_floor - passenger[current[x]].arr_floor))<distance)
                                                                {
                                                                        distance = abs(elevator[j].current_floor - passenger[current[x]].arr_floor);
                                                                        passenger_index = current[x];
                                                                }
                                                        }
                                                        if(waiting_passengers>0)
                                                        {
                                                                distance = elevator[j].current_floor - passenger[passenger_index].arr_floor;
                                                                if(distance>0) // Go Down
                                                                {
									if(flag == 0)
									{
										stringstream ss;
                                                                        	ss<<i<<" "<<elevator[j].current_floor<<" goes down";
                                                                        	s_e[j].push_back(ss.str());
									}
                                                                        elevator[j].up = false;
                                                                }
                                                                else //Go Up
                                                                {
									if(flag == 0)
									{
										stringstream ss;
                                                                        	ss<<i<<" "<<elevator[j].current_floor<<" goes up";
                                                                        	s_e[j].push_back(ss.str());
									}
                                                                        elevator[j].up = true;
                                                                }
                                                        }
							for(int x=0;x<elevator[j].carrying.size();x++) //Loop over currently carrying passengers
                                                        {
                                                                passenger[elevator[j].carrying[x]].in_elevator_time++;
                                                        }
                                                } // Floor check done. Either stop or move 
						else if(elevator[j].capacity == 10 && current.size()==0) // No passenger waiting
                                                                                                //Pick up the one who is arriving soon or is close
                                                {
                                                        int time1 = INT_MAX; //Finding the next arriving passenger
                                                        int distance1 = INT_MAX; //Finding the closest passenger
                                                        int time2 = INT_MAX; //Finding the next arriving passenger
                                                        int distance2 = INT_MAX; //Finding the closest passenger
                                                        int index1;
                                                        int index2;
                                                        int final;
                                                        bool no_one_left = true;
                                                        //Loop over all passengers to find the one who's coming next
                                                        for(int p=0;p<count;p++)
                                                        {
                                                                if(passenger[p].served_by == -1)//Not yet served, not yet arrived
                                                                {
                                                                        no_one_left = false;
                                                                        //Shortest Distance
                                                                        if((abs(elevator[j].current_floor - passenger[p].arr_floor))<distance1)
                                                                        {
                                                                                distance1 = abs(elevator[j].current_floor - passenger[p].arr_floor);
                                                                                index1 = p;
                                                                                time1 = passenger[p].arr_time;
                                                                        }
                                                                        //Shortest Time
                                                                        if(passenger[p].arr_time<time2)
                                                                        {
                                                                                time2 = passenger[p].arr_time;
                                                                                index2 = p;
                                                                                distance2 = abs(elevator[j].current_floor - passenger[p].arr_floor);
                                                                        }
								}
							}//End of for
							if(no_one_left)//No one is left to come, stop this elevator forever
                                                        {
                                                                elevator[j].is_stopped = true;
                                                                elevator[j].dont_move = true;
                                                        }
                                                        else //Someone's left
                                                        {
                                                                //Find a passenger who will be most optimal
                                                                if(abs((2*distance1) - (passenger[index1].arr_time - i)) < abs((2*distance2) - (passenger[index2].arr_time - i)))
                                                                {
                                                                        //Pick passenger 1
                                                                        if(elevator[j].current_floor - passenger[index1].arr_floor < 0)
                                                                        {
                                                                                //Go Up
                                                                                elevator[j].up = true;
                                                                        }
                                                                        else if(elevator[j].current_floor - passenger[index1].arr_floor > 0)
                                                                        {
                                                                                //Go Down
                                                                                elevator[j].up = false;
                                                                        }
                                                                        else //Passenger will come at same floor
                                                                        {
                                                                                elevator[j].is_stopped = true;
                                                                                elevator[j].stay = true;
                                                                        }
                                                                }
                                                                else
                                                                {
                                                                        //Pick passenger 2
                                                                        if(elevator[j].current_floor - passenger[index2].arr_floor < 0)
                                                                        {
                                                                                //Go Up
                                                                                elevator[j].up = true;
                                                                        }
                                                                        else if(elevator[j].current_floor - passenger[index2].arr_floor > 0)
                                                                        {
                                                                                //Go Down
                                                                                elevator[j].up = false;
                                                                        }
                                                                        else //Passenger will come at same floor
                                                                        {
                                                                                elevator[j].is_stopped = true;
                                                                                elevator[j].stay = true;
                                                                        }
                                                                }
                                                                if(flag == 0)
                                                                {
                                                                        if(elevator[j].up)
                                                                        {
                                                                                stringstream ss;
                                                                                ss<<i<<" "<<elevator[j].current_floor<<" goes up";
                                                                                s_e[j].push_back(ss.str());
                                                                        }
                                                                        else
                                                                        {
                                                                                stringstream ss;
                                                                                ss<<i<<" "<<elevator[j].current_floor<<" goes down";
										s_e[j].push_back(ss.str());
                                                                        }
                                                                }
                                                        }
                                                }
                                        } // 2 seconds check done
				} // going up processed
				else // going down
				{
					if(elevator[j].is_first) // needs 2 secs
                                        {
                                                if(elevator[j].timer<2) // in between floors
                                                {
                                                        //Update values of all passengers it is carrying
                                                        for(int x=0;x<elevator[j].carrying.size();x++) //Loop over currently carrying passengers
                                                        {
                                                                passenger[elevator[j].carrying[x]].in_elevator_time++;
                                                        }
							file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" goes down"<<endl;
							stringstream ss;
							ss<<i<<" "<<elevator[j].current_floor<<" goes down";
							s_e[j].push_back(ss.str());
                                                        j++;
                                                        continue;
                                                } // In between elevator check done 
                                         //Timer >= 2, which means elevator reached at some floor, Update necessary values
                                                elevator[j].timer = 0;
                                                elevator[j].is_first = false;
                                                elevator[j].current_floor--;
                                                elevator[j].floors_travelled++;
						int flag = 0;
                                        //Loop over currently carrying passengers and see if anyone wants to get dropped  at current floor
                                                for(int x=0;x<elevator[j].carrying.size();x++)
                                                {
                                                        if(passenger[elevator[j].carrying[x]].dest_floor == elevator[j].current_floor) // Drop off
                                                        {
								finished++;
								flag = 1;
								file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" drops off passenger "<<passenger[elevator[j].carrying[x]].id<<endl;
								stringstream ss;
								ss<<i<<" "<<elevator[j].current_floor<<" drops off passenger "<<passenger[elevator[j].carrying[x]].id;
								s_e[j].push_back(ss.str());
								passenger[elevator[j].carrying[x]].off_time = i;
                                                                elevator[j].carrying.erase(elevator[j].carrying.begin()+x); //Remove from list
								elevator[j].capacity++;
                                                                elevator[j].is_stopped = true;
                                                                elevator[j].is_first = true;
                                                                elevator[j].stop_time = 0;
								x--;
                                                        }
                                                } // Passengers in the elevator processed
                                             //check to see if anyone wants to hop in
                                                for(int x=0;x<current.size();x++) //Currently waiting passengers
                                                {
                                                //Anyone at same floor and elevator has capacity
                                                        if((passenger[current[x]].arr_floor == elevator[j].current_floor) && elevator[j].capacity>0)
                                                        {
                                                                //Pasasenger getting in
                                                                if(elevator[j].any) //Can go in any direction, i.e not carrying any passengers
                                                                {
									flag = 1;
									file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up  passenger "<<passenger[current[x]].id<<endl;
									stringstream ss;
									ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
									s_e[j].push_back(ss.str());
									passenger[current[x]].in_time = i;
									passenger[current[x]].served_by = j;
                                                                        elevator[j].any = false;
									elevator[j].served++;
                                                                  //Change elevator's direction to that of passenger's direction
                                                                        elevator[j].up = passenger[current[x]].up;
                                                                        passenger[current[x]].wait = false;
                                                                        elevator[j].capacity--;
                                                                        elevator[j].carrying.push_back(current[x]);
                                                                        elevator[j].is_stopped = true;
                                                                        elevator[j].is_first = true;
                                                                        elevator[j].stop_time = 0;
                                                                        current.erase(current.begin()+x);
									x--;
                                                                }//End of any check
                                                        //Some passengers already inside the elevator,
                                                        //If current passenger's direction maches the elevator's direction, take him in
                                                                 else if(passenger[current[x]].up == elevator[j].up) //Same direction
                                                                 {
									flag = 1;
									file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
									stringstream ss;
									ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
									s_e[j].push_back(ss.str());
									passenger[current[x]].in_time = i;
									passenger[current[x]].served_by = j;
                                                                        //Hop in
                                                                        passenger[current[x]].wait = false;
                                                                        elevator[j].capacity--;
									elevator[j].served++;
                                                                        elevator[j].carrying.push_back(current[x]);
                                                                        elevator[j].is_stopped = true;
                                                                        elevator[j].is_first = true;
                                                                        elevator[j].stop_time = 0;
                                                                        current.erase(current.begin()+x);
									x--;
                                                                 }
                                                          } //Passenger_in check done
                                                 } // Currently waiting passengers processed
                                //If elevator is still empty, serve the closest one
                                                  if(elevator[j].capacity == 10 && current.size()!=0)  //No passengers inside
                                                  {
                                                        int distance = INT_MAX;
                                                        int passenger_index;
                                                    //Loop through waiting passengers
                                                        for(int x=0;x<waiting_passengers;x++)
                                                        {
                                                                if((abs(elevator[j].current_floor - passenger[current[x]].arr_floor))<distance)
                                                                {
                                                                        distance = abs(elevator[j].current_floor - passenger[current[x]].arr_floor);
                                                                        passenger_index = current[x];
                                                                }
                                                        }
                                                        if(waiting_passengers>0)
                                                        {
                                                                distance = elevator[j].current_floor - passenger[passenger_index].arr_floor;
                                                                if(distance>0) // Go Down
                                                                {
									if(flag == 0)
									{
										stringstream ss;
                                                                        	ss<<i<<" "<<elevator[j].current_floor<<" goes down";
                                                                        	s_e[j].push_back(ss.str());
									}
                                                                        elevator[j].up = false;
                                                                }
                                                                else //Go Up
                                                                {
									if(flag == 0)
									{
										stringstream ss;
                                                                        	ss<<i<<" "<<elevator[j].current_floor<<" goes up";
                                                                        	s_e[j].push_back(ss.str());
									}
                                                                        elevator[j].up = true;
                                                                }
                                                        }
							for(int x=0;x<elevator[j].carrying.size();x++) //Loop over currently carrying passengers
                                                        {
                                                                passenger[elevator[j].carrying[x]].in_elevator_time++;
                                                        }
						 } // Floor check done. Either stop or move
						  else if(elevator[j].capacity == 10 && current.size()==0) // No passenger waiting
                                                                                                //Pick up the one who is arriving soon or is close
                                                  {
                                                        int time1 = INT_MAX; //Finding the next arriving passenger
                                                        int distance1 = INT_MAX; //Finding the closest passenger
                                                        int time2 = INT_MAX; //Finding the next arriving passenger
                                                        int distance2 = INT_MAX; //Finding the closest passenger
                                                        int index1;
                                                        int index2;
                                                        int final;
                                                        bool no_one_left = true;
                                                        //Loop over all passengers to find the one who's coming next
                                                        for(int p=0;p<count;p++)
                                                        {
                                                                if(passenger[p].served_by == -1)//Not yet served, not yet arrived
                                                                {
                                                                        no_one_left = false;
                                                                        //Shortest Distance
                                                                        if((abs(elevator[j].current_floor - passenger[p].arr_floor))<distance1)
                                                                        {
                                                                                distance1 = abs(elevator[j].current_floor - passenger[p].arr_floor);
                                                                                index1 = p;
                                                                                time1 = passenger[p].arr_time;
                                                                        }
									//Shortest Time
                                                                        if(passenger[p].arr_time<time2)
                                                                        {
                                                                                time2 = passenger[p].arr_time;
                                                                                index2 = p;
                                                                                distance2 = abs(elevator[j].current_floor - passenger[p].arr_floor);
                                                                        }
                                                                }
                                                        } //End of for
                                                        if(no_one_left)//No one is left to come, stop this elevator forever
                                                        {
                                                                elevator[j].is_stopped = true;
                                                                elevator[j].dont_move = true;
                                                        }
							else //Someone's left
                                                        {
                                                                //Find a passenger who will be most optimal
                                                                if(abs((2*distance1) - (passenger[index1].arr_time - i)) < abs((2*distance2) - (passenger[index2].arr_time - i)))
                                                                {
                                                                        //Pick passenger 1
                                                                        if(elevator[j].current_floor - passenger[index1].arr_floor < 0)
                                                                        {
                                                                                //Go Up
                                                                                elevator[j].up = true;
                                                                        }
                                                                        else if(elevator[j].current_floor - passenger[index1].arr_floor > 0)
                                                                        {
                                                                                //Go Down
                                                                                elevator[j].up = false;
                                                                        }
                                                                        else //Passenger will come at same floor
                                                                        {
                                                                                elevator[j].is_stopped = true;
                                                                                elevator[j].stay = true;
                                                                        }
                                                                }
                                                                else
                                                                {
                                                                        //Pick passenger 2
                                                                        if(elevator[j].current_floor - passenger[index2].arr_floor < 0)
                                                                        {
                                                                                //Go Up
                                                                                elevator[j].up = true;
                                                                        }
                                                                        else if(elevator[j].current_floor - passenger[index2].arr_floor > 0)
                                                                        {
                                                                                //Go Down
                                                                                elevator[j].up = false;
                                                                        }
                                                                        else //Passenger will come at same floor
                                                                        {
                                                                                elevator[j].is_stopped = true;
                                                                                elevator[j].stay = true;
                                                                        }
                                                                }
                                                                if(flag == 0)
                                                                {
                                                                        if(elevator[j].up)
                                                                        {
                                                                                stringstream ss;
                                                                                ss<<i<<" "<<elevator[j].current_floor<<" goes up";
                                                                                s_e[j].push_back(ss.str());
                                                                        }
									else
                                                                        {
                                                                                stringstream ss;
                                                                                ss<<i<<" "<<elevator[j].current_floor<<" goes down";
                                                                                s_e[j].push_back(ss.str());
                                                                        }
                                                                }
                                                        }
                                                } 
                                        } // is_first check done
                                        else // needs 1 second
                                        {
                                                if(elevator[j].timer<1) // in between floors
                                                {
                                                	//Update values of all passengers it is carrying
                                                        for(int x=0;x<elevator[j].carrying.size();x++) //Loop over currently carrying passengers
                                                        {
                                                                passenger[elevator[j].carrying[x]].in_elevator_time++;
                                                        }
							file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" goes down"<<endl;
							stringstream ss;
							ss<<i<<" "<<elevator[j].current_floor<<" goes down";
							s_e[j].push_back(ss.str());
                                                        j++;
                                                        continue;
                                                } // In between elevator check done
                                         //Timer >= 1, which means elevator reached at some floor, Update necessary values        
                                                elevator[j].timer = 0;
                                                elevator[j].is_first = false;
                                                elevator[j].current_floor--;
                                                elevator[j].floors_travelled++;
						int flag = 0;
                                        //Loop over currently carrying passengers and see if anyone wants to get dropped  at current floor
                                                for(int x=0;x<elevator[j].carrying.size();x++)
                                                {
                                                        if(passenger[elevator[j].carrying[x]].dest_floor == elevator[j].current_floor) // Drop off
                                                        {
								finished++;
								flag = 1;
								file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" drops off passenger "<<passenger[elevator[j].carrying[x]].id<<endl;
								stringstream ss;
								ss<<i<<" "<<elevator[j].current_floor<<" drops off passenger "<<passenger[elevator[j].carrying[x]].id;
								s_e[j].push_back(ss.str());
								passenger[elevator[j].carrying[x]].off_time = i;
                                                                elevator[j].carrying.erase(elevator[j].carrying.begin()+x); //Remove from list
								elevator[j].capacity++;
                                                                elevator[j].is_stopped = true;
                                                                elevator[j].is_first = true;
                                                                elevator[j].stop_time = 0;
								x--;
                                                        }
                                                } // Passengers in the elevator processed
                                             //check to see if anyone wants to hop in
                                                for(int x=0;x<current.size();x++) //Currently waiting passengers
                                                {
                                                //Anyone at same floor and elevator has capacity
                                                        if((passenger[current[x]].arr_floor == elevator[j].current_floor) && elevator[j].capacity>0)
                                                        {
                                                                //Pasasenger getting in
                                                                if(elevator[j].any) //Can go in any direction, i.e not carrying any passengers
                                                                {
									flag = 1;
									file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
									stringstream ss;
									ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
									s_e[j].push_back(ss.str());
									passenger[current[x]].in_time = i;
									passenger[current[x]].served_by = j;
                                                                        elevator[j].any = false;
									elevator[j].served++;
                                                                  //Change elevator's direction to that of passenger's direction
                                                                        elevator[j].up = passenger[current[x]].up;
                                                                        passenger[current[x]].wait = false;
                                                                        elevator[j].capacity--;
                                                                        elevator[j].carrying.push_back(current[x]);
                                                                        elevator[j].is_stopped = true;
                                                                        elevator[j].is_first = true;
                                                                        elevator[j].stop_time = 0;
                                                                        current.erase(current.begin()+x);
									x--;
                                                                }//End of any check
                                                        //Some passengers already inside the elevator,
                                                        //If current passenger's direction maches the elevator's direction, take him in
                                                                else if(passenger[current[x]].up == elevator[j].up) //Same direction
                                                                 {
									flag = 1;
									file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
									stringstream ss;
									ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
									s_e[j].push_back(ss.str());
									passenger[current[x]].in_time = i;
									passenger[current[x]].served_by = j;
                                                                        //Hop in
                                                                        passenger[current[x]].wait = false;
                                                                        elevator[j].capacity--;
									elevator[j].served++;
                                                                        elevator[j].carrying.push_back(current[x]);
                                                                        elevator[j].is_stopped = true;
                                                                        elevator[j].is_first = true;
                                                                        elevator[j].stop_time = 0;
                                                                        current.erase(current.begin()+x);
									x--;
                                                                 }
                                                          } //Passenger_in check done
                                                } // Currently waiting passengers processed
                                //If elevator is still empty, serve the closest one
                                                 if(elevator[j].capacity == 10 && current.size()!=0)  //No passengers inside
                                                 {
                                                        int distance = INT_MAX;
                                                        int passenger_index;
                                                    //Loop through waiting passengers
                                                        for(int x=0;x<waiting_passengers;x++)
                                                        {
                                                                if((abs(elevator[j].current_floor - passenger[current[x]].arr_floor))<distance)
                                                                {
                                                                        distance = abs(elevator[j].current_floor - passenger[current[x]].arr_floor);
                                                                        passenger_index = current[x];
                                                                }
                                                        }
                                                        if(waiting_passengers>0)
                                                        {
                                                                distance = elevator[j].current_floor - passenger[passenger_index].arr_floor;
                                                                if(distance>0) // Go Down
                                                                {
										if(flag == 0)
									{
										stringstream ss;
                                                                        	ss<<i<<" "<<elevator[j].current_floor<<" goes down";
                                                                        	s_e[j].push_back(ss.str());
									}
                                                                        elevator[j].up = false;
                                                                }
                                                                else //Go Up
                                                                {
									if(flag == 0)
									{
										stringstream ss;
                                                                        	ss<<i<<" "<<elevator[j].current_floor<<" goes up";
                                                                        	s_e[j].push_back(ss.str());
									}
                                                                        elevator[j].up = true;
                                                                }
                                                        }
							for(int x=0;x<elevator[j].carrying.size();x++) //Loop over currently carrying passengers
                                                        {
                                                                passenger[elevator[j].carrying[x]].in_elevator_time++;
                                                        }
                                                } // Floor check done. Either stop or move
						 else if(elevator[j].capacity == 10 && current.size()==0) // No passenger waiting
                                                                                                //Pick up the one who is arriving soon or is close
                                                {
                                                        int time1 = INT_MAX; //Finding the next arriving passenger
                                                        int distance1 = INT_MAX; //Finding the closest passenger
                                                        int time2 = INT_MAX; //Finding the next arriving passenger
                                                        int distance2 = INT_MAX; //Finding the closest passenger
                                                        int index1;
                                                        int index2;
                                                        int final;
                                                        bool no_one_left = true;
                                                        //Loop over all passengers to find the one who's coming next
                                                        for(int p=0;p<count;p++)
                                                        {
                                                                if(passenger[p].served_by == -1)//Not yet served, not yet arrived
                                                                {
                                                                        no_one_left = false;
                                                                        //Shortest Distance
                                                                        if((abs(elevator[j].current_floor - passenger[p].arr_floor))<distance1)
                                                                        {
                                                                                distance1 = abs(elevator[j].current_floor - passenger[p].arr_floor);
                                                                                index1 = p;
                                                                                time1 = passenger[p].arr_time;
                                                                        }
									//Shortest Time
                                                                        if(passenger[p].arr_time<time2)
                                                                        {
                                                                                time2 = passenger[p].arr_time;
                                                                                index2 = p;
                                                                                distance2 = abs(elevator[j].current_floor - passenger[p].arr_floor);
                                                                        }
                                                                }
                                                        } //End of for
                                                        if(no_one_left)//No one is left to come, stop this elevator forever
                                                        {
                                                                elevator[j].is_stopped = true;
                                                                elevator[j].dont_move = true;
                                                        }
                                                        else //Someone's left
                                                        {
                                                                //Find a passenger who will be most optimal
                                                                if(abs((2*distance1) - (passenger[index1].arr_time - i)) < abs((2*distance2) - (passenger[index1].arr_time - i)))
                                                                {
                                                                        //Pick passenger 1
                                                                        if(elevator[j].current_floor - passenger[index1].arr_floor < 0)
                                                                        {
                                                                                //Go Up
                                                                                elevator[j].up = true;
                                                                        }
                                                                        else if(elevator[j].current_floor - passenger[index1].arr_floor > 0)
                                                                        {
                                                                                //Go Down
                                                                                elevator[j].up = false;
                                                                        }
                                                                        else //Passenger will come at same floor
                                                                        {
                                                                                elevator[j].is_stopped = true;
                                                                                elevator[j].stay = true;
                                                                        }
                                                                }
                                                                else
                                                                {
                                                                        //Pick passenger 2
                                                                        if(elevator[j].current_floor - passenger[index2].arr_floor < 0)
                                                                        {
                                                                                //Go Up
                                                                                elevator[j].up = true;
                                                                        }
                                                                        else if(elevator[j].current_floor - passenger[index2].arr_floor > 0)
                                                                        {
                                                                                //Go Down
                                                                                elevator[j].up = false;
                                                                        }
                                                                        else //Passenger will come at same floor
                                                                        {
                                                                                elevator[j].is_stopped = true;
                                                                                elevator[j].stay = true;
                                                                        }
                                                                }
								if(flag == 0)
                                                                {
                                                                        if(elevator[j].up)
                                                                        {
                                                                                stringstream ss;
                                                                                ss<<i<<" "<<elevator[j].current_floor<<" goes up";
                                                                                s_e[j].push_back(ss.str());
                                                                        }
                                                                        else
                                                                        {
                                                                                stringstream ss;
                                                                                ss<<i<<" "<<elevator[j].current_floor<<" goes down";
                                                                                s_e[j].push_back(ss.str());
                                                                        }
                                                                }
                                                        }
                                                } 
                                        } // 1 second check done
                                } // going down processed
			} // in motion_check done	
			else // already stopped
			{
				//Update values
				if(elevator[j].dont_move)
				{
					j++;
					continue;
				}
				if(elevator[j].stay) //Wait for passenger
				{
					for(int x=0;x<current.size();x++)
					{
						if(passenger[current[x]].arr_floor == elevator[j].current_floor)
						{
							//Pick up
							file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
                                                        stringstream ss;
                                                        ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
                                                        s_e[j].push_back(ss.str());
                                                        passenger[current[x]].in_time = i;
                                                        passenger[current[x]].served_by = j;
                                                        elevator[j].any = false;
                                                        elevator[j].served++;
                                                        //Change elevator's direction to that of passenger's direction
                                                        elevator[j].up = passenger[current[x]].up;
                                                        passenger[current[x]].wait = false;
                                                        elevator[j].capacity--;
                                                        elevator[j].carrying.push_back(current[x]);
                                                        elevator[j].is_stopped = true;
                                                        elevator[j].is_first = true;
                                                        elevator[j].stop_time = 0;
						        elevator[j].stay = false;
                                                        current.erase(current.begin()+x);
                                                        x--;
						}
					}
					stringstream ss;
                                        ss<<i<<" "<<elevator[j].current_floor<<" stops";
                                        s_e[j].push_back(ss.str());
					j++;
					continue;
				}
				elevator[j].timer = 0;
				elevator[j].is_first = true;
				elevator[j].stop_time++;
				int flag = 0;
                                if(elevator[j].stop_time<2) //Passenger still either going in or going out
                                {
					flag = 1;
					file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" stops"<<endl;
					stringstream ss;
					ss<<i<<" "<<elevator[j].current_floor<<" stops";
					s_e[j].push_back(ss.str());
					for(int x=0;x<elevator[j].carrying.size();x++) //Passengers already inside the elevator
					{
						//Updaate their values
						passenger[elevator[j].carrying[x]].in_elevator_time++;
					}
                                        for(int x=0;x<current.size();x++) //Passengers waiting to be served
                                        {
                                         	if((passenger[current[x]].arr_floor == elevator[j].current_floor) && elevator[j].capacity>0)
                                                {
                                                                //Pasasenger getting in
                                                	if(elevator[j].any) //Can go in any direction, i.e not carrying any passengers
                                                        {
								file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
								stringstream ss;
								ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
								s_e[j].push_back(ss.str());
								passenger[current[x]].in_time = i;
								passenger[current[x]].served_by = j;
                                                        	elevator[j].any = false;
								elevator[j].served++;
                                                                 //Change elevator's direction to that of passenger's direction
                                                                elevator[j].up = passenger[current[x]].up;
                                                                passenger[current[x]].wait = false;
                                                                elevator[j].capacity--;
                                                                elevator[j].carrying.push_back(current[x]);
                                                                elevator[j].is_stopped = true;
                                                                elevator[j].is_first = true;
                                                                elevator[j].stop_time = 0;
                                                                current.erase(current.begin()+x);
								x--;
                                                         }//End of any check
                                                        //Some passengers already inside the elevator,
                                                        //If current passenger's direction maches the elevator's direction, take him in
                                                         else if(passenger[current[x]].up == elevator[j].up) //Same direction
                                                         {
								file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id<<endl;
								stringstream ss;
								ss<<i<<" "<<elevator[j].current_floor<<" picked up passenger "<<passenger[current[x]].id;
								s_e[j].push_back(ss.str());
								passenger[current[x]].in_time = i;
								passenger[current[x]].served_by = j;
                                                         	//Hop in
                                                                passenger[current[x]].wait = false;
                                                                elevator[j].capacity--;
								elevator[j].served++;
                                                                elevator[j].carrying.push_back(current[x]);
                                                                elevator[j].is_stopped = true;
                                                                elevator[j].is_first = true;
                                                                elevator[j].stop_time = 0;
                                                                current.erase(current.begin()+x);
								x--;
                                                         }
                                                 } //Passenger_in check done 
                                         } // Currently waiting passengers processed
			 //If elevator is still empty, serve the closest one
                                         if(elevator[j].capacity == 10 && current.size()!=0)  //No passengers inside
                                         {
                                         	int distance = INT_MAX;
                                                int passenger_index;
                                                //Loop through waiting passengers
                                                for(int x=0;x<waiting_passengers;x++)
                                                {
                                                	if((abs(elevator[j].current_floor - passenger[current[x]].arr_floor))<distance)
                                                        {
                                                        	distance = abs(elevator[j].current_floor - passenger[current[x]].arr_floor);
                                                                passenger_index = current[x];
                                                        }
                                                }
						if(waiting_passengers>0)
                                                {
                                                        if(passenger[passenger_index].arr_floor<elevator[j].current_floor) // Go Down
                                                        {
                                                        	elevator[j].up = false;
                                                        }
                                                        else //Go Up
                                                        {
                                                        	elevator[j].up = true;
                                                        }
                                                }
                                         } //  Empty elevator check done
                                }//end of if
                                else // stop_timer >=2, means start moving
                                {
					if(elevator[j].capacity == 10 && current.size()==0) //Elevator still empty and no one waiting then don't move. Keep waiting
					{
						file1<<i<<" "<<j+1<<" "<<elevator[j].current_floor<<" No passengers to serve, so stopped"<<endl;
						elevator[j].stop_time = 0;
					}
					else //Some passengers are inside or are waiting
					{
						for(int x=0;x<elevator[j].carrying.size();x++) //Loop over currently carrying passengers
                                                {
                                                	passenger[elevator[j].carrying[x]].in_elevator_time++;
                                                }
                                		elevator[j].is_stopped = false;
						elevator[j].stop_time = 0;
						stringstream ss;
						if(elevator[j].up)
						{
							ss<<i<<" "<<elevator[j].current_floor<<" goes up";
						}
						else
						{
							ss<<i<<" "<<elevator[j].current_floor<<" goes down";
						}
						s_e[j].push_back(ss.str());
					}
                                }
			}// already stopped processing done */
			j++;
		}// End of while (all elevators processed)
		//If there are passengers still waiting in the queue, update their wait times
		for(int x=0;x<current.size();x++)
		{
			passenger[current[x]].wait_time++;
			passenger[current[x]].wait = true; //Still waiting
		}
		if(finished == count)
		{
			break;
		}
        } // End of for (Entire Simulation)
	//Making s_e.txt
	for(int i=0;i<k;i++)
	{
		file2<<"Elevator "<<i+1<<":"<<endl;
		for(int j=0;j<s_e[i].size();j++)
		{
			file2<<s_e[i][j]<<endl;
		}
		file2<<endl;
		file2<<"Total number of floors travelled : "<<elevator[i].floors_travelled<<endl;
		file2<<"Total number of passengers picked up : "<<elevator[i].served<<endl;
	}
	//Making s_p.txt
	for(int i=0;i<count;i++)
	{
		file3<<passenger[i].id<<" "<<passenger[i].served_by+1<<" "<<passenger[i].in_time<<" "<<passenger[i].off_time<<endl;
	}
	file3<<endl;
	vector<int> wait_times;
	vector<int> total_time;
	int sum = 0;
	int total_sum = 0;
	for(int i=0;i<count;i++)
	{
		sum += passenger[i].wait_time;
		wait_times.push_back(passenger[i].wait_time);
		total_time.push_back(passenger[i].off_time - passenger[i].arr_time);
		total_sum += passenger[i].off_time - passenger[i].arr_time;
	}
	avg_wait_time = sum/((float)count);
	avg_time_to_dest = total_sum/((float)count);
	sort(wait_times.begin(),wait_times.end());
	sort(total_time.begin(),total_time.end());
	max_wait_time = wait_times[count-1];
	max_time_to_dest = total_time[count-1];
	file3<<"Average Waiting Time : "<<avg_wait_time<<endl;
	file3<<"Maximal Waiting Time : "<<max_wait_time<<endl;
	file3<<"Average Time to Arrive Destination : "<<avg_time_to_dest<<endl;
	file3<<"Maximal Time to Arrive Destination : "<<max_time_to_dest<<endl;
	return 0;
}// End of main	
