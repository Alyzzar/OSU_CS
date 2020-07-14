/**

Written by Ben Chan for CS 344

**/

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CONNECTIONS 6
#define TOT_ROOMS 7

/**
	Struct for room type
		Detail:	Struct used to construct the room. Contains information 
				concerning room identity, and connections to other rooms.
		Vars: 	char* name, char* type, int numOutboundConnections, char* outboundConnections[]
**/
struct room { 
	char* name;
	char* type;
	int numOutboundConnections;
	char* outboundConnections[MAX_CONNECTIONS];
};

/**
	Struct for game type
		Detail:	struct used to construct the game. Contains information
				concerning game turns, file directory, and current room location.
		Vars:	int turnCount, char* path, char* directory, struct currRoom
**/
struct game {
	unsigned int turnCount;
	char* path;
	char* directory;
	struct room* currRoom;
};

/**-------------------------------------------------------------------
						GETTERS/SETTERS
-------------------------------------------------------------------**/

/**
	Function getName()
		Params:	struct room
		Return:	char* name
**/
char* getName(struct room* room){
	return room->name;
}

/**
	Function setName()
		Params: struct room, char* name
**/
void setName(struct room* room, char* name){
	//Allocate new memory
	free(room->name);
	room->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(room->name, name);
	//Add null terminator
	room->name[strlen(name)] = '\0';
}

/**
	Function getType()
		Params: struct room
		Return: char* type (start, middle, or end room)
**/
char* getType(struct room* room){
	return room->type;
}

/**
	Function setType()
		Params: struct room, char* type
**/
void setType(struct room* room, char* type){
	//Allocate new memory
	free(room->type);
	room->type = (char*)malloc(sizeof(char) * (strlen(type) + 1));
	strcpy(room->type, type);
	//Add null terminator
	room->type[strlen(type)] = '\0';
}

/**
	Function getNumOut()
		Detail:	Returns the number of outbound connections to other rooms
		Params:	struct room
		Return:	int numOutboundConnections
**/
int getNumOut(struct room* room){
	return room->numOutboundConnections;
}

/**
	Function setNumOut()
		Detail:	Sets the number of outbound rooms coming out of this room.
		Params:	struct room, int num
**/
void setNumOut(struct room* room, int num){
	room->numOutboundConnections = num;
}

/**
	Function getOutbound()
		Detail:	Returns the name of the 'n'th room on the outboundConnections[]
				array of the given room.
		Params: struct room, int n
		Return: char* name of outboundConnections[n]
**/
char* getOutbound(struct room* room, int n){
	return room->outboundConnections[n];
}

/**
	Function getPath()
		Params:	struct game
		Return:	char* path taken (pre-formatted with '\n' in between rooms)
**/
char* getPath(struct game* game){
	return game->path;
}

/**-------------------------------------------------------------------
						END OF GETTERS/SETTERS
-------------------------------------------------------------------**/

/**
	Function addOutbound()
		Detail:	Adds the input char* to the array of outbound nodes connected to the given room
		Params:	struct room, char* connecting room (name only)
**/
void addOutbound(struct room* room, char* connection){
	int index = getNumOut(room);
	//printf("addOutbound( %s, %s) at index %d\n", getName(room), connection, index);
	room->outboundConnections[index] = (char*)malloc(sizeof(char) * (strlen(connection) + 1));
	strcpy(room->outboundConnections[index], connection);
	room->outboundConnections[index][strlen(connection)] = '\0';
	room->numOutboundConnections++;
}

/**
	Function resetOutbound()
		Detail:	Frees all values in the outboundConnections array of the given room
		Params:	struct room
**/
void resetOutbound(struct room* room){
	int i = 0;
	for (i = 0; i < getNumOut(room); i++){
		free(room->outboundConnections[i]);
	}
	setNumOut(room, 0);
}

/**
	Function setDir()
		Detail:	Automatically finds and sets the file directory to the newest folder with the correct prefix.
				Part of the initialization process
		Params:	struct game
		Return:	boolean value (1 on success, 0 on fail)
**/
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
			// If directory has prefix (chanbe.rooms.*)
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL){
				//Matching prefix found
				stat(fileInDir->d_name, &dirAttributes);
				if ((int)dirAttributes.st_mtime > newestDirTime){
					//Save as the newest folder
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
		//Allocate new memory for game->directory string
		game->directory = (char*)malloc(sizeof(char) * (strlen(newestDirName) + 1));
		strcpy(game->directory, newestDirName);
		//Add null terminator
		game->directory[strlen(newestDirName)] = '\0';
		//free(dirToCheck);
		//Return 1 if directory was found.
		return 1;
	}
	//Return 0 if directory could not be found.
	return 0;
}

/**
	Function addPath()
		Detail:	Adds the name of a room to the end of the list of rooms, tracked in game->path
		Params:	struct game, struct room
**/
void addPath(struct game* game, struct room* room){
	int n_strlen;
	char old_path [256];
	//Check if game->path is NULL
	if(getPath(game) == NULL){
			//Malloc space for the char*, then copy the name of the room into the newly malloc'd char*
			n_strlen = strlen(getName(room));
			game->path = (char*)malloc(sizeof(char) * ( n_strlen + 1));
			strcpy(game->path, getName(room));
	} else {
		//If not NULL, copy it's contents.
		strcpy(old_path, getPath(game));
		//n_strlen is the string length of both the old path, and the new room
		n_strlen = strlen(old_path) + strlen(getName(room));
		//Free the old game->path
		free(game->path);
		//Malloc the new memory using the new string length, +2 chars for '\n' and '\0' characters
		game->path = (char*)malloc(sizeof(char) * (n_strlen + 2));
		//Assign new value
		sprintf(game->path, "%s\n%s", old_path, getName(room));
	}
	game->path[n_strlen + 1] = '\0';
}

/**
	Function getTurn()
		Params:	struct game
		Return:	int turnCount
**/
int getTurn (struct game* game){
	return game->turnCount;
}

/**
	Function initializeRoom()
		Detail:	Initializer for struct room. Allocates memory for all variables
		Params:	struct room pointer
**/
void initializeRoom(struct room** room){
	*room = (struct room*)malloc(sizeof(struct room));
	(*room)->name = (char*)malloc(sizeof(char));
	(*room)->type = (char*)malloc(sizeof(char));
	setNumOut(*room, 0);
}

/**
	Function freeRoom()
		Detail: Deallocates any memory used in the room
		Params:	struct room
**/
void freeRoom(struct room* room){
	free(room->name);
	free(room->type);
	resetOutbound(room);
}

/**
	Function initializeGame()
		Detail: Initializer for struct game. Allocates memory for game variables, 
				and initializes the starting room.
		Params:	struct game
**/
void initializeGame(struct game** game){
	*game = (struct game*)malloc(sizeof(struct game));
	(*game)->turnCount = 0;
	//Initialize path with length of 1
	(*game)->path = NULL;
	initializeRoom(&(*game)->currRoom);
	//Initialize directory
	setDir(*game);
}

/**
	Function freeGame()
		Detail:	Deallocates any memory used in the game
		Params:	struct game
**/
void freeGame(struct game* game){
	//free(game->turnCount);
	free(game->path);
	free(game->directory);
	freeRoom(game->currRoom);
	free(game->currRoom);
	free(game);
}

/**
	Function parseRoom()
		Detail:	Reads a given file f, parses information from it into the current room.
				This is what moves the player from one room into the next.
		Params:	struct game
**/
void parseRoom(FILE* f, struct game* game){
	int i;
	int num_lines = 0;
	char name [64];
	char type [64];
	char* connection = (char*)malloc(64 * sizeof(char));
	size_t line_size = 32;
	char** lines = (char**)malloc(sizeof(char*) * 32);
	
	for(i = 0; i < 10; i++){
		lines[i] = NULL;
	}
	
	// Reset file pointer, and loop to store file as array
	fseek(f, 0, SEEK_SET);
	while(getline(&lines[num_lines], &line_size, f) > 0){
		num_lines++;
	}
	
	//printf(" - PARSING ROOM\n");
	// Assign room name;
	sscanf(lines[0], "%*s %*s %s", name);
	setName(game->currRoom, name);
	//printf(" - - Room name set: %s\n", name);
	
	// Assign room type;
	sscanf(lines[num_lines - 1], "%*s %*s %s", type);
	setType(game->currRoom, type);
	//printf(" - - Room type set: %s\n", type);
	
	// Reset outbound connections
	resetOutbound(game->currRoom);
	
	//for loop to parse outbound connections from line 2 onwards
	for(i = 0; i < (num_lines - 2); i++){
		sscanf(lines[i + 1], "%*s %*s %s", connection);
		//printf(" - - Parsed outbound: %s\n", connection);
		addOutbound(game->currRoom, connection);
	}
		
	// Add room name to path taken
	addPath(game, game->currRoom);
	
	// Free the lines[] array
	for(i = 0; i < num_lines; i++){
		free(lines[i]);
	}
	free(lines);
	free(connection);
}

/**
	Function findType()
		Detail:	Iterates through all files and lines to locate a specific string (char*).
				This is primarily used to locate the starting room.
		Params:	struct game, char* c
		Return:	boolean (1 on success, 0 of fail)
**/
int findType (struct game* game, const char* c){
	// Set variables
	FILE *f;
	int i, j, numChars;
	//char directory[256];
	char file_name[256];
	char file_path[256];
	size_t buffer = 64 * sizeof(char);
	struct stat st;
	struct dirent* dir_info;
	char* line = (char *) malloc(buffer);
	
	// Iterate to the first dir/file
	DIR* dir = opendir(game->directory);
	dir_info = readdir(dir);
	strcpy(file_name, dir_info->d_name);
	
	//open the first file
	i = 0;
	while (i < TOT_ROOMS) {
		//Search through file line by line until NULL
		if(strstr(dir_info->d_name, "_ROOM") != NULL){
			//Found file
			sprintf(file_path, "%s/%s", game->directory, file_name);
			f = fopen(file_path, "r");
			//Successfully opened the file
			j = 1;
			while (getline(&line, &buffer, f) != -1 && (j < MAX_CONNECTIONS + 3)) {
				line[strlen(line) - 1] = '\0';
				//Look for line with substring matching c
				if (strstr(line, c) != NULL) {
					//This file contains the correct room. Parse it.
					free(line);
					parseRoom(f, game);
					closedir(dir);
					return 1;
				}
				//If the string wasn't found, iterate to the next line
				j++;
			}
			fclose(f);
			//'i' keeps track of how many rooms have been checked
			i++;
		}
		//If the string wasn't found, iterate to the next file.
		dir_info = readdir(dir);
		sprintf(file_name, dir_info->d_name);
	}
	//Free variables and close directory. Return 0, since string wasn't found
	free(line);
	closedir(dir);
	return 0;
}

/**
	Function findName()
		Detail:	Iterates through all files (first line only) to locate a specific name (char*).
				This is used to set the new currRoom.
		Params:	struct game, char* c
		Return:	boolean (1 on success, 0 of fail)
**/
int findName (struct game* game, char* c){
	// Set variables
	FILE *f;
	int i, j, numChars;
	//char directory[256];
	char file_name[256];
	char file_path[256];
	struct stat st;
	struct dirent* dir_info;
	
	// Iterate to the first dir/file
	DIR* dir = opendir(game->directory);
	dir_info = readdir(dir);
	strcpy(file_name, dir_info->d_name);
	
	i = 0;
	while (i < TOT_ROOMS) {
		//printf(" - - FILE: %s\n", file_name);
		if(strstr(dir_info->d_name, "_ROOM") != NULL){
			// Copy file name into variable
			//printf(" - - - Comparing file name.\n");
			file_name[strlen(file_name) - 5] = '\0';
			//printf(" - - - - FILE: %s, TARGET: %s.\n", file_name, name);
			// Compare file name
			if(strcmp(file_name, c) == 0) {
				//printf(" - - - - TARGET FOUND. GENERATING FILE PATH:\n");
				// File has matching string.
				// Create file path
				sprintf(file_path, "%s/%s_ROOM", game->directory, file_name);
				//printf(" %s\n",file_path);
				f = fopen(file_path, "r");
				parseRoom(f, game);
				//fclose(f)
				closedir(dir);
				return 1;
			}
		}
		//Iterate to next file.
		dir_info = readdir(dir);
		strcpy(file_name, dir_info->d_name);
		sprintf(file_name, dir_info->d_name);
	}
	closedir(dir);
	return 0;
}

/**
	Function turn()
		Detail:	Primary game loop. Asks for user inputs, and calls functions in response.
		Params:	struct game
		Return:	boolean (1 on success, 0 of fail)
**/
int turn(struct game* game){
	int i;
	int running = 1;
	size_t buffer = 0;
	char* lineEntered = NULL;
	//Print initial information
	//Check if currRoom is the END
	if(strcmp(getType(game->currRoom), "END_ROOM") == 0){
		free(lineEntered);
		return 0;
	}
	
	printf("CURRENT LOCATION: %s\nPOSSIBLE CONNECTIONS: ", getName(game->currRoom));
	for (i = 0; i < getNumOut(game->currRoom); i++){
		printf("%s", getOutbound(game->currRoom, i));
		//Print comma, unless in last spot
		if(i < (getNumOut(game->currRoom) - 1)){
			printf(", ");
		}
	}
	printf(".\n");
	while (running > 0){
		printf("WHERE TO? >");
		getline(&lineEntered, &buffer, stdin);
		//Remove '\n' character using '\0'
		lineEntered[strlen(lineEntered)-1] = '\0';
		
		printf("\n");
		for (i=0; i < getNumOut(game->currRoom); i++){
			// Loop through connecting rooms to find matching name
			if (strcmp(lineEntered, getOutbound(game->currRoom, i)) == 0){
				//Found the correct room
				//Assign the 'found' room as the currRoom
				if (findName(game, lineEntered) != 1){
					printf("NO MATCHING ROOM FOUND. GAME TERMINATING.\n\n");
					free(lineEntered);
					return 0;
				}
				//Break out of loop, since room has already been found.
				printf("\n");
				free(lineEntered);
				return 1;
			}
		}
		// Didn't match any room names.
		printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
	}
}

/**
	Function main()
		Detail:	Initializes game and confirms that game files exists. If all files are present, runs game loop
				and increments the number of turns taken.
		Params: void
		Return:	0 on exit
**/
int main(void){
	
	//Initialize variables and objects.
	int running = 1;
	struct game* game;
	//printf("Initialized game object.\n");
	
	//printf("Initializing game values.\n");
	initializeGame(&game);
	
	if (game->directory == NULL || game->directory == 0){
		printf("NO COMPATIBLE ROOMS FOUND. GAME TERMINATING.\n");
		return 0;
	}
	//Directory successfully found. Game continuing.
	//Parse info to set start room	
	if (findType(game, "START_ROOM") == 0){
		printf("NO START ROOM FOUND. GAME TERMINATING.\n");
		return 0;
	}
	//Game runs until currRoom has type END_ROOM
	while (running > 0){
		//Game loop
		running = turn(game);
		//Increment turn count
		if (running != 0) game->turnCount++;
	}
	//Game over, prints turncount and path taken
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n%s\n\n", getTurn(game), getPath(game));
	//Terminates
	freeGame(game);
	return 0;
}
