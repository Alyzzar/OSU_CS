#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define MAX_CONNECTIONS 6
#define TOT_ROOMS 7

// Create struct for rooms
struct room { 
	char* name;
	char* type;
	int numOutboundConnections;
	struct room* outboundConnections[MAX_CONNECTIONS];
};

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
int IsGraphFull(struct room** rooms){
  //...
  return 0;
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(){
  struct room* A;
  struct room* B;

  while(1){
    A = GetRandomRoom();

    if (CanAddConnectionFrom(A) == 1)
      break;
  }

  do{
    B = GetRandomRoom();
  }
  while(CanAddConnectionFrom(B) == 0 || IsSameRoom(A, B) == 1 || ConnectionAlreadyExists(A, B) == 1);

  ConnectRoom(A, B);  // TODO: Add this connection to the real variables, 
  ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates
}

// Returns a random Room, does NOT validate if connection can be added
struct room GetRandomRoom(struct room** rooms){
	int rand_num = rand() % TOT_ROOMS;
	return rooms[rand_num];
	
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
int CanAddConnectionFrom(struct room* x){
	if (x.numOutboundConnections < MAX_CONNECTIONS){
	  return 1;
	}
	return 0;
}

// Returns true if a connection from Room x to Room y already exists, false otherwise
int ConnectionAlreadyExists(struct room* x, struct room* y){	
	int connections = 0;
	// for loop to see if y exists in x's outbound rooms
	int i = 0;
	for (i = 0; i < x.numOutboundConnections; i++){
		if (IsSameRoom(x.outboundConnections[i], y)){
			// X is connected to Y
			return 1;
		}
	}
	return 0;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct room* x, struct room* y){
	int n_x, n_y;
	n_x = x.numOutboundConnections;
	n_y = y.numOutboundConnections;
	x.outboundConnections[n_x] = y;
	y.outboundConnections[n_y] = x;
}

// Returns true if Rooms x and y are the same Room, false otherwise
int IsSameRoom(struct room* x, struct room* y){
	if (&x == &y) return 1;
	return 0;
}

// Set room names
void generateNames(struct room** rooms){
	int i, rand_num;
	// Create array of 10 potential room names
	char names[10][16] = {"Living Room", "Office", "Game Room", "Foyer", "Library", "Master Bedroom", "Guest Bedroom", "Dining Room", "Family Room", "Garage"};
	int taken[10] = {0,0,0,0,0,0,0,0,0,0};
	for(i = 0; i < TOT_ROOMS; i++){
		while (1) {
			rand_num = rand() % 10;
			if (taken[rand_num] == 0){
				rooms[i].name = names[rand_num];
				taken[rand_num] = 1;
				break;
			}
		}
	}
}

// Set room types
void setTypes(struct room** rooms){
	int i, rand_num;
	// Set random room to start and end
	rand_num = rand() % 7;
	rooms[rand_num].type = "START_ROOM"
	while(1){
		rand_num = rand() % 7;
		if(rooms[rand_num].type != "START_ROOM"){
			rooms[rand_num].type = "END_ROOM";
			break;
		}
	}
	
	for(i = 0; i < 10; i++){
		if((rooms[i].type != "START_ROOM") && (rooms[i].type != "END_ROOM")){
			rooms[i].type = "MID_ROOM";
		}
	}
}

// Generates rooms/connections, and exports rooms to files
void exportRooms(){
	//Variables
	int i;
	int j;
	int pid = getpid();
	int buffer = 64 * sizeof(char);
	int file_descriptor;
	struct stat st = {0};
	char *dir_name = malloc(buffer);
	char *file_path = malloc(buffer);
	char *file_connection = malloc(buffer);
	char *file_header = malloc(buffer);
	char *file_footer = malloc(buffer);
	
	// Create 7 rooms w/ generated (pre-listed) names
	struct room* rooms[TOT_ROOMS];
	generateNames(rooms);
	//***Create a function to randomly assign one START and one END room***//
	
	// Create all connections in graph
	while (IsGraphFull(rooms) == false)
	{
	  AddRandomConnection(rooms);
	}
	
	// Generate file directory
	sprintf(dir_name, "%s%d", "chanbe.rooms.", pid);
	
	if (stat(dir_name, $st) == -1){
		mkdir(dir_name, 0700);
	}
	
	// Open file to write

	for(i = 0; i < TOT_ROOMS; i++){
		// Create and open room file
		sprintf(file_path, "%s/%s_room", dir_name, rooms[i].name);
		file_descriptor = open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0700);
		
		if(file_descriptor == -1){
			printf("Error, could not generate room files at \"%s\"\n", file_path);
			perror("In main ()");
			exit(1);
		}
		
		//Write header to the file
		file_header = "";
		sprintf(file_header, "ROOM NAME: %d: %s\n", i, rooms[i].name);
		nwritten = write(file_descriptor, file_header, strlen(file_header) * sizeof(char));
		//Write connections to file
		for(j = 0; j < rooms[i].numOutboundConnections; j++){
			sprintf(file_connection, "CONNECTION %d: %s\n", j, rooms[i].name);
			nwritten = write (file_connection)
		}
		//Write footer to the file
		file_footer = "";
		sprintf(file_footer, "ROOM TYPE: %s\n", rooms[i].type);
	}
	free (dir_name);
	free (file_path);
	free (file_connection);
	free (file_header);
	free (file_footer);
}

int main (void) {
	//Create seed for random variables
	srand((unsigned) time(&t));
	//Run program
	exportRooms();
}
