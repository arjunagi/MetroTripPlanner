### Metro Trip Planner

Given an input file that contains the information about the metro rail system in the Washington DC metropolitan area (metro.txt), 
the program will help a customer to find the quickest trip from one station to another.

**Execution:**                                                                                                                        
The program will run with the following command:                                                                                        
a.out trip.txt                                                                                                                          
where a.out is the executable and trip.txt is the output file name provided by the user at the command line.

#### Code Logic:                                                                                                                         
**Data Structures Used:**                                                                                                               
 1. LINE - Structure for a line - will have start and end stations along with number of stations on a line.                             
 2. Array of lines (each element is an object of type LINE)                                                                             
 3. STATION - Stations of a line in a linked list                                                                                         
 4. TRANSFERSTATION - Strcuture to hold the transfer station - will have pointer to actual station (of type STATION)                    
 which is in the linked list of a line.                                                                                               
 5. Array of transfer stations (each element is an object of type TRANSFERSTATION).                                                     

**Algorithm:**
 1. Create a graph with all the lines (each line is a linked list) - The TRANSFERSTATION objects provides the connections in the graph.
 2. If source and dest are on same line, no need of transfer
 3. From the array of transfer stations, we can know at what stations transfer can be done and to what line.
 4. If they are not on same line, find a transfer station on the source line from which we can make a transfer to dest line.
