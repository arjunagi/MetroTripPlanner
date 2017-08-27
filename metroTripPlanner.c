/*
 * Please compile using "gcc -std=c99" 
 *
 * Description: This is a program to find the fastest path from source station to destination station in Washington DC metro map.
 *
 * Data Structures Used:
 * 1. LINE - Structure for a line - will have start and end stations along with number of stations on a line.
 * 2. Array of lines (each element is an object of type LINE)
 * 3. STATION - Stations of a line in a linked list 
 * 4. TRANSFERSTATION - Strcuture to hold the transfer station - will have pointer to actual station (of type STATION) which is in the linked list of a line.
 * 5. Array of transfer stations (each element is an object of type TRANSFERSTATION).
 *
 * Algorithm:
 * 1. Create a graph with all the lines (each line is a linked list) - The TRANSFERSTATION objects provides the connections in the graph.
 * 2. If source and dest are on same line, no need of transfer
 * 3. From the array of transfer stations, we can know at what stations transfer can be done and to what line. 
 * 4. If they are not on same line, find a transfer station on the source line from which we can make a transfer to dest line.
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

#define NUM_OF_LINES 6
#define NEW(x) (x*)malloc(sizeof(x))

//File to write the  output to.
FILE *out;

// Global variables to store the source and destination stations 
char* sourceName = NULL;
char* destinationName = NULL;

//Data structure for each station.
typedef struct station {
  char* lineName; //the line on which this station is located
  char* stationName;
  int stationNumber; //Each station in a line is assigned a number
  int numOfTransferLines; //number of other lines a transfer can be done to
  int timeToReach; //Time to reach this station from the first station on this line.
  int stopTime; //Duration for which the train stops on this station.
  char* transferLines[4]; //To store the transfer lines at a station.
  int transferTimes[4]; //To store the transfer times at a station.
  struct station* next;
  struct station* prev;
} STATION;

// Structure object for source and destination stations. Not part of any linked list.
STATION *source;
STATION *destination;

/*
 **************************************************************************************
 * Data structure for the line(root) node. The root node has the 
 * pointers to the first and last stations and also the number of stations on a line.
 **************************************************************************************
 */
typedef struct {
  int numOfStations;
  STATION * start;
  STATION * end;
}LINE;

LINE* line[6] = {NULL}; //There are 6 lines

/*
 ********************************************************************************
 * Structure to store a transfer station and its properties:
 * 1.Pointer to the transfer station node in the line linked list.
 * 2.Name of the station
 * 3.The current line color. 
 ********************************************************************************
 */
typedef struct transferStations {
  STATION* station; // Pointer to the transfer station in a line  
  char* stationName;
  char* lineName;
} TRANSFERSTATION;

//Array of transfer stations. Example: Fort Totten of green line is considered 1 transfer station and For Totten of red is considered as another.  
TRANSFERSTATION** transferStations;

// Create a line (Root)
LINE* makeLine() {
  LINE * temp;
  temp = NEW(LINE);
  if (temp != NULL) {
    temp->numOfStations = 0;
    temp->start = NULL;
    temp->end = NULL;
  }
  return temp;
}

// Creat a new node with the data as passed
STATION* makeStation(char* lineName, char* stationName, int stationNumber, int numOfTransferLines, int timeToReach, int stopTime, char* transferLines[], int transferTimes[]) {
  STATION* temp;
  temp = NEW(STATION);
  if (temp != NULL) {
    temp->lineName = lineName;
    temp->stationName = stationName;
    temp->stationNumber = stationNumber;
    temp->numOfTransferLines = numOfTransferLines;
    temp->timeToReach = timeToReach;
    temp->stopTime = stopTime;
    temp->next = NULL;
    temp->prev = NULL;
    if(numOfTransferLines!=0) {
      for(int i=0; i<numOfTransferLines; i++) {
        temp->transferLines[i] = transferLines[i];
        temp->transferTimes[i] = transferTimes[i];
      }
    }
  }
  return temp;
}

// Create a TransferStation
TRANSFERSTATION* makeTransferStation(STATION *station, char* stationName, char* lineName) {
  TRANSFERSTATION *temp;
  temp = NEW(TRANSFERSTATION);
  if(temp!=NULL) {
    temp->station = station;
    temp->stationName = stationName;
    temp->lineName = lineName;
  }
  return temp;
}

// Create new node with the passed data and insert at the tail of the list
STATION* insertStationInLine(LINE *line, char* lineName, char* stationName, int stationNumber, int numOfTransferLines, int timeToReach, int stopTime, char* transferLines[], int transferTimes[]) {

  STATION *temp;
  temp = makeStation(lineName,stationName,stationNumber,numOfTransferLines,timeToReach,stopTime,transferLines,transferTimes);
  
  if (temp == NULL) return NULL; // fail, cannot create new NODE

  if (line == NULL) {
    line = makeLine();
    if (line == NULL) return NULL;   // fail, cannot create ROOT
  }

  (line->numOfStations)++;

  if (line->numOfStations == 1)              // if previously the list is empty
    line->start = line->end = temp;

  else {
    line->end->next = temp;
    temp->prev = line->end;
    line->end = temp;
  }
  return temp;
}


/*
 **************************************************************************
 * There are 9 MAIN transfer stations in the map. Considering only those.
 **************************************************************************
 */
bool isStationValidTransferStation(char* stationName) {

  if(strcmp(stationName,"Fort_Totten") == 0) return true;
  else if(strcmp(stationName,"East_Falls_Church") == 0) return true;
  else if(strcmp(stationName,"Rosslyn") == 0) return true;
  else if(strcmp(stationName,"Metro_Center") == 0) return true;
  else if(strcmp(stationName,"Gallery_Place") == 0) return true;
  else if(strcmp(stationName,"L'Enfant_Plaza") == 0) return true;
  else if(strcmp(stationName,"Stadium_Armory") == 0) return true;
  else if(strcmp(stationName,"Pentagon") == 0) return true;
  else if(strcmp(stationName,"King_St_Old_Town") == 0) return true;
  else return false;
}



/*
 *************************************************************
 * Get the index of the required line from the LINE array.
 * This will give us the required LINE object from the array.
 *************************************************************
 */
int getLineIndex(char* lineColor) {
  if(strcmp(lineColor,"green") == 0) return 0;
  if(strcmp(lineColor,"yellow") == 0) return 1;
  if(strcmp(lineColor,"blue") == 0) return 2;
  if(strcmp(lineColor,"silver") == 0) return 3;
  if(strcmp(lineColor,"orange") == 0) return 4;
  if(strcmp(lineColor,"red") == 0) return 5;
  else return -1;
}

/*
 *************************************************************
 * Get the correct station as the source station.
 * Ex: to go from Fort Tottent to Pentagon, choose Fort Totten
 * of yellow line as source, not the green line.
 *************************************************************
 */
STATION* getCorrectStation(STATION* source, STATION* dest) {

  STATION *temp = NULL;
  bool found = false;
  int lineIndex, j;

  for(int i=0; i<source->numOfTransferLines; i++) {
    if(strcmp(source->transferLines[i], dest->lineName) == 0) {
      found = true;
      goto found;
    }
  }
  found:
  if(found == true) {
    lineIndex = getLineIndex(dest->lineName);
    temp = line[lineIndex]->start;
    while(strcmp(source->stationName, temp->stationName) !=0)
      temp = temp->next;
    return temp;
  }
  else return source;
}


/*
 *************************************************
 * Read the stations from the file along with its 
 * properties and store it in the data strcuture.
 *************************************************
 */
void readStationsFromFile(LINE* line[], TRANSFERSTATION* transferStations[]) {

  FILE *metro = fopen("metro.txt", "r");
  if(metro == NULL) {
    printf("\nmetro.txt file could not be opened\n");
    exit(0);
  }

  int ch=0;
  char *lineInfo = NULL, *lineName = NULL; 
  char *blankLine = calloc(5, sizeof(char*));
  int numOfStations = 0;
  char *stationInfo = NULL, *stationName = NULL;
  int numOfTransferLines = 0, stopTime = 0, timeToReach = 0;
  char** transferLines = NULL;
  int transferTimes[4];
  STATION *station = NULL;
  int k=0, n=0, x=0, y=0;
  char *tempTransferLines[8];
  char *temp = NULL;
  bool sourceFound=false, destFound=false;

  //Store the stations in each line
  for(int i=0; i<NUM_OF_LINES; i++) {
    lineInfo = (char*) malloc(20);
    lineName = (char*) malloc(10);
    memset(transferTimes, 0, sizeof(transferTimes));
    fgets(lineInfo,20,metro);
    sscanf(lineInfo, "%s (%d)", lineName, &numOfStations);
    line[i] = makeLine();

    // Store all the stations in this line
    for(int j=0;j<numOfStations;j++) {
      transferLines = (char**) malloc(sizeof(char) * 4);
      stationInfo = (char*) malloc(100);
      stationName = (char*) malloc(30);
      numOfTransferLines = 0;
      fgets(stationInfo,100,metro);
      sscanf(stationInfo,"%s %d %d %d", stationName, &numOfTransferLines, &timeToReach, &stopTime);
      temp = strtok (stationInfo," ");
      n=0;
      if(numOfTransferLines != 0) {
         while (temp != NULL && n<(4+(numOfTransferLines*2)))  
  	 {
         //At transfer stations, there are 4 strings of station name, transfers, time from first stop, stop time. If there is 1 transfer, then there are 2 
         //more strings - transfer line and transfer time. If there is 2 transfers, then there are 4 more strings and so on.
           if(n>=4)
             tempTransferLines[n-4] = temp;
           temp = strtok(NULL, " ");
           n++;
         }
        x=0, y=0;
        // Store the transfer lines and the transfer times in their respective arrays.
        for(n=0;n<(numOfTransferLines*2);n++) {
          if(n%2 == 0) { transferLines[x] = tempTransferLines[n]; x++; } 
          else { transferTimes[y] = atoi(tempTransferLines[n]); y++; }
        }
      }
      //Create the structure object and insert it in the list.
      station = insertStationInLine(line[i], lineName, stationName, j+1, numOfTransferLines, timeToReach, stopTime, transferLines, transferTimes);

      //Store only the main transfer stations in the transferStation array
      if(numOfTransferLines != 0 && isStationValidTransferStation(stationName)) {
         transferStations[k++] = makeTransferStation(station, stationName, lineName);
      }

      // Check if source and destination stations created are correct. If yes, then store their Station objects.
      if((strcmp(stationName,sourceName) == 0) && sourceFound == false) {
         sourceFound = true;
         source = station;
      }
      if((strcmp(stationName,destinationName) == 0) && destFound == false) {
         destFound = true;
         destination = station;
      }
   }
   fgets(blankLine, 5, metro); 
  }
  if(sourceFound == true && destFound == true) {
    source = getCorrectStation(source,destination);
  }
  free(stationInfo);
  free(stationName);
  free(lineInfo);
  free(transferLines);
  free(blankLine);
  free(lineName);
  fclose(metro);
  if(sourceFound == false || destFound == false) {
    printf("\n Source or destination station not found. Please try again! \n\n");
    exit(0);
  }
}


/*
 *****************************************************************
 * Get the direction in which the line traversal has to be done.
 *****************************************************************
 */
int getDirection(STATION *source, STATION *dest) {

   int direction = 0; // 0 - towards end of line, 1 - towards start of line

   if(source->stationNumber > dest->stationNumber) {
     direction = 1;
   }
   else if (source->stationNumber < dest->stationNumber) {
     direction = 0;
   }
   return direction;
}

/*
 *****************************************************************
 * Get the transfer station on the current line.
 * Ex: If transfer has to be done at Ft Totten from green line to
 * red line, then this function will return the STATION object for
 * Ft Totten of green(current) line.
 *****************************************************************
 */
STATION* getCurrentTransferStation(STATION* source, STATION* dest) {
  STATION *temp = source;
  bool found = false;
  char* destLineColor = dest->lineName;


  // Search on both sides of the source station

  while(temp->next != NULL && found == false) {
    for(int i=0; i<temp->numOfTransferLines; i++) {
      if(strcmp(temp->transferLines[i], destLineColor) == 0) {
        found = true;
        break;
      }
      if(found == true) break;
    }
    if(found == true) break;
    temp = temp->next;
  }

  if(found == false) temp = source;

  while(temp->prev != NULL && found == false) {
    for(int i=0; i<temp->numOfTransferLines; i++) {
      if(strcmp(temp->transferLines[i], destLineColor) == 0) {
        found = true;
        break;
      }
    }
    if(found == true) break;
    temp = temp->prev;
  }
  
  if(found == true) return temp;
  else return NULL;
}


/*
 ************************************************
 * Get the transfer time at a transfer station
 ************************************************
 */
int getTransferTime(STATION* source, STATION* dest, char* destLineColor) {
  
  STATION* temp = getCurrentTransferStation(source,dest);
  for(int i=0; i<temp->numOfTransferLines; i++) {
    if(strcmp(temp->transferLines[i], destLineColor) == 0) 
      return temp->transferTimes[i];
  }
  return 0;
}


/*
 ****************************************************
 * Get the total stop time between source and dest.
 ****************************************************
 */
int getStopTimes(STATION *source, STATION* dest) {

  int totalStopTime = 0;
  int direction = getDirection(source,dest);
  STATION *temp = NULL;

  if(direction==0) temp = source->next;
  else if(direction==1) temp = source->prev;

  for(int i=0;i<abs(source->stationNumber - dest->stationNumber)-1; i++) {
     if(direction == 0) {
       totalStopTime+= temp->stopTime;
       temp = temp->next;
     }
     else if(direction == 1) {
       totalStopTime+= temp->stopTime;
       temp = temp->prev;
     }
  }
  return totalStopTime;
}

/*
 *****************************************************************************
 * Get the index of the transfer station from the array of transfer stations.
 *****************************************************************************
 */
int getTransferStationIndex(char* stationName, char* lineName) {
  int i=0;
  for(i=0; i<25; i++) {
    if((strcmp(transferStations[i]->stationName, stationName) == 0) && (strcmp(transferStations[i]->lineName, lineName) == 0)) 
      goto found_index;
  }
  found_index:
  return i;
}

/*
 *******************************************************
 * Get the station to which the transfer has to made.
 *******************************************************
 */
STATION* getTransferStation(STATION* source, STATION* dest) {

  STATION *temp = NULL;
  STATION *transferStation;
  int transferStationIndex = 0;
  char* destLineColor = dest->lineName;
  
  temp = getCurrentTransferStation(source,dest);
  if(temp != NULL) {
    transferStationIndex =  getTransferStationIndex(temp->stationName, destLineColor);
    transferStation = transferStations[transferStationIndex]->station;
  }
  return transferStation;
}

/*
 ***************************************************************
 * Display the final path to take and also store it in the file.
 ***************************************************************
 */
void displayPathAndWriteToFile(int sourceLineIndex, int destLineIndex, STATION *source, STATION * dest, STATION *transferStation, int numberOfStationsCurrentLine, int numberOfStationsTransferLine, STATION* currentTransferStation) {

  int stopTime=0, totalTimeMin=0, totalTimeSec=0, transferTime=0; 
  char *towards1, *towards2; //Station name towards which the train is headed.

  //No transfer required
  if(transferStation == NULL) {
    if(source->stationNumber > dest->stationNumber)
      towards1 = line[sourceLineIndex]->start->stationName;
    else if (source->stationNumber < dest->stationNumber) 
      towards1 = line[sourceLineIndex]->end->stationName;

    stopTime = getStopTimes(source,dest);
    totalTimeMin = (stopTime + abs(source->timeToReach - dest->timeToReach))/60;
    totalTimeSec = (stopTime + abs(source->timeToReach - dest->timeToReach))%60;

    fprintf(out, "Start from %s station on %s line towards %s for %d stations to arrive at %s.\nTotal duration of journey: %d minutes %d seconds\n", source->stationName, source->lineName, towards1, numberOfStationsCurrentLine, dest->stationName, totalTimeMin, totalTimeSec);

    printf("\nStart from %s station on %s line towards %s for %d stations to arrive at %s.", source->stationName, source->lineName, towards1, numberOfStationsCurrentLine, dest->stationName);
    printf("\nTotal duration of journey: %d minutes %d seconds\n\n", totalTimeMin, totalTimeSec);
  }

  //Transfer required
  else {
    if(source->stationNumber > currentTransferStation->stationNumber)
      towards1 = line[sourceLineIndex]->start->stationName;
    else if (source->stationNumber < currentTransferStation->stationNumber)
      towards1 = line[sourceLineIndex]->end->stationName;

    if(transferStation->stationNumber > dest->stationNumber)
      towards2 = line[destLineIndex]->start->stationName;
    else if(transferStation->stationNumber <= dest->stationNumber)
      towards2 = line[destLineIndex]->end->stationName;

    stopTime = getStopTimes(source,currentTransferStation);
    stopTime+= getStopTimes(transferStation, dest);
    transferTime = getTransferTime(source, dest, dest->lineName);

    //If the destination is sae as transfer station, then no need to consider the transfer or transfer time
    if(numberOfStationsTransferLine == 0 && (strcmp(transferStation->stationName, dest->stationName)==0)) {
      totalTimeMin = (stopTime + abs(currentTransferStation->timeToReach - source->timeToReach) + abs(transferStation->timeToReach - dest->timeToReach))/60;
      totalTimeSec = (stopTime + abs(currentTransferStation->timeToReach - source->timeToReach) + abs(transferStation->timeToReach - dest->timeToReach))%60;

    fprintf(out, "Start from %s station on %s line towards %s for %d stations to reach %s.\nTotal duration of journey: %d minutes %d seconds.", source->stationName, source->lineName, towards1, numberOfStationsCurrentLine, currentTransferStation->stationName, totalTimeMin, totalTimeSec);

    printf("\nStart from %s station on %s line towards %s for %d stations to reach %s.", source->stationName, source->lineName, towards1, numberOfStationsCurrentLine, currentTransferStation->stationName);
    printf("\nTotal duration of journey: %d minutes %d seconds\n\n", totalTimeMin, totalTimeSec);
    }

    // Consider the transfer and transfer time
    else{
      totalTimeMin = (stopTime + abs(currentTransferStation->timeToReach - source->timeToReach) + transferTime + abs(transferStation->timeToReach - dest->timeToReach))/60;
      totalTimeSec = (stopTime + abs(currentTransferStation->timeToReach - source->timeToReach) + transferTime + abs(transferStation->timeToReach - dest->timeToReach))%60;
    
      fprintf(out, "Start from %s station on %s line towards %s for %d stations to reach %s.\nTransfer to %s line.\nTake %s line towards %s for %d stations to reach %s.\nTotal duration of journey: %d minutes %d seconds.", source->stationName, source->lineName, towards1, numberOfStationsCurrentLine, currentTransferStation->stationName, transferStation->lineName, transferStation->lineName, towards2, numberOfStationsTransferLine, dest->stationName, totalTimeMin, totalTimeSec);

      printf("\nStart from %s station on %s line towards %s for %d stations to reach %s.", source->stationName, source->lineName, towards1, numberOfStationsCurrentLine, currentTransferStation->stationName);
      printf("\nTransfer to %s line.", transferStation->lineName);
      printf("\nTake %s line towards %s for %d stations to reach %s", transferStation->lineName, towards2, numberOfStationsTransferLine, dest->stationName);
     printf("\nTotal duration of journey: %d minutes %d seconds\n\n", totalTimeMin, totalTimeSec);
   }
  }
}

 
/*
 ******************************************************
 * Find the path and write to the file.
 ******************************************************
 */
void findPathAndWriteToFile(STATION *source, STATION* dest) {
  
  int sourceLineIndex = getLineIndex(source->lineName); 
  int destLineIndex = getLineIndex(dest->lineName); 
  int numberOfStationsCurrentLine = 0, numberOfStationsTransferLine = 0; //Number of stations to be considered in the path from source line and dest line.
  STATION *transferStation = NULL; //Transfer station on destination line
  STATION *currentTransferStation = NULL; //Transfer station on source line. Station name will be same as the above, but lineName will be diffrent

  //If source and destination is on same line
  if(strcmp(source->lineName, dest->lineName) == 0) {
    numberOfStationsCurrentLine = abs(dest->stationNumber - source->stationNumber);
    displayPathAndWriteToFile(sourceLineIndex, destLineIndex, source, dest, transferStation, numberOfStationsCurrentLine, numberOfStationsTransferLine, currentTransferStation);
  }

  // source and destination on different line
  else {
     currentTransferStation = getCurrentTransferStation(source, dest);
     transferStation = getTransferStation(source, dest);
     numberOfStationsCurrentLine = abs(currentTransferStation->stationNumber - source->stationNumber);
     numberOfStationsTransferLine = abs(transferStation->stationNumber - dest->stationNumber);
     displayPathAndWriteToFile(sourceLineIndex, destLineIndex, source, dest, transferStation, numberOfStationsCurrentLine, numberOfStationsTransferLine, currentTransferStation);
  }
}


/*
 **********************************
 * Display the stations in a line
 **********************************
 */
void displayLine(LINE* line) {
  STATION *temp;
  if(line == NULL) printf("\n line is null\n");

  if (line != NULL) {
    temp = line->start;
    do {
      printf("%s ", temp->stationName);
      temp = temp->next;
    } while (temp != NULL);
  }
  printf("\n");
}


int main(int argc, char *argv[]) {

   if(argc != 2) {
     printf("\nWrong number of options provided for a.out\n");
     printf("\nThe usage is: a.out output_file\n");
     exit(0);
   }
   
   sourceName = (char*) malloc(30);;
   destinationName = (char*) malloc(30);;
   printf("\nEnter the source station(case sensitive): ");
   scanf("%s", sourceName);
   printf("Enter the destination station(case sensitive): ");
   scanf("%s", destinationName);

   source = NULL;
   destination = NULL;

   if(strcmp(sourceName, destinationName) == 0) {
     printf("\nSource and destination is same!\n");
     exit(0);
   }
  
   out = fopen(argv[1], "w+");

   //Array of transfer stations. Example: Fort Totten of green line is considered 1 transfer station and For Totten of red is considered as another.  
   transferStations = (TRANSFERSTATION**) malloc(sizeof(TRANSFERSTATION) * 25);
   readStationsFromFile(line, transferStations);

   findPathAndWriteToFile(source, destination);
  
   free(transferStations);
   fclose(out);
   return 0;
}
