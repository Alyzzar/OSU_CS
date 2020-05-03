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

char* getName(struct room* room){
	return room->name;
}

void setName(struct room* room, char* name){
	//Allocate new memory
	room->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(room->name, name);
	//Add null terminator
	room->name[strlen(name)] = '\0';
}

char* getType(struct room* room){
	return room->type;
}

void setType(struct room* room, char* type){
	//Allocate new memory
	room->type = (char*)malloc(sizeof(char) * (strlen(type) + 1));
	strcpy(room->type, type);
	//Add null terminator
	room->type[strlen(type)] = '\0';
}

int getNumOut(struct room* room){
	return room->numOutboundConnections;
}

void setNumOut(struct room* room, int num){
	room->numOutboundConnections = num;
}

struct room* getOutbound(struct room* room, int n){
	return room->outboundConnections[n];
}

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
int IsGraphFull(struct room** rooms){
	int i;
	for (i = 0; i < TOT_ROOMS; i++){
		if(getNumOut(rooms[i]) < 3){
			return 0;
		}
	}
	return 1;
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

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct room* x, struct room* y){
	//printf("Connecting Rooms %s and %s\n", getName(x), getName(y));
	int n_x, n_y;
	//Create the connection by linking pointers
	n_x = getNumOut(x);
	n_y = getNumOut(y);
	x->outboundConnections[n_x] = y;
	y->outboundConnections[n_y] = x;
	
	//Increase number of connections in each struct
	//printf(" - x numOut: %d -> ",x->numOutboundConnections);
	x->numOutboundConnections++;
	//printf("%d\n", x->numOutboundConnections);
	
	//printf(" - y numOut: %d -> ",y->numOutboundConnections);
	y->numOutboundConnections++;
	//printf("%d\n", y->numOutboundConnections);
	//setNumOut(x, getNumOut(x) + 1);
	//setNumOut(y, getNumOut(y) + 1);
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

// Returns true if Rooms x and y are the same Room, false otherwise
int IsSameRoom(struct room* x, struct room* y){
	if (x == y) return 1;
	return 0;
}

// Set room names
void generateNames(struct room** rooms){
	int i, rand_num;
	// Create array of 10 potential room names
	char names[10][16] = {"Aldaraan", "Bespin", "Coruscant", "Dathomir", "Endor", "Felucia", "Geonosis", "Mandalore", "Mustafar", "Kamino"};
	int taken[10] = {0,0,0,0,0,0,0,0,0,0};
	//printf(" - loop to gen names\n");
	for(i = 0; i < TOT_ROOMS; i++){
		//printf(" - - Room %d\n", i);
		while (1) {
			//printf(" - - - Generating rand_num\n");
			rand_num = rand() % 10;
			//printf(" - - - rand_num: %d\n", rand_num);
			if (taken[rand_num] == 0){
				//printf(" - - - - taken[rand_num] == 0\n");
				setName(rooms[i], names[rand_num]);
				//printf(" - - - - Name successfully set to %s\n", getName(rooms[i]));
				taken[rand_num] = 1;
				//printf(" - - - - taken[rand_num] => 1. Loop break\n");
				break;
			}
			//printf(" - - - Name not set, looping... \n");
		}
	}
}

// Set room types
void generateTypes(struct room** rooms){
	int i, rand_num;
	
	// Set START_ROOM
	rand_num = rand() % 7;
	//printf(" - set room[%d] = START_ROOM\n", rand_num);
	setType(rooms[rand_num], "START_ROOM");
	
	// Set END_ROOM
	while(1){
		//printf(" - Loop to find unset room for END_ROOM\n");
		rand_num = rand() % 7;
		//printf(" - rand_num = %d\n", rand_num);
		if(strcmp(getType(rooms[rand_num]), "START_ROOM") != 0){
			//printf(" - - set room[%d] = END_ROOM\n", rand_num);
			setType(rooms[rand_num], "END_ROOM");
			//printf(" - - break loop\n");
			break;
		}
	}
	
	// Set MID_ROOM
	for (i = 0; i < TOT_ROOMS; i++){
		//printf(" - Loop to find unset room for MID_ROOM\n");
		if((strcmp(getType(rooms[i]), "START_ROOM") != 0) && (strcmp(getType(rooms[i]), "END_ROOM") != 0 )){
			//printf(" - - set room[%d] = MID_ROOM\n", rand_num);
			setType(rooms[i], "MID_ROOM");
			//printf(" - - break loop\n");
		}
	}
}

//Initializes values in rooms array
void initializeRooms(struct room** rooms){
	int i;
	for (i = 0; i < TOT_ROOMS; i++){
		rooms[i] = (struct room*)malloc(sizeof(struct room));
		setName(rooms[i], "");
		setType(rooms[i], "");
		setType(rooms[i], "");
		setNumOut(rooms[i], 0);
	}
}

//Prints value of a room for testing purposes
void printRoom(struct room* x){
	printf(" - PRINT ROOM\n");
	printf(" - - Name: %s\n", getName(x));
	printf(" - - Type: %s\n", getType(x));
	printf(" - - nOut: %d\n", getNumOut(x));
}

//Prints outbound rooms for testing purposes
void printOutbound(struct room* x){
	int i;
	printf(" - PRINT OUTBOUNDS\n");
	for (i = 0; i < getNumOut(x); i++){
		printf(" - - CONNECTION %d: %s\n", (i+1), getName(getOutbound(x, i)));
	}
}

// Generates rooms/connections, and exports rooms to files
void exportRooms(){
	//Variables
	int i;
	int j;
	int pid = getpid();
	int buffer = 128 * sizeof(char);
	int file_descriptor;
	struct stat st = {0};
	char *dir_name = malloc(buffer);
	char *file_path = malloc(buffer);
	char *file_connection = malloc(buffer);
	char *file_header = malloc(buffer);
	char *file_footer = malloc(buffer);
	char *curr_file = malloc(buffer);
	ssize_t nread, nwritten;
	
	//printf("Variables defined, generating rooms\n");
	// Allocate space for 7 room structs
	struct room* rooms[TOT_ROOMS];
	initializeRooms(rooms);
	
	// Create 7 rooms w/ generated (pre-listed) names
	//printf("Gen. names\n");
	generateNames(rooms);
	//printf("Gen. types\n");
	generateTypes(rooms);
	
	//printf("Generating room connections\n");
	// Create all connections in graph
	// Runs until graph is full (every room has 3 - 6 connections)
	while (IsGraphFull(rooms) == 0)
	{
	  AddRandomConnection(rooms);
	}
	
	printf("Generating file directory\n");
	// Generate file directory
	sprintf(dir_name, "%s%d", "chanbe.rooms.", pid);
	
	if (stat(dir_name, &st) == -1){
		mkdir(dir_name, 0700);
	}
	
	printf("Generating file for write\n");
	// Open file to write
	for(i = 0; i < TOT_ROOMS; i++){
		printRoom(rooms[i]);
		// Set curr_file to be the generated file's name
		sprintf(curr_file, "%s_ROOM", getName(rooms[i]));
		//printf(" - Generated file name = %s\n", curr_file);
		
		// Set file_path to be the location of the file (../dir/file)
		sprintf(file_path, "%s/%s", dir_name, curr_file);
		//printf(" - Current directory = %s\n",dir_name);
		//printf(" - Generated file path = %s\n", file_path);
		
		//printf(" - Creating file to read/write\n");
		file_descriptor = open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0700);
		
		//printf(" - Successfully created and opened file\n");
		
		if(file_descriptor == -1){
			printf("Error, could not generate room files at \"%s\"\n", file_path);
			perror("In main ()");
			exit(1);
		}
		
		//Write header to the file
		//printf(" - Generating header\n");
		sprintf(file_header, "ROOM NAME: %s\n", getName(rooms[i]));
		//printf(" - - Writing header\n");
		write(file_descriptor, file_header, strlen(file_header) * sizeof(char));
		
		printf(" - Generating connections\n");
		//Write connections to file
		printOutbound(rooms[i]);
		
		for(j = 0; j < getNumOut(rooms[i]); j++){
			//printf (" = = = = PRINTING OUTBOUND ROOMS = = = =\n");
			//printRoom(getOutbound(rooms[i], j));
			sprintf(file_connection, "CONNECTION %d: %s\n", j, getName(getOutbound(rooms[i], j)));
			printf("Generating connection %d: %s\n", j, getName(getOutbound(rooms[i], j)));
			
			printf(" - - Writing connection %d\n", j);
			write (file_descriptor, file_connection, strlen(file_header) * sizeof(char));
		}
		
		//Write footer to the file
		//printf(" - Generating footer\n");
		sprintf(file_footer, "ROOM TYPE: %s\n", getType(rooms[i]));
		//printf(" - Writing footer\n");
		write(file_descriptor, file_footer, strlen(file_footer) * sizeof(char));
		printf(" - Write successful, terminating\n");
	}
	free (dir_name);
	free (file_path);
	free (file_connection);
	free (file_header);
	free (file_footer);
	free (curr_file);
	//free (rooms);
}

int main (void) {
	//Create seed for random variables
	srand((unsigned) time(0));
	//Run program
	exportRooms();
}
