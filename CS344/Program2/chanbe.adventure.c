#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_CONNECTIONS 6
#define TOT_ROOMS 7

struct room { 
	char* name;
	char* type;
	int numOutboundConnections;
	char* outboundConnections[MAX_CONNECTIONS];
};

struct game {
	unsigned int turnCount;
	char* start;
	char* end;
	char* path;
	char* directory;
	struct room* currRoom;
};

// Gets the name of the room
char* getName(struct room* room){
	return room->name;
}

// Sets the name of the room
void setName(struct room* room, char* name){
	//Allocate new memory
	room->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(room->name, name);
	//Add null terminator
	room->name[strlen(name)] = '\0';
}

// Gets room type
char* getType(struct room* room){
	return room->type;
}

// Sets room type
void setType(struct room* room, char* type){
	//Allocate new memory
	room->type = (char*)malloc(sizeof(char) * (strlen(type) + 1));
	strcpy(room->type, type);
	//Add null terminator
	room->type[strlen(type)] = '\0';
}

// Gets the numOutboundConnections for a room
int getNumOut(struct room* room){
	return room->numOutboundConnections;
}

// Sets the numOutboundConnections for a room
void setNumOut(struct room* room, int num){
	room->numOutboundConnections = num;
}

// Gets a room (name only) coming out of the room
char* getOutbound(struct room* room, int n){
	return room->outboundConnections[n];
}

// Adds the input string to the array of outbound nodes
void addOutbound(struct room* room, char* connection){
	room->outboundConnections[getNumOut(room)] = (char*)malloc(sizeof(char) * (strlen(connection) + 1));
	strcpy(room->outboundConnections[getNumOut(room)], connection);
	room->outboundConnections[getNumOut(room)][strlen(connection)] = '\0';
}

// Sets the start room in game struct
void setStart(struct game* game, char* start){
	//Allocate new memory
	game->start = (char*)malloc(sizeof(char) * (strlen(start) + 1));
	strcpy(game->start, start);
	//Add null terminator
	game->start[strlen(start)] = '\0';
}

// Set the end room in game struct
void setEnd(struct game* game, char* end){
	//Allocate new memory
	game->end = (char*)malloc(sizeof(char) * (strlen(end) + 1));
	strcpy(game->end, end);
	//Add null terminator
	game->end[strlen(end)] = '\0';
}

// Automatically finds and sets the file directory to the newest folder with the correct prefix
int setDir(struct game* game){
	int newestDirTime = -1;
	char targetDirPrefix[32] = "chanbe.rooms.";
	char newestDirName[256];
	memset(newestDirName, '\0', sizeof(newestDirName));

	DIR* dirToCheck;
	struct dirent *fileInDir;
	struct stat dirAttributes;

	//Open current directory
	dirToCheck = opendir(".");

	//If successful
	if (dirToCheck > 0){
		//Loop through subdirectories (Searching for chanbe.rooms.*)
		while ((fileInDir = readdir(dirToCheck)) != NULL){
			printf(" - - FILE: %s\n", fileInDir->d_name);
			
			// If directory has prefix (chanbe.rooms.*)
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL){
				printf(" - - - Matching prefix found.\n");
				
				stat(fileInDir->d_name, &dirAttributes);
				if ((int)dirAttributes.st_mtime > newestDirTime){
					printf(" - - - - Marking as newest folder.\n");
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
				}
			}
		}
	}
	closedir(dirToCheck);
	// Assign directory if it exists
	if(newestDirName >= 0){
		printf(" - - Assigning directory to %s.\n", newestDirName);
		//Allocate new memory for game->directory string
		game->directory = (char*)malloc(sizeof(char) * (strlen(newestDirName) + 1));
		strcpy(game->directory, newestDirName);
		//Add null terminator
		game->directory[strlen(newestDirName)] = '\0';
		//free(dirToCheck);
		//Return 1 if directory was found.
		return 1;
	}
	//free(dirToCheck);
	//Return 0 if directory could not be found.
	return 0;
}

// Gets game path
char* getPath(struct game* game){
	return game->path;
}

// Adds room to path
void addPath(struct game* game, struct room* room){
	char* old_path;
	int n_strlen = strlen(getPath(game)) + strlen(getName(room));
	game->path = (char*)malloc(sizeof(char) * (n_strlen + 3));
	sprintf(game->path, "%s\n%s", old_path, room);
	game->path[n_strlen] = '\0';
}

// Gets turn count
int getTurn (struct game* game){
	return game->turnCount;
}

// Initializes values in rooms array
void initializeRoom(struct room** room){
	*room = (struct room*)malloc(sizeof(struct room));
	setName(*room, "");
	setType(*room, "");
	setNumOut(*room, 0);
}

void initializeGame(struct game** game){
	*game = (struct game*)malloc(sizeof(struct game));
	setStart(*game, "");
	printf("INITIALIZE: START = %s\n", (*game)->start);
	setEnd(*game, "");
	printf("INITIALIZE: END = %s\n", (*game)->end);
	(*game)->turnCount = 1;
	printf("INITIALIZE: TURNCOUNT = %d\n", getTurn(*game));

	//Initialize path with length of 1
	(*game)->path = (char*)malloc(sizeof(char));
	printf(" - Game path initialized, creating currRoom.\n");
	initializeRoom(&(*game)->currRoom);
	
	//Initialize directory
	setDir(*game);
}

void freeGame(struct game* game){
	//free(game->turnCount);
	free(game->start);
	free(game->end);
	free(game->path);
	free(game->directory);
	free(game->currRoom);
	free(game);
}

int assignRoom(struct room* x, struct room* y){
	free (x);
	x = (struct room*)malloc(sizeof(struct room));
	setName(x, getName(y));
	setType(x, getType(y));
	setNumOut(x, getNumOut(y));
}

// Returns a parsed room from a file.
struct room* parseRoom(FILE* f, struct game* game){
	int i;
	int num_lines = 0;
	int running;
	char name [64];
	char type [64];
	char* connection = malloc(64 * sizeof(char));
	size_t buffer = 0;
	size_t line_size = 256;
	char** lines = (char**)malloc(sizeof(char*) * 256);
	//Initialize new room
	struct room* n_room; 
	initializeRoom(&n_room);
	
	//Store file as array
	while(getline(&lines[i], &line_size, f) > 0){
		num_lines++;
	}
	
	//assign room name;
	sscanf(lines[0], "%*s %*s %s", name);
	setName(game->currRoom, name);
	
	//assign room type;
	sscanf(lines[num_lines - 1], "%*s %*s %s", type);
	setType(game->currRoom, name);
	
	setNumOut(game->currRoom,(num_lines - 2));
	//for loop to parse outbound connections from line 2 onwards
	for(i = 0; i < n_room->numOutboundConnections; i++){
		sscanf(lines[i + 1], "%*s %*s %s", connection);
		addOutbound(game->currRoom, connection);
	}
	
	//Free array
	for(i = 0; i < num_lines; i++){
		free(lines[i]);
	}
	free(lines);
}

// Returns the room of a specific type. For setting the start/end room
struct room* findType (struct game* game, char* type){
	// Set variables
	FILE *f;
	int running = 1;
	//char directory[256];
	char file_name[256];
	size_t buffer = 64 * sizeof(char);
	struct stat st;
	struct dirent* dir_info;
	char* line = (char *) malloc(buffer);
	
	DIR* dir = opendir(game->directory);
	
	// Read the first file (Copy the first file's name into char* file_name)
	do{
        dir_info = readdir(dir);
		printf("CHANBE.ROOMS: %s\n", dir_info->d_name);
    } while (strstr(dir_info->d_name, "chanbe.rooms.") == NULL);
	
	strcpy(file_name, dir_info->d_name);
	//open the first file
	f = fopen(file_name, "r");
	printf("CHANBE.ROOMS: %s\n", dir_info->d_name);
	
	while (running) {
		//Search through file line by line until NULL
		while (getline(&line, &buffer, f) != -1) {
			//Look for line with substring matching type
			if (strstr(line, type) != NULL) {
				//This file contains the correct room.
				//Parse and return this room.
				free(line);
				return parseRoom(f, game);
			}
		//Reached end of file.
		}
		fclose(f);
		//Iterate to next file.
		dir_info = readdir(dir);
		strcpy(file_name, dir_info->d_name);
		f = fopen(file_name, "r");		
	}
	fclose(f);
	printf("NO %s ROOM FOUND. TERMINATING.\n", type);
	free(line);
	return NULL;
}

// Returns the room of a specific name. For use with traversing to outbound rooms
struct room* findName (struct game* game, char* name){
	// Set variables
	FILE *f;
	int running = 1;
	//char directory[256];
	char file_name[256];
	char line[256];
	struct stat st;
	struct dirent* dir_info;
	// Set directory
	//sprintf(directory, "%s/", game->directory);
	DIR* dir = opendir(game->directory);
	// Read the first file (Copy the first file's name into char* file_name)
	dir_info = readdir(dir);
	strcpy(file_name, dir_info->d_name);
	//open the first file
	f = fopen(file_name, "r");
	
	while (running) {
		//Search through file line by line until NULL
		while (fgets(line, sizeof(line), f)) {
			//Look for line with substring matching name
			if (strstr(line, name) != NULL) {
				//This file contains the correct room.
				//Parse and return this room.
				return parseRoom(f, game);
			}
		//Reached end of file.
		}
		fclose(f);
		//Iterate to next file.
		dir_info = readdir(dir);
		strcpy(file_name, dir_info->d_name);
		f = fopen(file_name, "r");
	}
	printf("NO START ROOM FOUND. TERMINATING.\n");
	return NULL;
}

//Does printouts, and asks for users input.
int turn(struct game* game){
	int i;
	int running = 1;
	size_t buffer = 0;
	char* n_name = (char*)malloc(64 * sizeof(char));
	char* selection = (char*)malloc(64 * sizeof(char));
	char* lineEntered = NULL;
	
	//Print initial information
	printf(" - Running turn().\n");
	
	printf("CURRENT LOCATION: %s\nPOSSIBLE CONNECTIONS: ", getName(game->currRoom));
	for (i = 0; i < getNumOut(game->currRoom); i++){
		printf("%s", getName(game->currRoom));
		//Print comma, unless in last spot
		if(i < (getNumOut(game->currRoom) - 1)){
			printf(", ");
		}
	}
	printf(".\nWHERE TO? >");
	//User input
	
	printf("\n\n");
	//Loop until valid input
	while (running > 0){
		getline(&lineEntered, &buffer, stdin);
		if (strcmp(lineEntered, "time")){
			// Return the time
			printf("Here's the time.\n");
			
			// Ask for another input
		} else {
			for (i=0; i < getNumOut(game->currRoom); i++){
				// Loop through connecting rooms to find matching name
				if (strcmp(lineEntered, getOutbound(game->currRoom, i))){
					//Found the correct room
					//Assign the 'found' room as the currRoom
					assignRoom(game->currRoom, findName(game, lineEntered));
					//Break out of loop, since room has already been found.
					break;
				}
			}
			// Didn't match any room names or 'time' function.
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}
	}
	printf("\n");
}

//Main Function
int main(){
	//Initialize variables and objects.
	int running = 1;
	struct game* game;
	printf("Initialized game object.\n");
	
	printf("Initializing game values.\n");
	initializeGame(&game);
	
	if (game->directory == NULL || game->directory == 0){
		printf("NO COMPATIBLE ROOMS FOUND. GAME TERMINATING.\n");
	} else {
		//Directory successfully found. Game continuing.
		printf("Directory found and set. Game sequence starting.\n");

		//Parse info to set start room
		printf("Searching for start room.\n");
		findType(game, "START_ROOM");
		
		//Game runs until currRoom has type END_ROOM
		while (running > 0){
			printf("Game loop %d.\n", getTurn(game));
			running = turn(game);
			if (running != 0) game->turnCount++;
		}
		//Game over, prints turncount and path taken
		printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: %s\n", getTurn(game), getPath(game));
	}
	//Terminates
	freeGame(game);
	return 0;
}
