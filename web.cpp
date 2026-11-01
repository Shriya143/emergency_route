// make_website.cpp : a C++ program that runs Dijkstra and WRITES the website (index.html)
// Compile: g++ make_website.cpp -o make_website      Run: ./make_website
#include <fstream>
#include <string>
#include <vector>
using namespace std;

const int V = 7, INF = 9999;
string nm[V] = {"Fire Station", "Sector 14", "Sector 21", "Market", "Bus Stand", "Rail Crossing", "Hospital"};
int px[V] = {80, 250, 250, 460, 500, 640, 720};     // where to draw each place
int py[V] = {170, 60, 280, 110, 280, 50, 190};
int road[V][V];

struct Step { int dist[V], parent[V]; bool done[V]; int cur; string note; };
vector<Step> steps;                                  // every step of Dijkstra is saved here

string str(int x) { return to_string(x); }
string dtxt(int d) { return d == INF ? "∞" : str(d); }

void build() {                                       // the city map
    int r[10][3] = {{0,1,4},{0,2,2},{1,2,1},{1,3,5},{2,3,8},{2,4,10},{3,4,2},{3,5,6},{4,6,3},{5,6,1}};
    for (int i = 0; i < V; i++) for (int j = 0; j < V; j++) road[i][j] = 0;
    for (int i = 0; i < 10; i++) road[r[i][0]][r[i][1]] = road[r[i][1]][r[i][0]] = r[i][2];
}
void cut(int a, int b) { road[a][b] = road[b][a] = 0; }

void saveStep(int dist[], int par[], bool done[], int cur, string note) {
    Step s; s.cur = cur; s.note = note;
    for (int i = 0; i < V; i++) { s.dist[i] = dist[i]; s.parent[i] = par[i]; s.done[i] = done[i]; }
    steps.push_back(s);
}

int dijkstra(int src, int dist[], int par[], bool rec) {
    bool done[V]; int checks = 0;
    for (int i = 0; i < V; i++) { dist[i] = INF; par[i] = -1; done[i] = false; }
    dist[src] = 0;
    if (rec) saveStep(dist, par, done, -1, "Start: " + nm[src] + " has distance 0. All other places are unknown (∞).");
    for (int c = 0; c < V; c++) {
        int u = -1;
        for (int i = 0; i < V; i++) if (!done[i] && (u == -1 || dist[i] < dist[u])) u = i;
        if (dist[u] == INF) break;
        done[u] = true;
        string note = "Pick " + nm[u] + " (distance " + str(dist[u]) + ") because it is the nearest unfinished place. ";
        for (int v = 0; v < V; v++) if (road[u][v] > 0) {
            checks++;
            if (dist[u] + road[u][v] < dist[v]) {
                note += nm[v] + " improves " + dtxt(dist[v]) + " → " + str(dist[u] + road[u][v]) + ". ";
                dist[v] = dist[u] + road[u][v]; par[v] = u;
            }
        }
        if (rec) saveStep(dist, par, done, u, note);
    }
    return checks;
}

int bellman(int src, int dist[], int par[]) {
    int checks = 0;
    for (int i = 0; i < V; i++) { dist[i] = INF; par[i] = -1; }
    dist[src] = 0;
    for (int pass = 1; pass < V; pass++) {
        bool changed = false;
        for (int u = 0; u < V; u++) for (int v = 0; v < V; v++)
            if (road[u][v] > 0 && dist[u] != INF) {
                checks++;
                if (dist[u] + road[u][v] < dist[v]) { dist[v] = dist[u] + road[u][v]; par[v] = u; changed = true; }
            }
        if (!changed) break;
    }
    return checks;
}

string line(int a, int b, string col, int w) {
    return "<line x1='" + str(px[a]) + "' y1='" + str(py[a]) + "' x2='" + str(px[b]) + "' y2='" + str(py[b]) +
           "' stroke='" + col + "' stroke-width='" + str(w) + "' stroke-linecap='round'/>";
}

string drawMap(const Step& s, bool last) {            // one SVG picture of the city
    string o = "<svg viewBox='0 0 800 350' class='g'>";
    for (int a = 0; a < V; a++) for (int b = a + 1; b < V; b++) if (road[a][b] > 0) o += line(a, b, "#b0bccf", 3);
    for (int v = 0; v < V; v++) if (s.parent[v] != -1) o += line(s.parent[v], v, "#1976d2", 5);
    if (last) for (int v = V - 1; v != -1 && s.parent[v] != -1; v = s.parent[v]) o += line(s.parent[v], v, "#d32f2f", 8);
    for (int a = 0; a < V; a++) for (int b = a + 1; b < V; b++) if (road[a][b] > 0)
        o += "<text class='w' x='" + str((px[a] + px[b]) / 2) + "' y='" + str((py[a] + py[b]) / 2 + 5) + "'>" + str(road[a][b]) + "</text>";
    for (int v = 0; v < V; v++) {
        string fill = s.done[v] ? "#2e7d32" : "#1F3864";
        if (v == s.cur) fill = "#f57c00";
        o += "<circle cx='" + str(px[v]) + "' cy='" + str(py[v]) + "' r='27' fill='" + fill + "' stroke='#fff' stroke-width='3'/>";
        o += "<text class='d' x='" + str(px[v]) + "' y='" + str(py[v] + 5) + "'>" + dtxt(s.dist[v]) + "</text>";
        o += "<text class='n' x='" + str(px[v]) + "' y='" + str(py[v] + 48) + "'>" + nm[v] + "</text>";
    }
    return o + "</svg>";
}

string testRow(string title, int src, int dst) {
    int d1[V], p1[V], d2[V], p2[V];
    int c1 = dijkstra(src, d1, p1, false), c2 = bellman(src, d2, p2);
    bool ok = d1[dst] != INF;
    return "<tr><td>" + title + "</td><td>" + (ok ? str(d1[dst]) + " km" : "Not reachable") + "</td><td>" + (ok ? str(c1) : "–") +
           "</td><td>" + (ok ? str(d2[dst]) + " km" : "Not reachable") + "</td><td>" + (ok ? str(c2) : "–") + "</td></tr>";
}

int main() {
    build();
    int dist[V], par[V];
    dijkstra(0, dist, par, true);                     // run Dijkstra and save all steps
    int n = steps.size();
    string route;                                     // final route text
    vector<int> path;
    for (int v = 6; v != -1; v = par[v]) path.insert(path.begin(), v);
    for (size_t i = 0; i < path.size(); i++) route += (i ? " → " : "") + nm[path[i]];

    ofstream f("index.html");
    f << "<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
         "<title>Shortest Path for Emergency Services</title><style>";
    f << R"CSS(
*{box-sizing:border-box}body{margin:0;font-family:system-ui,'Segoe UI',Arial,sans-serif;background:#f3f6fb;color:#1b2a41;line-height:1.6}
header{background:linear-gradient(135deg,#1F3864,#2E74B5);color:#fff;padding:48px 20px;text-align:center}
header h1{margin:0 0 8px;font-size:2.1rem}header p{margin:4px;opacity:.92}
main{max-width:960px;margin:auto;padding:12px 16px}
section{background:#fff;border-radius:14px;padding:22px;margin:20px 0;box-shadow:0 2px 10px rgba(0,0,0,.08)}
h2{color:#1F3864;margin-top:0}
input[name=s]{display:none}.tabs{display:flex;flex-wrap:wrap;gap:6px;margin-bottom:12px}
.tabs label{padding:7px 14px;border-radius:20px;background:#e3eaf6;cursor:pointer;font-size:.9rem}
.panel{display:none}.g{width:100%;background:#fafcff;border:1px solid #dde5f2;border-radius:10px}
.w{font:700 14px sans-serif;text-anchor:middle;paint-order:stroke;stroke:#fff;stroke-width:5px;fill:#1b2a41}
.d{fill:#fff;font:700 16px sans-serif;text-anchor:middle}.n{fill:#1b2a41;font:600 13px sans-serif;text-anchor:middle}
.note{background:#fff7e0;border-left:5px solid #f9a825;padding:10px 14px;border-radius:6px;margin:12px 0}
table{border-collapse:collapse;width:100%;font-size:.9rem;margin:8px 0}th{background:#1F3864;color:#fff}
th,td{border:1px solid #cfd8e8;padding:6px;text-align:center}
.legend{display:flex;flex-wrap:wrap;gap:14px;font-size:.9rem;margin:10px 0}.legend b{display:inline-block;width:14px;height:14px;border-radius:50%;vertical-align:middle;margin-right:5px}
.big{font-size:1.25rem;background:#e8f5e9;border-radius:10px;padding:14px;text-align:center}
.cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:14px}.card{background:#eef3fb;border-radius:10px;padding:14px}
footer{text-align:center;padding:26px;color:#556}
)CSS";
    for (int i = 0; i < n; i++)                       // CSS that shows only the chosen step (no JavaScript needed)
        f << "#s" << i << ":checked~.panels #p" << i << "{display:block}#s" << i << ":checked~.tabs label[for=s" << i
          << "]{background:#1F3864;color:#fff}";
    f << "</style></head><body><header><h1>🚑 Shortest Path Planning for Emergency Services</h1>"
         "<p>DSA-II Case Study · Dijkstra’s Algorithm vs Bellman–Ford</p><p>Shriya · 25BAI10016 · BE CSE Microsoft AI · 25BAI-601</p></header><main>";

    f << "<section><h2>1. The problem</h2><p>An ambulance or fire engine must reach the hospital by the <b>shortest road</b>. "
         "We draw the city as a graph: <b>places are circles</b> and <b>roads are lines</b> with their length in km. "
         "The goal: find the shortest route from the <b>Fire Station</b> to the <b>Hospital</b>.</p>"
         "<div class='legend'><span><b style='background:#1F3864'></b>not finished</span><span><b style='background:#f57c00'></b>just picked</span>"
         "<span><b style='background:#2e7d32'></b>finished (final distance)</span><span><b style='background:#1976d2'></b>best known way</span>"
         "<span><b style='background:#d32f2f'></b>final shortest route</span></div></section>";

    f << "<section><h2>2. Watch Dijkstra work, step by step</h2><p>Click a step. The number inside each circle is its distance from the Fire Station.</p><div>";
    for (int i = 0; i < n; i++) f << "<input type='radio' name='s' id='s" << i << "'" << (i == 0 ? " checked" : "") << ">";
    f << "<div class='tabs'>";
    for (int i = 0; i < n; i++) f << "<label for='s" << i << "'>" << (i == 0 ? string("Start") : "Step " + str(i)) << "</label>";
    f << "</div><div class='panels'>";
    for (int i = 0; i < n; i++) {
        f << "<div class='panel' id='p" << i << "'>" << drawMap(steps[i], i == n - 1) << "<div class='note'>" << steps[i].note << "</div><table><tr>";
        for (int v = 0; v < V; v++) f << "<th>" << nm[v] << "</th>";
        f << "</tr><tr>";
        for (int v = 0; v < V; v++) f << "<td>" << dtxt(steps[i].dist[v]) << "</td>";
        f << "</tr></table></div>";
    }
    f << "</div></div></section>";

    f << "<section><h2>3. The answer</h2><div class='big'>Shortest route: <b>" << route << "</b><br>Total distance: <b>" << dist[6]
      << " km</b></div><p>Dijkstra always picks the nearest unfinished place, so a place is never made shorter later. "
         "This works because road lengths are never negative.</p></section>";

    f << "<section><h2>4. Tests: Dijkstra vs Bellman–Ford</h2><table><tr><th>Test</th><th>Dijkstra</th><th>Road checks</th><th>Bellman–Ford</th><th>Road checks</th></tr>";
    build(); f << testRow("1. Normal case", 0, 6);
    build(); cut(3, 4); f << testRow("2. Market–Bus Stand road closed", 0, 6);
    build(); f << testRow("3. Start = End", 0, 0);
    build(); cut(4, 6); cut(3, 5); f << testRow("4. Hospital cut off", 0, 6);
    f << "</table><p>Both give the same answer, but Bellman–Ford checks the roads many more times.</p></section>";

    f << "<section><h2>5. How fast is it?</h2><div class='cards'>"
         "<div class='card'><h3>Dijkstra</h3><p>Time <b>O(V²)</b> (array version)<br>or <b>O((V+E) log V)</b> with a priority queue.<br>Needs non-negative road lengths.</p></div>"
         "<div class='card'><h3>Bellman–Ford</h3><p>Time <b>O(V × E)</b><br>Slower, but allows negative lengths.</p></div>"
         "<div class='card'><h3>Limitation</h3><p>This model uses only distance. Real maps also have traffic and one-way roads.</p></div></div></section>";

    f << "</main><footer>This page was generated by a C++ program (make_website.cpp). DSA-II Case Study 2026.</footer></body></html>";
    f.close();
    return 0;
}
