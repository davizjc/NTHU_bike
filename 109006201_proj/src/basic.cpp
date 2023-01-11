// #include "./nthu_bike.h"


// void basic(string s)
// {

//     selectedCase = s;

//     initial_outputs();

//     // importing files
//     if(!import_files(selectedCase))
//         cout << "Couldn't import files\n";

//     // find the shortest distances
//     shortest_paths();

//     // run time
//     for (size_t t = MIN_TIME; t <= MAX_TIME; t++)
//     {
//         // Handle arriving bikes
//         handle_bikes(t);

//         // check if any user arrives
//         handle_users(t);
//     }

//     print_bikes();

//     f_station.close();
//     f_users.close();
//     f_transfer.close();
    
    
// }