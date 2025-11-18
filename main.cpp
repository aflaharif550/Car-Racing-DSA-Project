#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <chrono>
#include <thread>
#include <queue>
#include<iomanip>
#include<fstream>
#include<string>
#include"header.h"
using namespace std;

// ANSI escape codes for text color
#define RESET "\033[0m"
#define CYAN "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RED "\033[1;31m"  
#define AQUA "\033[36m" 
#define GOLD "\033[38;5;220m" // ANSI escape code for gold color
#define NAVY_BLUE "\033[38;5;18m" // ANSI escape code for navy blue color
#define VIOLET "\033[38;5;92m" // ANSI escape code for violet color
#define CRIMSON "\033[38;5;196m"
#define GRAY "\033[38;5;245m" 

int main() {
    int rows = 20; // Increased the size of the map
    int cols = 20; // Increased the size of the map

    // Create the game graph
    Graph gameGraph(rows, cols);

    // Print the initial map and edges (for debugging purposes)
    // Display the menu
    displayMenu(gameGraph);

    return 0;
}