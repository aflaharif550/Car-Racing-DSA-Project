#include<iostream>
#include<cstdlib>
#include<ctime>
#include<conio.h>
#include<chrono>
#include<thread>
#include<queue>
#include<iomanip>
#include<fstream>
#include<string>

using namespace std;

// ANSI escape codes for text color
#define RESET "\033[0m"
#define CYAN "\033[31m"
#define GREEN "\033[32m"
#define IMPERIAL_RED "\033[38;2;176;32;82m"
#define ORANGE "\033[38;2;255;165;0m"
#define DARK_BLUE "\033[0;34m"
#define LIGHT_CYAN "\033[1;36m"
#define MINT_CREAM "\033[38;5;121m"
#define MAGENTA "\033[1;35m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define TAN "\033[0;33m"
#define RED "\033[1;31m"  
#define TEAL "\033[1;36m"
#define AQUA "\033[36m" 
#define GOLD "\033[38;5;220m" // ANSI escape code for gold color
#define NAVY_BLUE "\033[38;5;18m" // ANSI escape code for navy blue color
#define VIOLET "\033[38;5;92m" // ANSI escape code for violet color
#define CRIMSON "\033[38;5;196m"
#define GRAY "\033[38;5;245m" 
// Node structure representing each location, power-up, or obstacle




void write_to_leaderboard(const string& playerName, int score) {
    ofstream outputFile("leaderboard.txt", ios::app);

    if (outputFile.is_open()) {

        // Write format: Name Score
        outputFile << playerName << " " << score << endl;

        outputFile.close();
        cout << "Successfully added " << playerName 
             << " with score " << score 
             << " to the leaderboard." << endl;
    }
    else {
        cerr << "ERROR: Unable to open leaderboard.txt for writing." << endl;
    }
}

void read_leaderboard() {
    // std:: becomes unnecessary:
    ifstream inputFile("leaderboard.txt");

    if (inputFile.is_open()) {
        string line;
        
        cout << "\n--- Current Leaderboard ---" << endl;

        while (getline(inputFile, line)) {
            size_t separatorPos = line.find(':');

            if (separatorPos != string::npos) {
                string playerName = line.substr(0, separatorPos);
                string scoreStr = line.substr(separatorPos + 1);
                
                double score = 0.0;
                try {
                    // std:: becomes unnecessary:
                    istringstream ss(scoreStr);
                    ss >> score;
                    
                    cout << "Name: " << playerName << " | Score: " << score << endl;

                } catch (const exception& e) {
                    cerr << "Warning: Failed to parse score for line: " << line << endl;
                    cout << line << endl; 
                }
            } else {
                cerr << "Warning: Skipping malformed line in file: " << line << endl;
            }
        }

        inputFile.close();
        cout << "---------------------------\n" << endl;

    } else {
        cerr << "ERROR: Unable to open leaderboard.txt for reading. File may not exist yet." << endl;
    }

   cout << CYAN << "Press any key to return to the menu..." << RESET << endl;
    _getch(); // Wait for a key press before returning to the menu
}

struct Node 
{
    char type; // 'L' for location, 'P' for power-up, '@' for obstacle, '>' for car
    int value; // Value associated with the node (2 for power-up, -5 for obstacle, 0 for location)
    bool visited; // Flag to mark whether the node has been visited during pathfinding
};

// Edge structure representing a path between two nodes
struct Edge 
{
    pair<int, int> start; // Starting position of the path
    pair<int, int> end;   // Ending position of the path
};

struct NodeAVL {
    string name;
    int score;
    int height;

    NodeAVL* left;
    NodeAVL* right;

    NodeAVL(string n, int s)
        : name(n), score(s), height(1), left(nullptr), right(nullptr) {}
};


class AVLTree {
private:
    NodeAVL* root;

    int getHeight(NodeAVL* node) {
        return node ? node->height : 0;
    }

    int getBalance(NodeAVL* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    NodeAVL* rightRotate(NodeAVL* y) {
        NodeAVL* x = y->left;
        NodeAVL* T2 = x->right;

        // rotation
        x->right = y;
        y->left = T2;

        // update heights
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x; // new root
    }

    NodeAVL* leftRotate(NodeAVL* x) {
        NodeAVL* y = x->right;
        NodeAVL* T2 = y->left;

        // rotation
        y->left = x;
        x->right = T2;

        // update heights
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y; // new root
    }

    NodeAVL* insert(NodeAVL* node, string name, int score) {
        if (!node)
            return new NodeAVL(name, score);

        if (score < node->score)
            node->left = insert(node->left, name, score);
        else
            node->right = insert(node->right, name, score);

        // update height
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        int balance = getBalance(node);

        // Left Left
        if (balance > 1 && score < node->left->score)
            return rightRotate(node);

        // Right Right
        if (balance < -1 && score > node->right->score)
            return leftRotate(node);

        // Left Right
        if (balance > 1 && score > node->left->score) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right Left
        if (balance < -1 && score < node->right->score) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node; // unchanged
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(string name, int score) {
        root = insert(root, name, score);
    }

    // -----------------------------------------
    //      Read from file and insert
    // -----------------------------------------
    void loadFromFile_simple(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file\n";
        return;
    }

    string name;
    int score;
    while (file >> name >> score) {
        insert(name, score);
    }
}
   // -----------------------------------------
    //        Inorder Print (ranking)
    // -----------------------------------------
    void printInOrder(NodeAVL* node) {
        if (!node) return;
        printInOrder(node->right); // highest score first
        cout << node->name << " - " << node->score << endl;
        printInOrder(node->left);
    }

    void printRanking() {
        printInOrder(root);
    }

    NodeAVL* searchByScore(NodeAVL* node, int score) {
    if (!node) return nullptr;

    if (score == node->score)
        return node;

    else if (score < node->score)
        return searchByScore(node->left, score);

    else
        return searchByScore(node->right, score);
}

string getNameByScore(int score) {
    NodeAVL* result = searchByScore(root, score);
    if (result)
        return result->name;

    return "";  // not found
}

};

void AVLMenu() {
    AVLTree AVLleaderboard;
    AVLleaderboard.loadFromFile_simple("leaderboard.txt");

    while (true) {
        system("cls");
        cout << CYAN << "=== Leaderboard Menu ===" << RESET << endl;
        cout << "1. Show Ranking" << endl;
        cout << "2. Search Player by Score" << endl;
        cout << "3. Back to Main Menu" << endl;
        cout << "\nChoose an option: ";

        char sub;
        cin >> sub;

        switch (sub) {
        case '1': {
            system("cls");
            cout << GREEN << "=== Ranking ===" << RESET << endl;
            AVLleaderboard.printRanking();
            cout << "\n" << CYAN << "Press any key to continue..." << RESET << endl;
            _getch();
            break;
        }

        case '2': {
            system("cls");
            cout << YELLOW << "Enter Score to Search: " << RESET;
            int score;
            cin >> score;

            string name = AVLleaderboard.getNameByScore(score);

            if (name.empty()) {
                cout << RED << "No player found with that score!" << RESET << endl;
            } else {
                cout << GREEN << "Player: " << name << RESET << endl;
            }

            cout << "\n" << CYAN << "Press any key to continue..." << RESET << endl;
            _getch();
            
            break;
        }

        case '3':
            return; // go back to main menu

        default:
            cout << RED << "Invalid option!" << RESET << endl;
            _getch();
        }
    }
}

struct Obstacle {
    int row;
    int col;
};


class Coin 
{
public:
    int value;
    int row;
    int col;
    Coin* next;

    Coin(int val, int r, int c) 
    {
        value = val;
        row = r;
        col = c;
        next = nullptr;

    }
};

// LinkedListForCoins class
class LinkedListForCoins 
{
public:
    Coin* head;
    int totalValue;

    LinkedListForCoins() 
    {
        head = nullptr;
        totalValue = 0; 
    }

    // Add a coin to the linked list
    void addCoin(int value, int row, int col) 
    {
        Coin* newCoin = new Coin(value, row, col);
        newCoin->next = head;
        head = newCoin;
        totalValue += value;
    }

    // Display and collect coins
    int collectCoins(int carRow, int carCol) 
    {
        int Total_Colledted_Values = 0;

        Coin* curr = head;
        Coin* prev = nullptr;

        while (curr != nullptr) 
        {
            if (curr->row == carRow && curr->col == carCol) 
            {
                // Car has collected the coin
                Total_Colledted_Values += curr->value;

                // Remove the coin from the linked list
                if (prev != nullptr) 
                {
                    prev->next = curr->next;
                    delete curr;
                    curr = prev->next;
                }
                else 
                {
                    head = curr->next;
                    delete curr;
                    curr = head;
                }
            }
            else 
            {
                prev = curr;
                curr = curr->next;
            }
        }

        totalValue -= Total_Colledted_Values;
        return Total_Colledted_Values;
    }

    // Display collected coins
    void displayCollectedCoins() 
    {
        cout << GOLD << "Coins Collected: " << totalValue << RESET << endl;
    }

    // Destructor to free memory
    ~LinkedListForCoins() 
    {
        Coin* curr = head;
        while (curr != nullptr) {
            Coin* temp = curr;
            curr = curr->next;
            delete temp;
        }
        head = nullptr;
    }
};


class SimpleQueueForObstacles 
{
private:
    struct Node 
    {
        Obstacle data;
        Node* next;
        Node(Obstacle d) 
        {
            data = d; 
            next = nullptr; 
        }
    };

    Node* front;
    Node* rear;

public:
    SimpleQueueForObstacles()
    {
        front = nullptr;
        rear = nullptr;

    }

    void enqueue(Obstacle obstacle) 
    {
        Node* newObstacle = new Node(obstacle);
        if (rear == nullptr) 
        {
            front = rear = newObstacle;
        }
        else {
            rear->next = newObstacle;
            rear = newObstacle;
        }
    }

    
};


class Graph {
private:
    SimpleQueueForObstacles queueObstacles;
public:
    string carColor;
    string mapColor;

    static const int MAX_ROWS = 20; // Increased to accommodate the boundary
    static const int MAX_COLS = 15; // Increased to accommodate the boundary
    Node nodes[MAX_ROWS][MAX_COLS];
    Edge edges[MAX_ROWS * MAX_COLS * 2]; // Max edges is rows * (cols - 1) + (rows - 1) * cols for a grid graph
    int Rows, Col;
    
    int powerUpsCollected; // Added to track power-ups collected
    int distanceCovered; // Added to track distance covered
    int obstacleMultiplier; // Added to track obstacle multiplier
    int powerUpMultiplier; // Added to track power-up multiplier
    double timeTaken; // Added to track time taken

    // Constructor to initialize the graph with a maze
    Graph(int rows = 0, int cols = 0) : Rows(rows), Col(cols) {
        generateMaze();
        generateEdges();
        generateBoundary();
    }


    void generateMaze() 
    {
        srand(time(0));
        queue<Obstacle> obstacleQueue;

        // Fill the maze with walls initially
        for (int i = 0; i < Rows; ++i) 
        { // Changed to <= to accommodate the boundary
            for (int j = 0; j < Col; ++j) 
            { // Changed to <= to accommodate the boundary
                nodes[i][j] = { '-', 0 }; // Wall
                nodes[i][j].visited = false; // Reset the visited flag
            }
        }

        // Create a maze with joint walls
        for (int i = 1; i < Rows - 1; ++i) 
        {
            for (int j = 1; j < Col - 1; ++j) 
            {
                // Place walls at even positions
                if (i % 2 == 0 && j % 2 == 0) { // Changed to % 2 == 0 to accommodate the boundary
                    nodes[i][j] = { '*', 0 }; // Joint wall
                }
                else 
                {
                    nodes[i][j] = { '-', 0 }; // Open path
                }
            }
        }

        // Add additional paths, power-ups, and obstacles
        for (int i = 1; i < Rows - 1; ++i) 
        {
            for (int j = 1; j < Col - 1; ++j) 
            {
                if (nodes[i][j].type == '-') 
                {
                    int randNum = rand() % 10;
                    // Decrease the probability of power-ups and obstacles
                    if (randNum < 1) {
                        nodes[i][j] = { 'P', 1 }; // Power-up
                    }
                    else if (randNum > 8) {
                        // Enqueue obstacle
                        obstacleQueue.push({ i, j });
                    }
                }
            }
        }

        // Process the obstacle queue
        while (!obstacleQueue.empty()) 
        {
            Obstacle obstacle = obstacleQueue.front();
            obstacleQueue.pop();
            nodes[obstacle.row][obstacle.col] = { '@', 1 }; // Obstacle
        }

        // Create joint walls
        for (int i = 0; i < Rows - 1; ++i) 
        {
            for (int j = 0; j < Col - 1; ++j) 
            {
                if (nodes[i][j].type == '*' && nodes[i + 1][j].type == '*' && nodes[i][j + 1].type == '*' && nodes[i + 1][j + 1].type == '*') 
                {
                    // Joint walls, leave a path through the joint
                    nodes[i][j] = { '-', 0 };

                    nodes[i + 1][j] = { '-', 0 };

                    nodes[i][j + 1] = { '-', 0 };

                    nodes[i + 1][j + 1] = { '-', 0 };
                }
            }
        }
    }

    // Function to generate edges based on the nodes
    void generateEdges() 
    {
        int Counting_edge = 0;
        for (int i = 1; i < Rows - 1; ++i) 
        {
            for (int j = 1; j < Col - 1; ++j) 
            {
                // Add horizontal edge
                edges[Counting_edge++] = { {i, j}, {i, j + 1} };
                // Add vertical edge
                edges[Counting_edge++] = { {i, j}, {i + 1, j} };
            }
        }
    }

    // Function to generate a boundary around the map
    void generateBoundary() {
        for (int i = 0; i < Rows; ++i) 
        {
            nodes[i][0] = { '*', 0 }; // Left boundary

            nodes[i][Col - 1] = { '*', 0 }; // Right boundary
        }
        for (int j = 0; j < Col; ++j) 
        {
            nodes[0][j] = { '*', 0 }; // Top boundary
            nodes[Rows - 1][j] = { '*', 0 }; // Bottom boundary
        }
    }

    // Function to find the shortest path using Dijkstra's algorithm
    void findShortestPath(pair<int, int> start, pair<int, int> end, pair<int, int> path[MAX_ROWS * MAX_COLS]) 
    {
        // Dijkstra's algorithm
        int dist[MAX_ROWS][MAX_COLS];
        pair<int, int> parent[MAX_ROWS][MAX_COLS];

        // Reset visited flags and initialize distances
        for (int i = 0; i < Rows; ++i) 
        {
            for (int j = 0; j < Col; ++j) 
            {
                nodes[i][j].visited = false;
                dist[i][j] = INT_MAX;
                parent[i][j] = { -1, -1 };
            }
        }

        dist[start.first][start.second] = 0;

        for (int count = 0; count < Rows * Col - 1; ++count) {
            int u = -1, v = -1;
            // Find the node with the minimum distance value
            int minDist = INT_MAX;
            for (int i = 0; i < Rows; ++i) {
                for (int j = 0; j < Col; ++j) {
                    if (!nodes[i][j].visited && dist[i][j] < minDist) {
                        minDist = dist[i][j];
                        u = i;
                        v = j;
                    }
                }
            }

            nodes[u][v].visited = true;

            for (int i = 0; i < Rows * (Col - 1) + (Rows - 1) * Col; ++i) 
            {
                if (edges[i].start == make_pair(u, v)) {
                    int neighborU = edges[i].end.first;
                    int neighborV = edges[i].end.second;
                    int neighborDist = dist[u][v] + 1; // Assuming each edge has a unit weight

                    if (!nodes[neighborU][neighborV].visited && neighborDist < dist[neighborU][neighborV] && nodes[neighborU][neighborV].type != '@') 
                    {

                        parent[neighborU][neighborV] = { u, v };
                        dist[neighborU][neighborV] = neighborDist;
                    }
                }
            }
        }
        // Reconstruct the path
        int pathIndex = 0;
        pair<int, int> current = end; // Start from the end
        while (current != start) {
            path[pathIndex++] = current;
            current = parent[current.first][current.second];
        }
        path[pathIndex++] = start;

        // Reverse the path
        for (int i = 0; i < pathIndex / 2; ++i) 
        {
            swap(path[i], path[pathIndex - i - 1]);
        }
    }

    void setCarColor() 
    {
        // Change color for '>'
        if (carColor == "blue") {
            cout << DARK_BLUE;
        }
        else if (carColor == "red") {
            cout << RED;
        }
        else if (carColor == "green") {
            cout << GREEN;
        }
        else {
            cout << RESET;
        }
    }

    void setMapColor() 
    {
        // Change color for '#'
        if (mapColor == "cyan") {
            cout << CYAN;
        }
        else if (mapColor == "yellow") {
            cout << YELLOW;
        }
        else if (mapColor == "green") {
            cout << GREEN;
        }
        else {
            cout << RESET;
        }
    }

    void printMap(int score) 
    {

        // Display the score at the top with dark green color
        cout << AQUA << "Score: " << score << RESET << endl;

        for (int i = 0; i < Rows; ++i) {
            for (int j = 0; j < Col; ++j) {
                switch (nodes[i][j].type) {
                case '>':
                    setCarColor();
                    cout << nodes[i][j].type << " " << RESET;
                    break;
                case '*':
                    setMapColor();
                    cout << nodes[i][j].type << " " << RESET;
                    break;
                case 'P':
                    cout << GOLD << nodes[i][j].type << " " << RESET;
                    break;
                case '@':
                    cout << IMPERIAL_RED << nodes[i][j].type << " " << RESET;
                    break;
                case 'H':
                    // Bright green color for health packs
                    cout << "\033[1;32m" << nodes[i][j].type << " " << RESET;
                    break;
                case '-':
                    // Bright yellow color for start point
                    cout << MINT_CREAM << nodes[i][j].type << " " << RESET;
                    break;
                case 'C':
                    // Bright yellow color for coins
                    cout << BLUE << nodes[i][j].type << " " << RESET;
                    break;
                default:
                    cout << nodes[i][j].type << " ";
                }
            }
            cout << endl;
        }
    }

    int calculateScore(double timeTaken, int distanceCovered, int& obstacleMultiplier, int& powerUpMultiplier, const Graph& graph, const pair<int, int>& carPosition, int powerUpsCollected);
    //Function to update the leaderboard with the score and power-ups collected
    
    
};


int Graph::calculateScore(double timeTaken, int distanceCovered, int& obstacleMultiplier, int& powerUpMultiplier, const Graph& graph, const pair<int, int>& carPosition, int powerUpsCollected) {
    // Adjust these values as needed
    int baseScore = 50;
    
    // Calculate the score using the provided formula
    int score = static_cast<int>((baseScore / timeTaken) * distanceCovered);
    
    // Apply power-up multiplier ONLY if power-ups have been collected
    // This maintains the multiplier effect across all frames
    if (powerUpsCollected > 0) {
        score *= (1 + powerUpsCollected);  // Increase multiplier based on total collected
    }
    
    // Add obstacle multiplier (this is additive, not multiplicative)
    score += obstacleMultiplier;
    
    // Ensure the final score is non-negative
    if (score < 0) {
        score = 0;
    }
    
    // Check if the car is at the bounus  (*) and increment the score
    if (graph.nodes[carPosition.first][carPosition.second].type == '*') {
        score += 10;
    }
    
    // Check if the car is on an obstacle (@) and decrement the score
    if (graph.nodes[carPosition.first][carPosition.second].type == '@') {
        score -= 1;
    }
    
    return score;
}


void manualMode(Graph& graph, string name) 
{
    char input;
    pair<int, int> carPosition = { 1, 1 }; // Initial car position (inside the boundary)
    auto startTime = chrono::steady_clock::now(); // Record the start time

    int health = 100; // Initial health
    int powerUpMultiplier = 1; // Multiplier for power-ups
    int powerUpsCollected = 0; // Variable to keep track of the number of power-ups collected
    double timeTaken = 0;
    int distanceCovered = 0;
    int obstacleMultiplier = 1;
    int tries = 3; // Number of tries
    LinkedListForCoins coinList;
    const int maxCoins = 20;
    int currentCoins = 0;
    int totalCoinsCollected = 0;
    // Flag to track if a health pack is currently on the map
    bool healthPackAvailable = false;

    // Variables for health pack timer
    auto healthPackStartTime = chrono::steady_clock::now();
    bool healthPackTimerStarted = false;

    do 
    {
        system("cls"); // Clear the console (for Windows, replace with appropriate command on other platforms)
        cout << setw(110) << "\033[1;31mWELCOME \033[1;36m" << name << "\033[1;31m TO THE CAR RACING GAME!\033[0m" << endl;


        cout << GREEN << "Health: " << health <<"\t Time: "<< timeTaken << RESET << endl;
        // Display health at the top
        cout << BLUE << "Coins Collected: " << totalCoinsCollected<<"\t Distance: "<< distanceCovered  << RESET << endl;
        cout << ORANGE << "Press P to pause" << RESET << endl;

        // Place the car on the map
        graph.nodes[carPosition.first][carPosition.second].type = '>';

        // Print the map
        graph.printMap(graph.calculateScore(timeTaken, distanceCovered, obstacleMultiplier, powerUpMultiplier, graph, carPosition, powerUpsCollected));

        cout << GOLD << "Power-ups Collected: " << powerUpsCollected << RESET << endl;

        int coinRow, coinCol, coinValue;

        // Keep generating new coin coordinates until an empty location is found
        if (currentCoins < maxCoins) 
        {
            do {
                coinRow = 1 + rand() % (graph.Rows - 2); // Changed to 1 + rand() % (graph.rows - 2) to accommodate the boundary
                coinCol = 1 + rand() % (graph.Col - 2); // Changed to 1 + rand() % (graph.cols - 2) to accommodate the boundary
                coinValue = 1 + rand() % 10; // Changed to 1 + rand() % 10 to accommodate the boundary
            } while (graph.nodes[coinRow][coinCol].type != '-'); // Changed to != '-' to accommodate the boundary

            // Place the coin on the map
            graph.nodes[coinRow][coinCol].type = 'C';
            coinList.addCoin(coinValue, coinRow, coinCol);
            currentCoins++;
        }

        // Check if all power-ups have been collected
        bool allPowerUpsCollected = true;
        for (int i = 1; i < graph.Rows - 1; ++i) {
            for (int j = 1; j < graph.Col - 1; ++j) {
                if (graph.nodes[i][j].type == 'P') {
                    allPowerUpsCollected = false;
                    break;
                }
            }
            if (!allPowerUpsCollected) {
                break;
            }
        }

        // Check if all power-ups have been collected
        if (allPowerUpsCollected) {
            // Hide the map
            system("cls"); // Clear the console (for Windows, replace with appropriate command on other platforms)

            // Display winning message
            cout << VIOLET << R"(



                                  _____                             __        __  __  __ ____   __  __  _      __ ____   _  __
                                 / ___/___   ___  ___ _ ____ ___ _ / /_ ___  / /  \ \/ // __ \ / / / / | | /| / // __ \ / |/ /)" << GREEN << R"(
                                / /__ / _ \ / _ \/ _ `// __// _ `// __/(_-< /_/    \  // /_/ // /_/ /  | |/ |/ // /_/ //    / )" << RED << R"(
                                \___/ \___//_//_/\_, //_/   \_,_/ \__//___/(_)     /_/ \____/ \____/   |__/|__/ \____//_/|_/  
                                                /___/                                                                         
            )" << RESET << endl;

            // Display the score in the middle
            int terminalWidth = 80; // Adjust this based on your terminal width
            int padding = (terminalWidth - 12) / 2; // Calculate padding for centering
            cout << endl;
            cout << endl;
            cout << endl;
            cout << endl;
            cout << endl;

            cout << string(padding, ' ') << GOLD << setw(30) << R"(
                                         __  __ ____   __  __ ___      ____ _____ ____   ___   ____  _   
                                         \ \/ // __ \ / / / // _ \    / __// ___// __ \ / _ \ / __/ (_)  
                                          \  // /_/ // /_/ // , _/   _\ \ / /__ / /_/ // , _// _/  _     
                                          /_/ \____/ \____//_/|_|   /___/ \___/ \____//_/|_|/___/ (_)    
                                                                   )" << setw(50) << graph.calculateScore(timeTaken, distanceCovered, obstacleMultiplier, powerUpMultiplier, graph, carPosition, powerUpsCollected) << RESET << endl;

            cout << endl;
            write_to_leaderboard(name, graph.calculateScore(timeTaken, distanceCovered, obstacleMultiplier, powerUpMultiplier, graph, carPosition, powerUpsCollected));
            cout << endl;


            // Prompt for going back to the main menu
            cout << CYAN << R"(
             __                                                                                                 _    /         \    
            |  \ _      _          _  _ |_  |_ _    _  _   |_  _  _|   |_ _   |_|_  _   _  _ . _    _  _ _       )  ( \_/ /|\ | ).  
            |__/(_)  \/(_)|_|  \)/(_|| )|_  |_(_)  (_)(_)  |_)(_|(_|(  |_(_)  |_| )(-  |||(_||| )  |||(-| )|_|  .    \ | / | \|/ .  
                     /                             _/                                                                              
             )" << RESET;
            char backToMainMenuInput;
            cin >> backToMainMenuInput;

            if (backToMainMenuInput == 'Y' || backToMainMenuInput == 'y') {
                // Quit the loop to return to the main menu
                break;
            }
        }

        // Get user input
        input = _getch(); // Use _getch() to capture a single key press

        // Calculate the new position based on user input
        int newCarPositionX = carPosition.first;
        int newCarPositionY = carPosition.second;

        switch (input) {
        case 'W':
        case 'w':
            newCarPositionX = max(1, carPosition.first - 1);
            break;
        case 'S':
        case 's':
            newCarPositionX = min(graph.Rows - 2, carPosition.first + 1);
            break;
        case 'A':
        case 'a':
            newCarPositionY = max(1, carPosition.second - 1);
            break;
        case 'D':
        case 'd':
            newCarPositionY = min(graph.Col - 2, carPosition.second + 1);
            break;
        case 'Q':
        case 'q':
            // Quit the loop on 'Q' or 'q'
            break;
        }

        // Check if the user pressed 'P' for pause
        if (input == 'P' || input == 'p') {
            // Pause the game and show the menu
            char pauseMenuInput;

            cout << CYAN << "Game Paused" << RESET << endl;
            cout << YELLOW << "1. Resume" << RESET << endl;
            cout << YELLOW << "2. Main Menu" << RESET << endl;
            cout << YELLOW << "3. Quit Game" << RESET << endl;
            cout << CYAN << "Enter your choice: " << RESET;
            cin >> pauseMenuInput;

            switch (pauseMenuInput) {
            case '1':
                // Resume the game
                break;
            case '2':
                // Go back to the main menu
                return; // This will exit the function and return to the main menu
            case '3':
                // Quit the game
                exit(0);
            default:
                cout << "Invalid choice. Resuming game..." << endl;
                break;
            }
        }

        // Check if the health pack timer has expired after getting user input
        if (health <= 20 && healthPackAvailable) {
            auto healthPackElapsedTime = chrono::steady_clock::now() - healthPackStartTime; // Calculate the elapsed time
            int remainingTime = 5 - chrono::duration_cast<chrono::seconds>(healthPackElapsedTime).count(); // Changed to chrono::seconds to accommodate the boundary
            if (remainingTime <= 0) {
                // Health pack timer expired, remove health pack from the map
                healthPackAvailable = false;
            }
        }

        // Generate a new health pack if needed
        if (!healthPackAvailable) 
        {
            int healthPackRow, healthPackCol;

            // Keep generating new health pack coordinates until an empty location is found
            do 
            {
                healthPackRow = 1 + rand() % (graph.Rows - 2); // Changed to 1 + rand() % (graph.rows - 2) to accommodate the boundary
                healthPackCol = 1 + rand() % (graph.Col - 2); // Changed to 1 + rand() % (graph.cols - 2) to accommodate the boundary
            } while (graph.nodes[healthPackRow][healthPackCol].type != '-'); // Changed to != '-' to accommodate the boundary

            // Place the health pack on the map
            graph.nodes[healthPackRow][healthPackCol].type = 'H'; // Changed to 'H' to accommodate the boundary
            healthPackAvailable = true;

            // Reset the health pack timer
            healthPackStartTime = chrono::steady_clock::now(); // Record the start time
        }

        // Check if the new position is an obstacle, a wall, or a power-up
        if (graph.nodes[newCarPositionX][newCarPositionY].type != '@') {
            // Move the car
            // Reset the previous position of the car
            graph.nodes[carPosition.first][carPosition.second].type = '-'; // Changed to '-' to accommodate the boundary

            // Update the car position
            carPosition.first = newCarPositionX; // Changed to first to accommodate the boundary
            carPosition.second = newCarPositionY; // Changed to second to accommodate the boundary

            // Check if the new position has a power-up
            if (graph.nodes[newCarPositionX][newCarPositionY].type == 'P') { // Power-up is available and car can collect it
                // Power-up multiplier
                powerUpMultiplier = 2;
                powerUpsCollected++; // Increment the count of power-ups collected
                // Clear the power-up from the map
                graph.nodes[newCarPositionX][newCarPositionY].type = '-';
            }
            else if (graph.nodes[newCarPositionX][newCarPositionY].type == 'C') {
                // Car collected a coin
                totalCoinsCollected++;

                // Clear the coin from the map
                graph.nodes[newCarPositionX][newCarPositionY].type = '-';
            }
            else {
                powerUpMultiplier = 1;
            }

            // Calculate the time taken
            auto endTime = chrono::steady_clock::now();
            chrono::duration<double> elapsedSeconds = endTime - startTime; // Calculate the elapsed time
            timeTaken = elapsedSeconds.count();

            // Increment the distance covered (assuming each move covers one node)
            distanceCovered++;

            // Calculate the obstacle multiplier
            obstacleMultiplier = (graph.nodes[newCarPositionX][newCarPositionY].type == '@') ? -5 : 1; // Obstacle multiplier is -5 if the new position is an obstacle

            int score = graph.calculateScore(timeTaken, distanceCovered, obstacleMultiplier, powerUpMultiplier, graph, carPosition, powerUpsCollected); // Added to calculate the score

            // Update the leaderboard
            // graph.updateLeaderboard(timeTaken, distanceCovered, obstacleMultiplier, powerUpMultiplier, powerUpsCollected, carPosition); // Added to update the leaderboard

            // Display the score
            cout << "Score: " << score << endl;

            // Display remaining health
            cout << "Health: " << health << endl;

            // Check if the new position has a health pack
            if (graph.nodes[newCarPositionX][newCarPositionY].type == 'H' && health < 100) { // Health pack is available and car can collect it
                // Health pack is available and car can collect it
                health += 20;

                // Remove the health pack from the map
                graph.nodes[newCarPositionX][newCarPositionY].type = '-'; // Changed to '-' to accommodate the boundary
                healthPackAvailable = false; // Reset the health pack availability
            }
        }
        else {
            // Car has crashed
            cout << CRIMSON << "Car has crashed! " << RESET << endl;

            // Decrement health on collision with an obstacle or wall
            health -= 20;

            // Display the remaining health
            cout << GREEN << "Remaining Health: " << health << RESET << endl;

            // Check if health is zero
            if (health <= 0) {
                cout << GREEN << "Out of health!" << RESET << endl;

                // Prompt for retry
                cout << CYAN << "You have 3 tries.Try again? (Y/N): " << RESET;
                char retryInput;
                cin >> retryInput;

                if (retryInput == 'Y' || retryInput == 'y') {
                    // Reset the crashed position
                    graph.nodes[carPosition.first][carPosition.second].type = '>'; // Changed to '>' to accommodate the boundary

                    // Reset health
                    health = 100;

                    // Reset the start time
                    startTime = chrono::steady_clock::now();

                    // Reset the health pack availability
                    healthPackAvailable = false;
                }
                else {
                    // Quit the loop
                    break;
                }
            }
        }

    } while (input != 'Q' && input != 'q'); // Quit the loop on 'Q' or 'q'
}


// Function to handle automated movement
void automatedMode(Graph& graph) {
    // Prompt the user for the starting and ending points
    pair<int, int> start, end;

    cout << endl << endl << CYAN << R"(                                                      Enter the starting point (row col) = )" << RESET;
    cin >> start.first >> start.second;

    cout << endl << endl << CYAN << R"(                                                      Enter the ending point (row col) = )" << RESET;
    cin >> end.first >> end.second;

    // Find the shortest path using Dijkstra's algorithm
    pair<int, int> path[Graph::MAX_ROWS * Graph::MAX_COLS]; // Changed to MAX_ROWS * MAX_COLS to accommodate the boundary
    graph.findShortestPath(start, end, path); // Changed to MAX_ROWS * MAX_COLS to accommodate the boundary

    // Update the map with start and end points
    graph.nodes[start.first][start.second].type = 'S'; // Changed to 'S' to accommodate the boundary
    graph.nodes[end.first][end.second].type = 'E'; // Changed to 'E' to accommodate the boundary

    // Move the car along the path
    for (int i = 0; i < Graph::MAX_ROWS * Graph::MAX_COLS && path[i] != make_pair(-1, -1); ++i) { // Changed to MAX_ROWS * MAX_COLS to accommodate the boundary
        system("cls");
        pair<int, int> currentPos = path[i];
        graph.nodes[currentPos.first][currentPos.second].type = '>'; // Place the car on the map
        graph.printMap(0); // Print the map
        this_thread::sleep_for(chrono::milliseconds(500)); // Add a delay for better visualization
        graph.nodes[currentPos.first][currentPos.second].type = '>'; // Reset the previous position of the car

        // Check if the car has reached the ending node
        if (currentPos == end) {
            cout << VIOLET << "Congratulations! You reached you destination!" << RESET << endl;

            // Ask if the user wants to go back to the main menu
            char choice;
            cout << CYAN << "Do you want to go back to the main menu? (Y/N): " << RESET;
            cin >> choice;

            if (choice == 'Y' || choice == 'y') {
                return; // Return to the main menu
            }
            else {
                exit(0); // Exit the program
            }
        }
    }

    // Reset the start and end points on the map
    graph.nodes[start.first][start.second].type = '>'; // Changed to '>' to accommodate the boundary
    graph.nodes[end.first][end.second].type = '>'; // Changed to '>' to accommodate the boundary
}


// Function to set the color for the car ('>')
void F(string color) {
    // Change color for '>'
    if (color == "blue") {
        cout << BLUE;
    }
    else if (color == "red") {
        cout << RED;
    }
    else if (color == "green") {
        cout << GREEN;
    }
    else {
        cout << RESET;
    }
}

// Function to set the color for walls ('#')
void setMapColor(const string& color) {
    // Change color for '#'
    if (color == "cyan") {
        cout << CYAN;
    }
    else if (color == "yellow") {
        cout << YELLOW;
    }
    else if (color == "green") {
        cout << GREEN;
    }
    else {
        cout << RESET;
    }
}

void displayInstructions() {
    system("cls"); // Clear the console (for Windows, replace with an appropriate command on other platforms)

    cout << setw(60) << RED << "GAME INSTRUCTIONS" << RESET << endl;
    cout << setw(55) << CYAN << "======================" << RESET << endl;
    cout << YELLOW << "1. Manual Mode:" << RESET << endl;
    cout << CRIMSON << "   - Use 'W', 'A', 'S', 'D' keys to move the car." << RESET << endl;
    cout << CRIMSON << "   - Collect power-ups ('P')and bonus ('*') to increase your score." << RESET << endl;
    cout << CRIMSON << "   - Avoid obstacles ('@')." << RESET << endl;
    cout << CRIMSON << "   - Health packs ('H') restore your health." << RESET << endl;
    cout << CRIMSON << "   - The game ends if your health reaches zero." << RESET << endl;
    cout << CRIMSON << "   - Press 'Q' to quit the game." << RESET << endl;
    cout << YELLOW << "2. Automated Mode:" << RESET << endl;
    cout << CRIMSON << "   - Enter the starting and ending points to find the shortest path." << RESET << endl;
    cout << CRIMSON << "   - The car will automatically move along the path." << RESET << endl;
    cout << YELLOW << "3. Customize Map Color:" << RESET << endl;
    cout << CRIMSON << "   - Choose the color for walls ('*') in the map." << RESET << endl;
    cout << YELLOW << "4. Customize Car Color:" << RESET << endl;
    cout << CRIMSON << "   - Choose the color for the car ('>')." << RESET << endl;
    cout << YELLOW << "5. Display Leaderboard:" << RESET << endl;
    cout << CRIMSON << "   - View the scores and power-ups collected in previous games." << RESET << endl;
    cout << YELLOW << "6. Exit:" << RESET << endl;
    cout << CRIMSON << "   - Quit the program." << RESET << endl << endl;

    cout << CYAN << "Press any key to return to the menu..." << RESET << endl;
    _getch(); // Wait for a key press before returning to the menu
}



void displayMenu(Graph& gameGraph) {
    char option;

    int colors[][3] = {
        {255, 0, 0},    // Red
        {255, 165, 0},  // Orange
        {255, 255, 0},  // Yellow
        {0, 255, 0},    // Green
        {0, 0, 255},    // Blue
        {75, 0, 130},   // Indigo
        {128, 0, 128}   // Violet
    };

    // Number of colors
    int numColors = sizeof(colors) / sizeof(colors[0]);

    
    cout << YELLOW << "1. Enter InGame Name: " << RESET << endl;
    string name;
    getline(cin, name);

    do {
        system("cls"); // Clear the console (for Windows, replace with appropriate command on other platforms)


        cout << setw(100) << GREEN << "CAR GAME" << endl;




        cout << RESET << setw(30) << BLUE << "*********************************************************************************************************************" << RESET << endl;



       
        cout << GREEN << "==================================================================================" << RESET << endl;
        cout << RED << "                      CAR RACING GAME - MAIN MENU" << RESET << endl;
        cout << GREEN << "==================================================================================" << RESET << endl;
        cout << endl;
        
        cout << YELLOW << "1. Play Manual Mode" << RESET << endl;
        cout << "   - Use WASD keys to control the car manually" << endl;
        cout << endl;
        
        cout << YELLOW << "2. Play Automated Mode" << RESET << endl;
        cout << "   - Uses Dijkstra's algorithm for pathfinding" << endl;
        cout << endl;
        
        cout << YELLOW << "3. Customize Map Color" << RESET << endl;
        cout << "   - Change the color of map" << endl;
        cout << endl;
        
        cout << YELLOW << "4. Customize Car Color" << RESET << endl;
        cout << "   - Change the color of your car" << endl;
        cout << endl;
        
        cout << YELLOW << "5. Display Leaderboard" << RESET << endl;
        cout << "   - View current game scores" << endl;
        cout << endl;
        
        cout << YELLOW << "6. Game Instructions" << RESET << endl;
        cout << "   - Learn how to play the game" << endl;
        cout << endl;

        cout << YELLOW << "7. LEADERBOARD BST" << RESET << endl;
        cout << "   - perfrom operations using AVL" << endl;
        cout << endl;

        cout << YELLOW << "8. Exit" << RESET << endl;
        cout << "   - Quit the game" << endl;
        cout << endl;
        

    cout << "-----------------------------------" << endl;
    cout << "Enter your choice (1-8): ";

         // Get user input
        //cout << CYAN << "Enter your choice (1-6): " << RESET;
        cin >> option;

        if (option == '1') {
            // Play manual mode
            manualMode(gameGraph, name);
        }
        else if (option == '2') {
            // Play automated mode
            automatedMode(gameGraph);
        }
        else if (option == '3') {
            // Customize map color
            cout << CYAN << "Enter map color: " << endl;
            cout << CYAN << "=> " << CYAN << "CYAN" << RESET << endl;
            cout << YELLOW << "=> " << YELLOW << "YELLOW" << RESET << endl;
            cout << GREEN << "=> " << GREEN << "GREEN" << RESET << endl;
            cin >> gameGraph.mapColor;
            gameGraph.printMap(0);
            cout << CYAN << "Press any key to return to the menu..." << RESET << endl;
    _getch(); // Wait for a key press before returning to the menu

        }
        else if (option == '4') {
            // Customize car color
            cout << CYAN << "Enter car color: " << RESET << endl;
            cout << BLUE << "=> " << BLUE << "BLUE" << RESET << endl;
            cout << RED << "=> " << RED << "RED" << RESET << endl;
            cout << GREEN << "=> " << GREEN << "GREEN" << RESET << endl;
            cin >> gameGraph.carColor;
            gameGraph.printMap(0);
        }
         else if (option == '5') {
            // Display leaderboard
             read_leaderboard();
        }
        else if (option == '6') {
            // Display game instructions
            displayInstructions();
        }else if (option == '7') {
            // Display leaderboard using BST
            AVLMenu(); // Wait for a key press before returning to the menu
            
        }
        else if (option == '8') {
            

            break;
        }
        
        else {
            // Invalid input
            cout << RED << "Invalid input. Please try again." << RESET << endl;
        }


    } while (option != '8');
}