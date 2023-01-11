#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstddef>
#include <cmath>
using namespace std;

// define size limits
#define MAX_STATIONS 1000
#define MAX_BIKES 10000
#define MIN_TIME 0
#define MAX_TIME 1440
#define MAX_USERS 100000
#define MAX_DIST 1e9

// output resources
string selectedCase;
string out_base = "./result/";
ofstream f_station;
ofstream f_users;
ofstream f_transfer;

// priority queue implementation.
template <typename t>
struct Node
{
    t element;
    float priority1;    
    int priority2;      //only id uses so don't need float
};

template <typename t>
class priority_queue
{
private:
    int size = 0, max_size;
    Node<t>* heap;
public:

    // constructor for the priority queue
    priority_queue()
    {
        // dynamically allocate an array of size = 1. This will get scaled up and down as required.
        heap = new Node<t>[2];
        max_size = 1; // zeroth index is ignored.
    }

    // doubles size of the array.
    void scale_up()
    {
        // do not scale up if not all current indices are used.
        if(size < max_size)
            return;

        // new max size
        max_size *= 2;

        // allocate new array of double the current max size.
        Node<t>* new_heap = new Node<t>[max_size+1];

        // copy contents of the current array into the new.
        for (size_t i = 1; i <= size; i++)
            new_heap[i] = heap[i];

        // deallocate memory of the old heap
        delete[] heap;

        // set heap pointer to the new heap array
        heap = new_heap;
    }

    // halves size of the array
    void scale_down()
    {
        // do not scale down if at least half of current indices are used
        if(size >= max_size/2)
            return;

        // new max size
        max_size /= 2;

        // allocate new array of half the current max size.
        Node<t>* new_heap = new Node<t>[max_size+1];

        // copy contents of the current array into the new.
        for (size_t i = 1; i <= size; i++)
            new_heap[i] = heap[i];

        // deallocate memory of the old heap
        delete[] heap;

        // set heap pointer to the new heap array
        heap = new_heap;
    }

    void enqueue(t element, float priority1, int priority2=0)
    {
        // scale up if needed
        scale_up();

        // insert node at the end of the heap
        heap[++size] = {element, priority1, priority2};

        // heapify up starting at the newly added node
        heapify_up(size);
    }

    bool peek(t& element)
    {
        // return false if queue is empty
        if(size <= 0)
            return false;

        element = heap[1].element; //first element of the heap starts at index 1 not 0
        return true;
    }

    //for debugging
    void print() 
    {
        for (int i=1; i<=size; i++)
        {
            cout<<heap[i].element.id << " p "<< heap[i].priority1 <<endl;
        }
    }

    bool dequeue(t& element)
    {
        // return false if queue is empty
        if(!peek(element))
            return false;

        // pop current root node from the queue & replace with last node
        heap[1] = heap[size--];

        // heapify down starting at root node
        heapify_down(1);

        // scale down if allowed
        scale_down();

        // all is well
        return true;
    }

    void heapify_up(int child)
    {
        // root has no parent
        if(child == 1)
            return;

        // get index of parent
        int parent = child/2;

        // compare priorities
        if( (heap[parent].priority1 > heap[child].priority1) ||
            ((heap[parent].priority1 == heap[child].priority1) && (heap[parent].priority2 > heap[child].priority2))
                )
        {
            // swap parent and child
            Node<t> tmp_node = heap[child];
            heap[child] = heap[parent];
            heap[parent] = tmp_node;

            // Continue the journy of heapifying up
            heapify_up(parent);
        }
    }

    void heapify_down(int parent)
    {
        // index of the min{parent, left child, right child}
        int min_index = parent;

        // get childs' indices
        int left_child = 2*parent;
        int right_child = left_child+1;

        // check for min_index
        if( left_child<=size &&
            ((heap[left_child].priority1 < heap[min_index].priority1) ||
             ((heap[left_child].priority1 == heap[min_index].priority1) && (heap[left_child].priority2 < heap[min_index].priority2)))
                )
            min_index = left_child;
        if( right_child<=size &&
            ((heap[right_child].priority1 < heap[min_index].priority1) ||
             ((heap[right_child].priority1 == heap[min_index].priority1) && (heap[right_child].priority2 < heap[min_index].priority2)))
                )
            min_index = right_child;

        // swap parent node and min_index node if the parent is not min_index
        if(parent != min_index)
        {
            // swap parent and child
            Node<t> tmp_node = heap[min_index];
            heap[min_index] = heap[parent];
            heap[parent] = tmp_node;

            // Continue the journy of heapifying down
            heapify_down(min_index);
        }
    }
    // check if empty
    bool isEmpty()
    {
        return size == 0;
    }

    // destructor for the priority queue
    ~priority_queue()
    {
        // deallocate the dynamically allocated array upon destruction of the class
        delete[] heap;
    }
};

// user struct
struct User
{
    int id;
    string bikes_type;
    int start_time;
    int end_time;
    int start_point;
    int end_point;
};

// bike struct
struct Bike
{
    int type;
    int id;
    int station;
    float rental_price=0.0;
    int rental_count;
    int arrival_time = 0;
};

// depreciation discount price and the rental count limit
float depreciation_discount_price;
int rental_count_limit;

// matrix of distances between stations
int distances[MAX_STATIONS+1][MAX_STATIONS+1];

// bikes initial rental price
int bike_info[MAX_BIKES+1];

// Each station has a priority queue of bikes
priority_queue<Bike> station_bikes[MAX_STATIONS+1][MAX_BIKES+1];

// Each station has a priority queue of retired bikes
priority_queue<Bike> retired_bikes[MAX_STATIONS+1][MAX_BIKES+1]; 

// bikes that has not arrived yet at the current time. Initially, all bikes start here.
priority_queue<Bike> not_yet_bikes;

// users priority queue
priority_queue<User> users;

// extracts the id integer from a given string id
int extract_int(string id)
{
    return stoi(id.substr(1));
}

// input the map
bool input_map(string file)
{

    // Initialize distances
    for (size_t i = 0; i <= MAX_STATIONS; i++)
        for (size_t j = 0; j <= MAX_STATIONS; j++)
            distances[i][j] = MAX_DIST;

    // initialize stream to read the file
    ifstream fin(file);

    if(fin.is_open())
    {
        // read and record each entry in the file
        string s1, s2;
        int id1, id2;
        int d;
        while (fin >> s1 >> s2 >> d)
        {
            // input data of station connection
            id1 = extract_int(s1);
            id2 = extract_int(s2);

            // record the connection
            distances[id1][id2] = d;
            distances[id2][id1] = d;
        }

        // close the stream upon completion
        fin.close();

        // return true: all is well
        return true;
    }

    // return false: something went wrong
    return false;
}

// input bike_info
bool input_bike_info(string file)
{
    // initialize stream to read the file
    ifstream fin(file);

    if(fin.is_open())
    {
        //read the depreciation discount price and the rental count limit
        fin >> depreciation_discount_price >> rental_count_limit;

        // read and record each entry in the bikes rental prices
        string B;
        int id, price;
        while (fin >> B >> price)
        {
            // input data of station connection

            id = extract_int(B);

            // record the connection
            bike_info[id] = price;
        }

        // close the stream upon completion
        fin.close();

        // return true: all is well
        return true;
    }

    // return false: something went wrong
    return false;
}

// input users
bool input_users(string file)
{
    // initialize stream to read the file
    ifstream fin(file);

    if(fin.is_open())
    {
        // read and record each user data
        string U, B, S1, S2;
        int IDU, IDB, IDS1, IDS2, start_time, end_time;
        while (fin >> U >> B >> start_time >> end_time >> S1 >> S2)
        {
            // input data of a single user
            IDU = extract_int(U);
            IDS1 = extract_int(S1);
            IDS2 = extract_int(S2);

            // record the user entry in users prioriy queue
            User u;
            u.id = IDU;
            u.bikes_type = B;
            u.start_time = start_time;
            u.end_time = end_time;
            u.start_point = IDS1;
            u.end_point = IDS2;
            users.enqueue(u, start_time, IDU);
        }
        // users.print();

        // close the stream upon completion
        fin.close();

        // return true: all is well
        return true;
    }

    // return false: something went wrong
    return false;
}

// input bikes
bool input_bikes(string file)
{
    // initialize stream to read the file
    ifstream fin(file);

    if(fin.is_open())
    {
        // read and record each bike entry
        string type, station;
        int t, s, id, count;
        float price;
        while (fin >> type >> id >> station >> price >> count)
        {
            // input data of a single bike
            t = extract_int(type);
            s = extract_int(station);

            // record the bike entry in the station it is initially in
            Bike b;
            b.type = t;
            b.id = id;
            b.station = s;
            b.rental_price = price;
            b.rental_count = count;
            // cout<< "b.id " << b.id << ' '<< t << ' '<<  s << " price "<< -price << endl;
            station_bikes[s][t].enqueue(b, -price, id);
        }

        // close the stream upon completion
        fin.close();
        // station_bikes[4][0].print();

        // return true: all is well
        return true;
    }

    // return false: something went wrong
    return false;
}

// input all data
bool import_files(string selectedCase)
{
    string dir = "./testcases/" + selectedCase;

    // import map
    if(!input_map(dir+"/map.txt"))
    {
        cout << "Error importing map.txt\n";
        return false;
    }

    // import bike info
    if(!input_bike_info(dir+"/bike_info.txt"))
    {
        cout << "Error importing bike_info.txt\n";
        return false;
    }

    // import users
    if(!input_users(dir+"/user.txt"))
    {
        cout << "Error importing user.txt\n";
        return false;
    }

    // import bikes
    if(!input_bikes(dir+"/bike.txt"))
    {
        cout << "Error importing bike.txt\n";
        return false;
    }

    // all is well
    return true;
}

// resolve any bikes that arrive at a given time
void handle_bikes(int time)
{
    Bike b;

    // loop till all bikes arriving now are extracted
    while (true)
    {
        // exit if no bikes arrived, or no bikes at all
        if(!not_yet_bikes.peek(b) || b.arrival_time!=time)
            return;

        // pop bike from the priority queue
        not_yet_bikes.dequeue(b);

        //retire the bike
        if (b.rental_count >= rental_count_limit){      
            retired_bikes[b.station][b.type].enqueue(b, b.id);
        }else{
            // insert the bike in the corresponding priority queue
            station_bikes[b.station][b.type].enqueue(b, -b.rental_price, b.id);
        }
    }
}

// floyd-warshall algorithm to find shortest path between two stations
void shortest_paths()
{
    // run algorithm to find shortest distances
    for (int k = 0; k < MAX_STATIONS; k++)
        for (int i = 0; i < MAX_STATIONS; i++)
            for (int j = 0; j < MAX_STATIONS; j++)
                if (distances[i][k] < MAX_DIST && distances[k][j] < MAX_DIST)
                    distances[i][j] = min(distances[i][j], distances[i][k] + distances[k][j]);
}

// handling a given user request
void handle_user(const User& u)
{

    Bike b, chosen_b;
    bool resolved = false;
    stringstream str(u.bikes_type);
    string bs;

    int distance = distances[u.start_point][u.end_point];
    int user_time = u.end_time - u.start_time;

    // Can the bike get there on time?
    if(distance >= user_time)
    {
        f_users << "U" << u.id << ' ' << 0 << ' ' << 0 << ' ' << 0 << ' ' << 0 << ' ' << 0 << '\n';
        return;
    }

    // for every desired bike_type
    while(getline(str, bs, ','))
    {
        // get bike type
        int btype = extract_int(bs);

        // Bike exists at station?
        if(!station_bikes[u.start_point][btype].peek(b))
            continue;


        resolved = true;

        // compare
        if(b.rental_price > chosen_b.rental_price)
            chosen_b = b;
        else if((b.rental_price == chosen_b.rental_price) && (b.id < chosen_b.id))
            chosen_b = b;

    }

    if(!resolved)
    {
        f_users << "U" << u.id << ' ' << 0 << ' ' << 0 << ' ' << 0 << ' ' << 0 << ' ' << 0 << '\n';
        return;
    }

    // remove bike from station
    station_bikes[u.start_point][chosen_b.type].dequeue(b);

    b.arrival_time = (u.start_time + distance);

    // print to users
    f_users << "U" << u.id << ' ' << 1 << ' ' << b.id << ' ' << u.start_time << ' ' << b.arrival_time << ' ' << std::floor(distance*b.rental_price) << '\n';

    b.station = u.end_point;        //bike station = user endpoint
    b.rental_count++;               //increase the rental count
    b.rental_price -= depreciation_discount_price;  //rental price - depreciation price
    not_yet_bikes.enqueue(b, b.arrival_time);   

    // print to transfer output
    f_transfer << b.id << " S"<<chosen_b.station << " S" << b.station << ' ' << u.start_time << ' ' << b.arrival_time << " U" << u.id << '\n';

}

// handle users arriving at a given time
void handle_users(int time)
{
    User u;

    // loop till all bikes arriving now are extracted
    while (true)
    {
        // exit if no users arrived, or no users at all
        if(!users.peek(u) || u.start_time!=time)
            return;
        // pop bike from the priority queue
        users.dequeue(u);

        // handle the user's request.
        handle_user(u);

    }
}

void initial_outputs()
{
    f_station.open(out_base + "/" + selectedCase + "/station_status.txt");
    f_users.open(out_base + "/" + selectedCase + "/user_result.txt");
    f_transfer.open(out_base + "/" + selectedCase + "/transfer_log.txt");
}

void print_bikes()
{
    //priority queue
    priority_queue<Bike> bs;

    for (size_t i = 0; i <= MAX_STATIONS; i++){
        // for output sorting
        priority_queue<Bike> output_bike;
        for (size_t j = 0; j <= MAX_BIKES; j++)
        {
            while (!station_bikes[i][j].isEmpty())
            {
                Bike b;
                station_bikes[i][j].dequeue(b);
                output_bike.enqueue(b, b.id);
            }
            while (!retired_bikes[i][j].isEmpty())
            {   
                Bike b;
                retired_bikes[i][j].dequeue(b);
                output_bike.enqueue(b, b.id);
            }
        }
        while (!output_bike.isEmpty())
            {
                Bike b;
                output_bike.dequeue(b);
                f_station << "S" << i << ' ' << b.id << " B" << b.type << ' ' << b.rental_price << ' ' << b.rental_count << '\n';
            }
    }
}

// // $case could be case1 or case2 or case3
// chmod +x bin/verifier
// ./bin/verifier $case1