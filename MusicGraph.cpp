#include "MusicGraph.h"

// =============================================================================
// PRIVATE HELPER METHODS
// =============================================================================

int MusicGraph::getSongIndex(const string& id) const {
    // TODO: Implement linear search to find the index of a song by its ID
    for (int i = 0; i < songsList.size(); i++){
        if (songsList[i].id == id) return i;
    }
    return -1;
}

bool MusicGraph::isVisited(const string& id, const vector<string>& visitedList) const {
    // TODO: Check if the given ID exists in the visitedList
    for (int i = 0; i < visitedList.size(); i++){
        if (visitedList[i] == id) return true;
    }
    return false;
}

// =============================================================================
// PUBLIC METHODS
// =============================================================================

void MusicGraph::addSong(const string& id, const string& title, const string& artist, const string& genre) {
    // TODO: Add a new song to the system and add its ID as a vertex in the graph
    if (getSongIndex(id) != -1) return;
    Song newSong;
    newSong.id = id;
    newSong.title = title;
    newSong.artist = artist;
    newSong.genre = genre;
    SongEntry newEntry;
    newEntry.id = id;
    newEntry.data = newSong;
    songsList.push_back(newEntry);
    this->addVertex(id);
}

void MusicGraph::printSongInfo(const string& id) const {
    // TODO: Print the song information in the required format
    int idx = getSongIndex(id);
    if (idx == -1) return;
    cout<< "[" << songsList[idx].data.id << "] "
        << songsList[idx].data.title << " - "
        << songsList[idx].data.artist;
}

// =============================================================================
// REQUIREMENT 1: Recommend related songs (BFS)
// =============================================================================
void MusicGraph::recommendRelatedSongs(const string& startId) const {
    if (!this->hasVertex(startId)) return;

    cout << "\n[1] SUGGEST RELATED SONGS FOR: ";
    printSongInfo(startId);
    cout << "\n-------------------------------------------------\n";

    // TODO: Implement Breadth-First Search (BFS) to recommend related songs
    vector<string> visitedList;
    vector<string> customQueue;
    visitedList.push_back(startId);
    customQueue.push_back(startId);
    int head = 0;
    while (head < customQueue.size()){
        string currentId = customQueue[head];
        head++;
        getNeighbors(currentId);
        for (int i = 0; i < getNeighbors(currentId).size(); i++){
            string neighborId = getNeighbors(currentId)[i].target;
            if (!isVisited(neighborId, visitedList)){
                visitedList.push_back(neighborId);
                customQueue.push_back(neighborId);
                cout<<"-> ";
                printSongInfo(neighborId);
                cout << "\n";
            }
        }
    }
}

// =============================================================================
// REQUIREMENT 2: Create playlist by clusters (Connected Components using BFS)
// =============================================================================
void MusicGraph::generatePlaylistsByClusters() const {
    cout << "\n[2] CREATE PLAYLIST BY CLUSTERS (CONNECTED COMPONENTS)\n";
    cout << "-------------------------------------------------\n";

    // TODO: Find connected components and print each cluster as a playlist
    vector<string> visitedList;
    int playlistCount = 0;
    for (int i = 0; i < adjList.size(); i++){
        if (!isVisited(adjList[i].vertex, visitedList)){
            playlistCount++;
            cout<< "=== Playlist " << playlistCount << " ===\n";
            vector<string> customQueue;
            visitedList.push_back(adjList[i].vertex);
            customQueue.push_back(adjList[i].vertex);
            int head = 0;
            while (head < customQueue.size()){
                string currentId = customQueue[head];
                head++;
                cout<<"  * ";
                printSongInfo(currentId);
                cout << "\n";
                vector<Edge> neighbors = getNeighbors(currentId);
                for (int j = 0; j < neighbors.size(); j++){
                    string neighborId = neighbors[j].target;
                    if (!isVisited(neighborId, visitedList)){
                        visitedList.push_back(neighborId);
                        customQueue.push_back(neighborId);
                    }
                }
            }
        }
    }
}

// =============================================================================
// REQUIREMENT 3: Smooth song transition (Dijkstra's Algorithm)
// =============================================================================
void MusicGraph::findSmoothTransition(const string& startId, const string& endId) const {
    cout << "\n[3] SMOOTHEST TRANSITION (DIJKSTRA)\n";
    cout << "From: "; printSongInfo(startId); cout << "\n";
    cout << "To: "; printSongInfo(endId); cout << "\n";
    cout << "-------------------------------------------------\n";

    int n = this->adjList.size();
    int startIdx = this->getVertexIndex(startId);
    int endIdx = this->getVertexIndex(endId);

    if (startIdx == -1 || endIdx == -1) {
        cout << "Error: Song not found!\n";
        return;
    }

    // TODO: Implement Dijkstra's algorithm to find the shortest path between startId and endId
    vector<double> dist (n, 1e9);
    vector<int> prev (n, -1);
    vector<bool> visited (n, false);
    dist[startIdx] = 0;
    for (int i = 0; i < n; i++){
        int u = -1;
        for (int j = 0; j < n; j++){
            if (!visited[j] && (u == -1 || dist[j] < dist[u])) u = j;
        }
        if (dist[u] == 1e9) break;
        visited[u] = true;
        vector<Edge> neighbors = getNeighbors(adjList[u].vertex);
        for (const Edge& edge : neighbors) {
            int v = getVertexIndex(edge.target);
            if (!visited[v] && dist[u] + edge.weight < dist[v]) {
                dist[v] = dist[u] + edge.weight;
                prev[v] = u;
            }
        }
    }
    
    if (dist[endIdx] == 1e9) {
        cout << "No transition path between these two songs.\n";
        return;
    }
    
    vector<int> pathIndices;
    int curr = endIdx;
    while (curr != -1) {
        pathIndices.push_back(curr);
        curr = prev[curr];
    }
    
    int size = pathIndices.size();
    for (int i = 0; i < size / 2; i++) {
        int temp = pathIndices[i];
        pathIndices[i] = pathIndices[size - 1 - i];
        pathIndices[size - 1 - i] = temp;
    }
    
    cout << "-> Total Deviation (Cost): " << dist[endIdx] << "\n";
    cout << "-> Playback Order:\n";
    for (int i = 0; i < pathIndices.size(); i++) {
        cout << "   " << (i + 1) << ". ";
        printSongInfo(adjList[pathIndices[i]].vertex);
        cout << "\n";
    }
}


// =============================================================================
// REQUIREMENT 4: Find the network hub song (In-degree Calculation)
// =============================================================================
void MusicGraph::findMostPopularSong() const {
    cout << "\n[4] FIND NETWORK HUB SONG (IN-DEGREE)\n";
    cout << "-------------------------------------------------\n";

    // TODO: Calculate the in-degree of all vertices and find the one with the maximum value
    int n = this->adjList.size();
    vector<int> inDegree (n, 0);
    int maxIdx = 0;
    for (int i = 0; i < n; i++){
        for (int j = 0; j < adjList[i].neighbors.size(); j++){
            int v = getVertexIndex(adjList[i].neighbors[j].target);
            inDegree[v]++;
        }
    }
    for (int i = 0; i < n; i++){
        if (inDegree[i] > inDegree[maxIdx]) maxIdx = i;
    }
    cout << "-> Network Hub Song:\n   ";
    printSongInfo(adjList[maxIdx].vertex);
    cout << "\n   (In-degree: " << inDegree[maxIdx] << ")\n";
}

// =============================================================================
// REQUIREMENT 5: Detect music loop (DFS Cycle Detection)
// =============================================================================

// DFS recursive helper function

bool MusicGraph::dfsCycleHelper(int idx, vector<bool>& visited, vector<bool>& recursionStack,
                                vector<int>& parent, bool& found) const {
    // TODO: Implement the recursive DFS logic to detect cycles
    visited[idx] = true;
    recursionStack[idx] = true;
    
    for (int i = 0; i < adjList[idx].neighbors.size(); i++) {
        int neighborIdx = getVertexIndex(adjList[idx].neighbors[i].target);
        
        if (!visited[neighborIdx]) {
            parent[neighborIdx] = idx;
            if (dfsCycleHelper(neighborIdx, visited, recursionStack, parent, found)) return true;
        }
        else if (recursionStack[neighborIdx]) {
            found = true;
            
            cout << "-> Music loop detected!\n";
            cout << "-> Loop:\n";
            
            vector<int> cyclePath;
            cyclePath.push_back(neighborIdx);
            
            int curr = idx;
            while (curr != neighborIdx && curr != -1) {
                cyclePath.push_back(curr);
                curr = parent[curr];
            }
            cyclePath.push_back(neighborIdx);
            
            for (int j = cyclePath.size() - 1; j >= 0; j--) {
                cout << "   ";
                printSongInfo(adjList[cyclePath[j]].vertex);
                cout << "\n";
            }
            
            return true;
        }
    }
    
    recursionStack[idx] = false;
    return false;
}

void MusicGraph::detectMusicLoop() const {
    cout << "\n[5] DETECT MUSIC LOOP (DFS CYCLE DETECTION)\n";
    cout << "-------------------------------------------------\n";

    // TODO: Initialize required arrays and start DFS to detect a music loop
    int n = this->adjList.size();
    vector<bool> visited (n, false);
    vector<bool> recursionStack (n, false);
    vector<int> parent (n, -1);
    bool found = false;
    for (int i = 0; i < n; i++){
        if (!visited[i]){
            if (dfsCycleHelper(i, visited, recursionStack, parent, found)) return;
        }
    }
    if (!found) cout << "No music loop detected.\n";
}