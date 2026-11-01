# emergency_route
📌 About the project

Ambulances and fire engines must reach the hospital by the shortest road. In this project a small city is drawn as a graph: places are vertices and roads are edges with their length in km. Dijkstra's algorithm finds the shortest route from the Fire Station to the Hospital, and it is compared with the Bellman–Ford algorithm.

Result: the shortest route is Fire Station → Sector 21 → Sector 14 → Market → Bus Stand → Hospital = 13 km.

📂 Files
File	What it does
emergency_route.cpp	Console program: runs Dijkstra and Bellman–Ford and prints the routes for 4 test cases
make_website.cpp	C++ program that runs Dijkstra and generates the website (index.html)
index.html	The explainer website (step-by-step Dijkstra viewer, no JavaScript)
DSA_II_Case_Study_Shortest_Path_Emergency_Services.docx	Full case study report
▶️ How to run

1. Console program

bash
g++ emergency_route.cpp -o emergency_route
./emergency_route

2. Generate the website

bash
g++ make_website.cpp -o make_website
./make_website

Then open index.html in any browser.

🧪 Test cases and results
Test	Dijkstra	Dijkstra checks	Bellman–Ford	Bellman–Ford checks
1. Normal case	13 km	20	13 km	60
2. Market–Bus Stand road closed	15 km	18	15 km	54
3. Start = End	0 km	20	0 km	60
4. Hospital cut off	Not reachable	–	Not reachable	–

Both algorithms always give the same answer, but Dijkstra does much less work.

⏱️ Complexity
Algorithm	Time	Note
Dijkstra (array version)	O(V²)	Needs non-negative road lengths
Dijkstra (priority queue)	O((V + E) log V)	Faster for big maps
Bellman–Ford	O(V × E)	Slower, but allows negative lengths
⚠️ Limitations

The model uses only road length. Real maps also have traffic, signals and one-way roads.

🤖 Acknowledgement

Drafted with the help of Claude (Anthropic). The code was compiled and tested, and I can explain the algorithm and the code.
