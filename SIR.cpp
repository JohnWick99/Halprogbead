using namespace std;
#include <iostream> 
#include <list> 
#include <iterator> 
#include <vector>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <set>
#include <iterator>
#include <fstream>
#include <sstream>
#include <chrono>
#include <random>



//IMPORTANT: Parameters have to be changed manually: the line where they are:   population size: 33
                                                                              //size: 34
                                                                              //incubation time: 125
                                                                              //turns to die: 129
                                                                              //turns to become immune: 147
                                                                              //infection chance: 396
                                                                              //death rate:  397






// setting up the values for the grid 
const int N = 1000;  //population size
const int size = 64; //Length of edge of the square where agents can move, size^2 should be around 4x the size of N
int R_0; //infection chance 100 = 100%
int D_0; //death rate, 100 = every one dies who gets infected
int Recov_0;  //1-D_0, those who survive become immune

vector<pair<int,int>> data;  //global vector, makes life easier

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(0, 100); //found out rand() is pretty bad for iterations, so using a more complex rng


struct Point{   //for some reason the class wouldnt accept pair<int,int> so I had to improvise
        int x;
        int y;
    };
class Person {
        public:
        Point pos;   //position of agent
        int state;   //susceptible:1 infected:2 immune:3 dead:4
        bool cont;  //susceptible had contact with infected and got infected
        int  time;   //time scince getting infected, used for incubation period
        bool first;    //will be explained at line 98

    };

void get_data(int suminf,int dead){   //getting the data out of the iterations

data.push_back(make_pair(suminf,dead));
 
};

Person* Enviroment(){  //generates the agents, in first versions generated an array that can be made into a video by python

    

    Point points[N];
    std::uniform_int_distribution<int> S(1,size-1);
    for (int i = 0; i < N; i++)
    {
        points[i].x = S(mt);   //randomly generating the positions for the agents
        points[i].y = S(mt);
        
    }
    


    Person* people = new Person[N];   //making a class for the agents
    for (int i = 0; i < N; i++)
    {
        people[i].pos.x = points[i].x;
        people[i].pos.y = points[i].y;
        people[i].state = 1;
        people[i].cont = false;
        people[i].time = 0;
        people[i].first = false;


    }
    

    int k = rand() % N;
    people[k].state = 2;   //1 random human infected
    people[k].cont = true; //important for the code to work
    people[k].first = true;  //used to eliminate cases, where the zero patient dies resoulting in no meaningful data

    return people;
}


bool update(Person* people, std::map< pair<int,int>, vector<pair<int,int>> > pos_tocheck, bool is_any_infected){
  
    set <pair<int,int>> positions;
    set <pair<int,int>> inf_positions;  //sets of positions, makes it really fast to check if there is any agent at a set position

    vector<int> infected;     //basicly a list of infected agents

    
    int temp = 0;   //counts how many new infected there are in a turn
    vector <int> del_list; //this list will hold the index of agents who died, so they need removal
    for(int i = 0; i < N; i++){
    if (people[i].state != 4) //only checking for alive agents, not really needed as dead agents are removed, but its safer this way
    {

        positions.insert(make_pair(people[i].pos.x,people[i].pos.y)); //filling the positions set with the actual positions of the agents

        
        if (people[i].cont and people[i].state != 3){     //only checking for agents that have been infected, but not immune
              

            people[i].time += 1;    //advancing their time by 1, basicly turns scince infection
            if (people[i].time == 1){  //change this for incubation time. At 1 there is no incubation time, any higher and the incubation time will be that number - 1 in turns
                people[i].state = 2;  //change agent state to infected/infecting
                temp += 1;      
            }
            if (people[i].time == 7){  //the time agents may die, the chance is D_0

                if( dist(mt) < D_0 ){   
                    people[i].state = 4;  //bad luck, agent died
                } 
                else{
                    people[i].state = 2;  //good luck, agent lived
                }    

                if (people[i].state == 4 and people[i].first == false){   //this is where if the agent is the zero patient, he will never die, 
                    del_list.push_back(i);                                  //this eliminates trivial cases where the zero patient dies and ther is almost no infected (usually less than 10 for N = 1000)
                }                                                               //I would recomend switching it to false at line 88 for  D_0 = 100
                                                                                //as at that value the zero patient must die, not like at 99, where we can say he was very "lucky" to live
                else if (people[i].state == 4 and people[i].first == true){        //recap: if at line 88, people[k].first = true, he never dies, if it is false he will die
            
                    people[i].state = 2;
                }
            }
            if (people[i].time == 24){ //24 turns after getting infected agents who survive will become recovered and immune
                people[i].state = 3;
            }
        
            if (people[i].state == 2){     //after updating of states is done all infected/infecting is added to a set
                inf_positions.insert(make_pair(people[i].pos.x,people[i].pos.y));
            }
        }
    }
    }
    if  (inf_positions.size() == 0){ //if ther isn't any infected/infecting left this value is set to false, meaning the simulation stops as ther will be no new infection
        
        is_any_infected = false;
    }


    get_data(temp,del_list.size()); //calling function to save the data nicely



    for( int i = 0; i < N; i++){   //this whole thing basicly checks predetermined positions for every agent and checks if there are any infected agents there
        int infected_nearby = 0;
        if (not people[i].cont and people[i].state != 2 and people[i].state != 3 and people[i].state != 4){ //only checking for susceptibles
            
            vector<pair<int,int>> exact_pos_tocheck = pos_tocheck[make_pair(people[i].pos.x,people[i].pos.y)];
            
            for( int m = 0; m < (int) exact_pos_tocheck.size(); m++){
                
                pair<int,int> pos_tocompare = exact_pos_tocheck[m];

                if (inf_positions.find(pos_tocompare) != inf_positions.end()){
                    
                    int a = pos_tocompare.first - people[i].pos.x;
                    
                    int b = pos_tocompare.second - people[i].pos.y;

                    if (std::abs(a) <= 1 and std::abs(b) <= 1){
                        
                        infected_nearby += 1;
                    }
                }
            }
        if (infected_nearby > 0)  //using rng to say if a susceptible agent gets infected
        {   
        
            if (infected_nearby * R_0 <= 100){
                
                if ( dist(mt) < R_0  ){
                    infected.push_back(i);
                }
            }
            else{
                 
                infected.push_back(i);
            }
        } 
        }
    }
    for (int i = 0; i < (int) infected.size(); i++ ){ //similar to the one up, this whole for cycle makes agents move
        people[infected[i]].cont = true;
    }
    for(int i = 0; i < N; i++){
        if (people[i].state == 4){
            continue;
        };
        int l = 0;
        vector<pair<int,int>> possible_coordinates;  //again, predetermined coordinates
        for (int i = -1; i < 2; i++)
        {
            for (int j = -1; j < 2; j++)
            {
                possible_coordinates.push_back(make_pair(i,j));
            };
            
        };

        
        while( l < 9){  //checks all 9 possible positions for every agent, the moves the agent there if its free and inside the bounds
            l = l+1;
            uniform_int_distribution <int> random(1,9);
            int x = random(mt);

            pair<int,int> try_coordinates = possible_coordinates[x];

            pair<int,int> newpos = make_pair(people[i].pos.x+try_coordinates.first,people[i].pos.y+try_coordinates.second);  //basicyl checking the  9 positions randomly without repetition
            if( size <= newpos.first or 0 == newpos.first or size <= newpos.second or 0 == newpos.second){                  
                possible_coordinates.erase(possible_coordinates.begin() + x);
                continue;
            };
            
            if (positions.find(newpos) != positions.end()){
                possible_coordinates.erase(possible_coordinates.begin() + x);
                continue;
            };
            positions.erase(make_pair(people[i].pos.x,people[i].pos.y));
            people[i].pos.x = newpos.first;
            people[i].pos.y = newpos.second;

            positions.insert(newpos); //updating positions
            
            break; //if new position is found the cycle end
        };
    
    };
    
    /*
    //update data
    //delete_pos = np.nonzero(grid)


    //for i in range(len(people)):
    //    print(i)
    //    grid[delete_pos[0][i]][delete_pos[1][i]] = 0

   // grid = np.zeros(shape = (size+1,size+1))

    //for i in range(len(people)):

    //    grid[people[i].pos[0]][people[i].pos[1]] = people[i].state

    //for i in sorted(del_list, reverse=True){
    //    del people[i]
    //}
    //img.set_data(grid)
    */
    return is_any_infected;
    
    
};
// starting function

void start(){
    
    
    
    int M[5]= {-2,-1,0,1,2};
    int J[5] = {-2,-1,0,1,2};
    vector<pair<int,int>> temp;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            temp.push_back(make_pair(M[i],J[j]));
        }  
    
    }
    // possible positions, and the corresponding positions to check

    vector<vector<pair<int,int>>> lista;
    vector<pair<int,int>> poziciok;
    vector<pair<int,int>> temp2;

    int temp_size = static_cast<int>(temp.size());   //this whole thing generates all positions next to any position in the environment
    for (int i = 1; i < size; i++){
        for (int j = 1; j < size; j++){
            poziciok.push_back(make_pair(i,j));
            
            if (i < 2 and j < 2){
                for(int m = 0; m < temp_size; m++){
                    if ( temp[m].first > -2 and temp[m].second > -2 ){
                        temp2.push_back(make_pair(i+temp[m].first,j+temp[m].second));
                    }}}
            else if (i < 2 and j >= 2){
                for(int m = 0; m < temp_size; m++){
                    if (temp[m].first > -2){
                        temp2.push_back(make_pair(i+temp[m].first,j+temp[m].second));
                    }}}
            else if (i >= 2 and j < 2){
                for(int m = 0; m < temp_size; m++){
                    if (temp[m].second > -2){
                        temp2.push_back(make_pair(i+temp[m].first,j+temp[m].second));
                    }}}
            else if (i > size-2 and j > size-2){
                for(int m = 0; m < temp_size; m++){
                    if (temp[m].first < 2 and temp[m].second < 2){
                        temp2.push_back(make_pair(i+temp[m].first,j+temp[m].second));
                    }}}
            else if (i > size-2 and j <=  size-2){

                for(int m = 0; m < temp_size; m++){

                    if (temp[m].first < 2){
                        temp2.push_back(make_pair(i+temp[m].first,j+temp[m].second));
                    }}}
            else if (i <= size-2 and j  > size-2){
                for(int m = 0; m < temp_size; m++){
                    if (temp[m].second < 2){
                        temp2.push_back(make_pair(i+temp[m].first,j+temp[m].second));
                    }}}
            else{
                for(int m = 0; m < temp_size; m++){
                    temp2.push_back(make_pair(i+temp[m].first,j+temp[m].second));
                    }}
            lista.push_back(temp2);
            temp2.clear();
                    
        }
    }

    std::map< pair<int,int>, vector<pair<int,int>> > pos_tocheck;   //map of all possible positions
    
    int a = static_cast<int>(poziciok.size());  //adding positions to check for all positons possilbe, basicly a look up table: see where the agent is and you know what positions to check
    for(int i = 0; i < a; i++){
        pos_tocheck[poziciok[i]] = lista[i];

    };
   
    // declare grid



    Person* people = Enviroment(); //generate agents

    
    //int grid[size+1][size+1] = { 0 };
    //for( int i; i < N; i++){
    //    grid[people[i].pos[0]][people[i].pos[1]] = people[i].state

    bool is_any_infected = true; 

    while (is_any_infected){   //run the updating until no infected left
        is_any_infected = update(people, pos_tocheck, is_any_infected); 
    }
    delete[] people;
}

// call main
    
    

    

int main(){

using namespace std::chrono;  //starting the clock
auto starttime = high_resolution_clock::now();

string fileName = "data_D_0=";
string fileName2 = "R_0=";  //filename parts
string fileType = ".txt";
 
stringstream ss;
 
ofstream f;

int n = 1; //number of iterations per set D_0, I recommend at least 20 for this value

for(int i = 0; i < 1; i++){  //decides the range of D_0 to run
    R_0 = 5;    //have to manually set R_0
    D_0 = i * 5 + 10;  //i * a + c format, a is the D_0 change per iteration, c is the starting point
    Recov_0 = 1-D_0;

    
    ss<<fileName<<D_0<<fileName2<<R_0<<fileType; //setting the name of the file where the data will be printed
         
    f.open(ss.str());   //creating files with names of D_0 included for easier readback
    
    for(int j = 0; j < n; j++ ){

        data.push_back(make_pair(0,0));
        start();


        for (int i = 0; i < data.size(); i++)
        {
            f << data[i].first << "," << data[i].second << ","; //prints new death and infected count into the file. First is infected, second is dead
                                                                //summing will give the total number of infected or dead, which in my opinion the most interesting when doing parameter sweeping
                                                                
        }
        f << "\n";  //makes it so that every iteration of simulation is in a new line in the file
        

        data.clear();    //clears data vector for new iteration
        auto stoptime = high_resolution_clock::now();
        auto duration = duration_cast<seconds>(stoptime - starttime);
        cout << duration.count() << endl;   //prints time, good for seeing if the program is running, and if there are problems they are sometimes where the time is too high or too low for a run
    }
    f.close();
    ss.str("");  //closing file and clearing ss
    
}


return 0; //ending the program
}


    
    



