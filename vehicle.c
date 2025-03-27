#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vehicle.h"

// Custom memory copy function to replace memcpy
void custom_copy(char* dest, const char* src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}
/*
Purpose: Dynamically allocates a string to store a vehicle's information. Specifically, the value and model year
are stored in the first 4 bytes. The remaining memory is for the description of the vehicle.
Parameters : None
Returns: a dynamically allocated block of memory containing the vehicle's information
*/
char* createVehicle() {
    // Temporary buffer for description
    char temp_description[100];

    // Get vehicle description from user
    printf("Enter vehicle description: ");
    fgets(temp_description, sizeof(temp_description), stdin);

    // Remove newline if present
    size_t len = strlen(temp_description);
    if (len > 0 && temp_description[len - 1] == '\n') {
        temp_description[len - 1] = '\0';
        len--;
    }

    // Get vehicle value (unsigned dollar amount)
    unsigned int value;
    printf("Enter vehicle value (no cents): ");
    scanf_s("%u", &value);
    getchar();  // Clear input stream

    // Ensure value is within 21-bit range (0 to 2,097,151)
    value &= 0x1FFFFF;

    // Get model year
    unsigned int year;
    printf("Enter model year: ");
    scanf_s("%u", &year);
    getchar();  // Clear input stream

    // Ensure year is within 11-bit range (0 to 2047)
    year &= 0x7FF;

    // Combine value and year into a single 32-bit integer using bit operations
    unsigned int combined = (value << 11) | year;

    // Allocate exact memory for vehicle 
    // 4 bytes for value and year + description length + null terminator
    char* vehicle = malloc(4 + len + 1);

    // Copy combined value and year manually to first 4 bytes
    custom_copy(vehicle, (char*)&combined, 4);

    // Copy description after the first 4 bytes manually
    for (size_t i = 0; i <= len; i++) {
        vehicle[4 + i] = temp_description[i];
    }

    return vehicle;
}

/*
Purpose: Display the information stored in a vehicle
Parameters: char* representing all information stored for a vehicle
Returns: Nothing
*/
void displayVehicle(char* vehicle) {
    //Extracts combined value and year
    unsigned int combined;
    custom_copy((char*)&combined, vehicle, 4);

    //Extracts value (upper 21 bits)
    unsigned int value = combined >> 11;

    //Extracts year (lower 11 bits)
    unsigned int year = combined & 0x7FF;

    //Prints vehicle details
    printf("Description: %s\n", vehicle + 4);
    printf("Value: $%u\n", value);
    printf("Model Year: %u\n\n", year);
}


/*
Purpose: Store multiple vehicles in an array. Dynamically allocates an array of char pointers.
Calls the createVehicle method for each vehicle to be created
Parameters: int - Number of vehicles to add to the array
Returns: a dynamically allocated array of char*'s, each pointer representing a vehicle
*/
char** createGarage(int numVehicles) {
    char** garage = (char**)malloc(numVehicles * sizeof(char*)); //this code will allocate memory for array of vehicle  pointers
    if (!garage) {
        printf("Memory allocation failed for garage.\n");
        exit(1);
    }

    for (int i = 0; i < numVehicles; i++) {
        printf("\nCreating vehicle %d:\n", i + 1);
        garage[i] = createVehicle(); //Create a new vehicle and store in garage
    }
    return garage; //This will Return pointer to allocated array
}

/*
Purpose: Display all the information stored for each vehicle in the garage. Calls
displayVehicle for each vehicle in the garage.
Parameters: char** - pointer to the garage
int - number of vehicles in the garage
Returns: Nothing.
*/
void displayGarage(char** garage, int numVehicles) {
    if (!garage) {
        printf("Garage is empty.\n");
        return; // Exit if garage is empty
    }

    for (int i = 0; i < numVehicles; i++) {
        printf("\nVehicle %d:\n", i + 1);
        displayVehicle(garage[i]); //Display details of each vehicle
    }
}

/*
Purpose: Remove a vehicle from the garage. Returns a new dynamically allocated garage with remaining cars stored in it.
Note that this function should free the garage passed in.
Parameters:  char** - pointer to current garage
int - number of vehicles in current garage
int - index of the vehicle to remove (0 based)
Returns: a reference to the new dynamically allocated garage
*/
char** removeVehicle(char** garage, int numVehicles, int index) {
    if (index < 0 || index >= numVehicles) {
        printf("Invalid index. No vehicle removed.\n");
        return garage; //Returns the  original garage if index is out of bounds
    }

    //this code will free memory of the vehicle being removed
    free(garage[index]);

    // Allocate new garage with one less vehicle
    char** newGarage = (char**)malloc((numVehicles - 1) * sizeof(char*));
    if (!newGarage) {
        printf("Memory allocation failed for new garage.\n");
        exit(1);
    }

    //Copy over all vehicles except the removed one
    for (int i = 0, j = 0; i < numVehicles; i++) {
        if (i != index) {
            newGarage[j++] = garage[i]; //Copy valid vehicles to new array
        }
    }

    //Free old garage memory
    free(garage);

    return newGarage; //Return the updated garage
}


// Test function for garage operations
void testGarage() {
    // Number of vehicles to add to the garage
    int numVehicles;

    // Prompt the user to enter how many vehicles to store
    printf("Enter number of vehicles you would like to store: ");
    scanf_s("%d", &numVehicles);
    getchar();  // Clear input stream

    // Create a new garage
    char** garage = createGarage(numVehicles);

    // Display the contents of the garage
    displayGarage(garage, numVehicles);

    // Remove a vehicle
    garage = removeVehicle(garage, numVehicles--, 1);

    // Display the contents of the garage
    displayGarage(garage, numVehicles);

    // Free remaining vehicles and garage
    for (int i = 0; i < numVehicles; i++) {
        free(garage[i]);
    }
    free(garage);
}
