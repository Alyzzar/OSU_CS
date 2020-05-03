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

// Returns a random Room, does NOT validate if connection can be added
struct room* GetRandomRoom(struct room** rooms){
	int rand_num = rand() % TOT_ROOMS;
	return rooms[rand_num];
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
int CanAddConnectionFrom(struct room* x){
	if (getNumOut(x) < MAX_CONNECTIONS){
	  return 1;
	}
	return 0;
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(struct room** rooms){
  struct room* x;	//Pointer to a connectable random room
  struct room* y;	//Pointer to a different, connectable random room

  while(1){
    x = GetRandomRoom(rooms);

    if (CanAddConnectionFrom(x) == 1)
      break;
  }

  do{
    y = GetRandomRoom(rooms);
  }
  while(CanAddConnectionFrom(y) == 0 || IsSameRoom(x, y) == 1 || ConnectionAlreadyExists(x, y) == 1);

  ConnectRoom(x, y);
}

// Returns true if a connection from Room x to Room y already exists, false otherwise
int ConnectionAlreadyExists(struct room* x, struct room* y){	
	int connections = 0;
	// for loop to see if y exists in x's outbound rooms
	int i = 0;
	for (i = 0; i < getNumOut(x); i++){
		if (IsSameRoom(x->outboundConnections[i], y)){
			// X is connected to Y
			return 1;
		}
	}
	return 0;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct room* x, struct room* y){
	int n_x, n_y;
	//Create the connection by linking pointers
	n_x = getNumOut(x);
	n_y = getNumOut(y);
	x->outboundConnections[n_x] = y;
	y->outboundConnections[n_y] = x;
	//Increase number of connections in each struct
	x->numOutboundConnections++;
	y->numOutboundConnections++;
}

// Returns true if Rooms x and y are the same Room, false otherwise
int IsSameRoom(struct room* x, struct room* y){
	if (&x == &y) return 1;
	return 0;
}

char* getName(struct room* room){
	return room->name;
}

void setName(struct room* room, char* name){
	room->name = name;
}

char* getType(struct room* room){
	return room->type;
}

void setType(struct room* room, char* type){
	room->type = type;
}

int getNumOut(struct room* room){
	return room->numOutboundConnections;
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
				setName(rooms[i], names[rand_num]);
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
	setType(rooms[rand_num], "START_ROOM");
	while(1){
		rand_num = rand() % 7;
		if(getType(rooms[rand_num]) != "START_ROOM"){
			setType(rooms[rand_num], "END_ROOM");
			break;
		}
	}
	
	for(i = 0; i < 10; i++){
		if((getType(rooms[i]) != "START_ROOM") && (getType(rooms[i]) != "END_ROOM")){
			setType(rooms[i], "MID_ROOM");
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
	ssize_t nread, nwritten;
	
	// Create 7 rooms w/ generated (pre-listed) names
	struct room* rooms[TOT_ROOMS];
	generateNames(rooms);
	//***Create a function to randomly assign one START and one END room***//
	
	// Create all connections in graph
	while (IsGraphFull(rooms) == 0)
	{
	  AddRandomConnection(rooms);
	}
	
	// Generate file directory
	sprintf(dir_name, "%s%d", "chanbe.rooms.", pid);
	
	if (stat(dir_name, &st) == -1){
		mkdir(dir_name, 0700);
	}
	
	// Open file to write

	for(i = 0; i < TOT_ROOMS; i++){
		// Create and open room file
		sprintf(file_path, "%s/%s_room", dir_name, getName(rooms[i]));
		file_descriptor = open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0700);
		
		if(file_descriptor == -1){
			printf("Error, could not generate room files at \"%s\"\n", file_path);
			perror("In main ()");
			exit(1);
		}
		
		//Write header to the file
		file_header = "";
		sprintf(file_header, "ROOM NAME: %d: %s\n", i, getName(rooms[i]));
		nwritten = write(file_descriptor, file_header, strlen(file_header) * sizeof(char));
		//Write connections to file
		for(j = 0; j < getNumOut(rooms[i]); j++){
			sprintf(file_connection, "CONNECTION %d: %s\n", j, getName(rooms[i]));
			nwritten = write (file_connection);
		}
		//Write footer to the file
		file_footer = "";
		sprintf(file_footer, "ROOM TYPE: %s\n", getType(rooms[i]));
	}
	free (dir_name);
	free (file_path);
	free (file_connection);
	free (file_header);
	free (file_footer);
}

int main (void) {
	//Create seed for random variables
	srand((unsigned) time(0));
	//Run program
	exportRooms();
}
