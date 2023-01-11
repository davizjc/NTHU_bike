#include "nthu_bike.h"

void basic(string s)
{

    selectedCase = s;

    initial_outputs();

    // importing files
    if(!import_files(selectedCase))
        cout << "Couldn't import files\n";

    // find the shortest distances
    shortest_paths();

    // run time
    for (size_t t = MIN_TIME; t <= MAX_TIME; t++)
    {
        // Handle arriving bikes
        handle_bikes(t);

        // check if any user arrives
        handle_users(t);
    }

    print_bikes();

    f_station.close();
    f_users.close();
    f_transfer.close();
    
}

void advanced(string s)
{
    selectedCase = s;

    initial_outputs();

    // importing files
    if(!import_files(selectedCase))
        cout << "Couldn't import files\n";

    // find the shortest distances
    shortest_paths();

    // run time
    for (size_t t = MIN_TIME; t <= MAX_TIME; t++)
    {
        // Handle arriving bikes
        handle_bikes(t);

        // check if any user arrives
        handle_users(t);
    }

    print_bikes();

    f_station.close();
    f_users.close();
    f_transfer.close();
}

int main(int argc, char** argv)
{
    
    auto start = std::chrono::system_clock::now();
    string selectedCase = argv[1];
    string version = argv[2];
    cout << "You have set " << selectedCase << " as your testcase:" << "\n";
    cout << "running " << version << " currently" << "\n";
    cout << "-------------------------------------------" << endl;

    if(version == "advance")
        advanced(selectedCase);
    else
        basic(selectedCase);

    // record execution time
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    cout << "-------------------------------------------" << endl;
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;
    return 0;
}


//  chmod +x bin/verifier
// ./bin/verifier case1