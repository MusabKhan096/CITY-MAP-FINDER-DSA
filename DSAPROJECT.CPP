#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <iomanip>
#include <queue>
#include <cmath>
#include <map>

using namespace std;

// ==========================================
//        CONFIGURATION CONSTANTS
// ==========================================
const double PRICE_PETROL = 280.0;  // Price in PKR
const double PRICE_DIESEL = 295.0;  // Price in PKR
const int MAX_CITIES = 20;          // Maximum supported cities
const double INF = 1e9;             // Representation of Infinity

// ==========================================
//          DATA STRUCTURES
// ==========================================

// Enum to define traffic severity levels
enum TrafficLevel {
    LOW,        // 1.0x time (Clear road)
    MODERATE,   // 1.2x time (Normal traffic)
    HIGH,       // 1.5x time (Rush hour)
    JAMMED      // 2.5x time (Blockage)
};

// Enum to define road types
enum RoadType {
    MOTORWAY,   // High speed, better efficiency
    HIGHWAY,    // Variable speed
    LOCAL       // Low speed, stops
};

// Structure representing a road connection
struct Edge {
    int destination;
    double distanceKM;
    TrafficLevel traffic;
    RoadType type;
    string roadName;
};

// Structure for Priority Queue comparison
struct PqNode {
    int id;
    double timeCost;

    // Overload operator for Min-Heap (Lowest time first)
    bool operator>(const PqNode& other) const {
        return timeCost > other.timeCost;
    }
};

// ==========================================
//        CORE ROUTING CLASS
// ==========================================
class RoutePlanner {
private:
    // Adjacency List: Graph representation
    vector<Edge> adj[MAX_CITIES];
    string cityNames[MAX_CITIES];
    int cityCount;

public:
    RoutePlanner() {
        cityCount = 0;
        initializeMapData();
    }

    // Helper to get traffic multiplier based on severity
    double getTrafficMultiplier(TrafficLevel level) {
        switch (level) {
            case LOW: return 1.0;
            case MODERATE: return 1.2;
            case HIGH: return 1.5;
            case JAMMED: return 2.5;
            default: return 1.0;
        }
    }

    // Helper to convert Enum to String for display
    string getTrafficString(TrafficLevel level) {
        switch (level) {
            case LOW: return "Clear";
            case MODERATE: return "Moderate";
            case HIGH: return "Heavy";
            case JAMMED: return "Jammed";
            default: return "Unknown";
        }
    }

    // Physics Calculation: Fuel Efficiency (km/L) based on speed
    // Optimal efficiency is assumed around 80-90 km/h
    double calculateFuelEfficiency(int speed, RoadType type) {
        double baseEfficiency = 16.0; // Standard sedan
        
        // Adjust base efficiency by road type
        if (type == LOCAL) baseEfficiency -= 4.0; // Stop-and-go traffic
        
        // Aerodynamic drag formula approximation
        if (speed > 90) {
            double excess = speed - 90;
            // Efficiency drops drastically at high speeds due to air resistance
            double drop = (excess * excess) / 400.0; 
            return max(5.0, baseEfficiency - drop);
        } else if (speed < 40) {
             // Engines are inefficient at very low gears
            return baseEfficiency - 3.0;
        }
        return baseEfficiency;
    }

    // ==========================================
    //      MAP DATA INITIALIZATION
    // ==========================================
    void addCity(int id, string name) {
        if (id < MAX_CITIES) {
            cityNames[id] = name;
            cityCount = max(cityCount, id);
        }
    }

    void addRoad(int u, int v, double dist, TrafficLevel traf, RoadType type, string name) {
        // Add bidirectional edges (Roads go both ways)
        adj[u].push_back({v, dist, traf, type, name});
        adj[v].push_back({u, dist, traf, type, name});
    }

    void initializeMapData() {
        // 1. Define Cities
        addCity(1, "Karachi");
        addCity(2, "Hyderabad");
        addCity(3, "Sukkur");
        addCity(4, "Multan");
        addCity(5, "Faisalabad");
        addCity(6, "Lahore");
        addCity(7, "Islamabad");
        addCity(8, "Peshawar");
        addCity(9, "Quetta");
        addCity(10, "Gwadar");
        addCity(11, "Sialkot");
        addCity(12, "Abbottabad");
        addCity(13, "Gilgit");
        addCity(14, "Sahiwal");
        addCity(15, "Bahawalpur");

        // 2. Define Roads (Source, Dest, Dist, Traffic, Type, Name)
        
        // South Corridor
        addRoad(1, 2, 165, JAMMED, MOTORWAY, "M-9 Motorway"); // Karachi-Hyd
        addRoad(2, 3, 330, MODERATE, HIGHWAY, "N-5 National Hwy");
        addRoad(3, 4, 420, LOW, MOTORWAY, "M-5 Sukkur-Multan");
        addRoad(3, 9, 390, LOW, HIGHWAY, "N-65 Highway"); // Sukkur-Quetta
        
        // Central Corridor
        addRoad(4, 5, 240, LOW, MOTORWAY, "M-4 Motorway"); // Multan-Faisalabad
        addRoad(4, 15, 90, MODERATE, HIGHWAY, "N-5 Lodhran"); // Multan-BWP
        addRoad(15, 3, 300, LOW, HIGHWAY, "N-5 South"); // BWP-Sukkur
        addRoad(4, 14, 180, MODERATE, HIGHWAY, "N-5 GT Road"); // Multan-Sahiwal
        addRoad(14, 6, 170, HIGH, HIGHWAY, "N-5 Okara"); // Sahiwal-Lahore

        // Punjab Grid
        addRoad(5, 6, 150, HIGH, MOTORWAY, "M-3 Motorway"); // Fsd-Lahore
        addRoad(5, 7, 320, LOW, MOTORWAY, "M-4 (Goa-Pindi)"); // Fsd-Isb
        addRoad(6, 7, 375, MODERATE, MOTORWAY, "M-2 Motorway"); // Lhr-Isb
        addRoad(6, 11, 130, MODERATE, MOTORWAY, "M-11 Sialkot");
        
        // North Corridor
        addRoad(7, 8, 180, LOW, MOTORWAY, "M-1 Motorway"); // Isb-Peshawar
        addRoad(7, 12, 120, HIGH, HIGHWAY, "N-35 Karakoram"); // Isb-Abbottabad
        addRoad(12, 13, 450, HIGH, HIGHWAY, "KKH (Hazara)"); // Abbottabad-Gilgit

        // West Corridor
        addRoad(1, 10, 650, LOW, HIGHWAY, "N-10 Coastal Hwy"); // Karachi-Gwadar
        addRoad(10, 9, 920, LOW, HIGHWAY, "N-85 Highway"); // Gwadar-Quetta
        addRoad(9, 8, 800, LOW, HIGHWAY, "N-50 Zhob Route"); // Quetta-Peshawar (Long route)
    }

    // ==========================================
    //      MAIN ALGORITHM (DIJKSTRA)
    // ==========================================
    void findRoute(int startNode, int endNode, int speed) {
        // Validation
        if (startNode < 1 || startNode > cityCount || endNode < 1 || endNode > cityCount) {
            cout << "Invalid City ID Selected!" << endl;
            return;
        }

        // DP Arrays
        priority_queue<PqNode, vector<PqNode>, greater<PqNode>> pq;
        vector<double> minTime(MAX_CITIES, INF);
        vector<int> parent(MAX_CITIES, -1);
        vector<double> fuelConsumed(MAX_CITIES, 0.0);
        vector<double> pathDist(MAX_CITIES, 0.0);

        // Initialize Start
        minTime[startNode] = 0;
        pq.push({startNode, 0});

        while (!pq.empty()) {
            int u = pq.top().id;
            double currentTime = pq.top().timeCost;
            pq.pop();

            if (currentTime > minTime[u]) continue;

            for (auto& edge : adj[u]) {
                int v = edge.destination;
                
                // --- PHYSICS LOGIC START ---
                double multiplier = getTrafficMultiplier(edge.traffic);
                
                // Time = (Distance / Speed) * 60 minutes * TrafficPenalty
                double baseTime = (edge.distanceKM / speed) * 60.0;
                double realTime = baseTime * multiplier;

                if (minTime[u] + realTime < minTime[v]) {
                    minTime[v] = minTime[u] + realTime;
                    parent[v] = u;
                    pathDist[v] = pathDist[u] + edge.distanceKM;
                    
                    // Calculate Fuel for this segment
                    double segmentEff = calculateFuelEfficiency(speed, edge.type);
                    // Fuel for segment = Dist / Efficiency. 
                    // Note: Fuel calculation is accumulative for final report
                    fuelConsumed[v] = fuelConsumed[u] + (edge.distanceKM / segmentEff);
                    
                    pq.push({v, minTime[v]});
                }
                // --- PHYSICS LOGIC END ---
            }
        }

        // Check reachability
        if (minTime[endNode] == INF) {
            cout << "\nError: No road connection exists between these cities." << endl;
            return;
        }

        printDetailedReceipt(startNode, endNode, parent, minTime[endNode], pathDist[endNode], fuelConsumed[endNode], speed);
    }

    // ==========================================
    //          OUTPUT FORMATTING
    // ==========================================
    void printDetailedReceipt(int start, int end, vector<int>& parent, double totalTime, double totalDist, double totalFuel, int speed) {
        // Reconstruct path
        vector<int> path;
        for (int v = end; v != -1; v = parent[v]) {
            path.push_back(v);
        }

        cout << "\n";
        cout << "########################################################" << endl;
        cout << "              SMART ROUTE NAVIGATOR RESULTS             " << endl;
        cout << "########################################################" << endl;
        cout << " Origin      : " << cityNames[start] << endl;
        cout << " Destination : " << cityNames[end] << endl;
        cout << " Avg Speed   : " << speed << " km/h" << endl;
        cout << "--------------------------------------------------------" << endl;
        cout << left << setw(20) << "Leg From -> To" 
             << setw(18) << "Via Road" 
             << setw(10) << "Cond." 
             << "Dist." << endl;
        cout << "--------------------------------------------------------" << endl;

        // Print Path in correct order (Reverse of backtracking)
        for (int i = path.size() - 1; i > 0; i--) {
            int u = path[i];
            int v = path[i-1];
            
            // Find the specific edge used to get details
            string rName = "Unknown";
            string tCond = "Unknown";
            double d = 0;
            
            for(auto& e : adj[u]) {
                if(e.destination == v) {
                    rName = e.roadName;
                    tCond = getTrafficString(e.traffic);
                    d = e.distanceKM;
                    break;
                }
            }

            string leg = cityNames[u] + "->" + cityNames[v];
            // Truncate leg name if too long for cleaner output
            if(leg.length() > 18) leg = leg.substr(0, 18);

            cout << left << setw(20) << leg
                 << setw(18) << rName 
                 << setw(10) << tCond 
                 << d << " km" << endl;
        }

        cout << "--------------------------------------------------------" << endl;
        
        // Final Calculations
        int hrs = (int)totalTime / 60;
        int mins = (int)totalTime % 60;
        double cost = totalFuel * PRICE_PETROL;

        cout << right << setw(35) << "TOTAL DISTANCE : " << setw(10) << totalDist << " km" << endl;
        cout << right << setw(35) << "ESTIMATED TIME : " << hrs << "h " << mins << "m" << endl;
        cout << right << setw(35) << "FUEL REQUIRED : " << fixed << setprecision(1) << totalFuel << " L" << endl;
        cout << right << setw(35) << "EST. FUEL COST : " << "PKR " << setprecision(2) << cost << endl;
        cout << "########################################################" << endl;
        cout << "Note: Traffic conditions may vary based on weather." << endl;
    }

    void displayMenu() {
        cout << "\n--- AVAILABLE CITIES ---" << endl;
        for (int i = 1; i <= cityCount; i++) {
            cout << left << setw(3) << i << ". " << setw(15) << cityNames[i];
            if (i % 3 == 0) cout << endl; // New line every 3 cities
        }
        if (cityCount % 3 != 0) cout << endl;
    }
};

// ==========================================
//           MAIN EXECUTION
// ==========================================
int main() {
    RoutePlanner app;
    int source, dest, speedInput;
    char choice = 'y';

    while (choice == 'y' || choice == 'Y') {
        cout << "\n=============================================" << endl;
        cout << "      PAKISTAN INTER-CITY ROUTE FINDER       " << endl;
        cout << "=============================================" << endl;
        
        app.displayMenu();

        // Input Validation Loop for Source
        while (true) {
            cout << "\nEnter Start Location ID (1-15): ";
            if (cin >> source && source >= 1 && source <= 15) break;
            cout << "Invalid Input! Please enter a number between 1 and 15." << endl;
            cin.clear(); cin.ignore(1000, '\n');
        }

        // Input Validation Loop for Destination
        while (true) {
            cout << "Enter Destination ID (1-15) : ";
            if (cin >> dest && dest >= 1 && dest <= 15) break;
            cout << "Invalid Input! Please enter a number between 1 and 15." << endl;
            cin.clear(); cin.ignore(1000, '\n');
        }

        // Input Validation Loop for Speed
        while (true) {
            cout << "Enter Average Speed (40-160 km/h): ";
            if (cin >> speedInput && speedInput >= 40 && speedInput <= 160) break;
            cout << "Unrealistic speed! Please keep it between 40 and 160." << endl;
            cin.clear(); cin.ignore(1000, '\n');
        }

        // Execute Algorithm
        app.findRoute(source, dest, speedInput);

        cout << "\nDo you want to plan another trip? (y/n): ";
        cin >> choice;
    }

    cout << "\nThank you for using Smart Route Navigator. Drive Safely!" << endl;
    return 0;
}
