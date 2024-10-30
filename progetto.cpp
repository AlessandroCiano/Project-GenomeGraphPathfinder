#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <unordered_map>
#include <ctime>

using namespace std;

vector<string> sequence; //sequence of characters assciated to each node
vector<vector<int>> adj[3]; //first integeris adjacency list [0 = from_orientation, 1 = node, 2 = to_orientation], second is node number, third is adjacency list
vector<int> input_deg; //input degree of each node

vector<bool> visitato[2]; //for DFS
vector<bool> recStack[2];

vector<int> source; //source nodes
vector<vector<int>> destinations; //destination nodes

//I. Lettura e Creazione del Grafo

string filename = "example2var.gfa";

unordered_map<string, int> umap; //hash_table

int countNodes_Edges(){ //count number of nodes
    ifstream file(filename);
    if (file.is_open()){
        int i = 0;
        string line;
        while (getline(file, line)){
            if (line[0] == 'S'){
                i++;
            }
        }
        file.close();
        return i;
    }
    else std::cout << "Error: file not found" << endl;
    return 0;
}
//readNodes and readEdges are separated because there's no guarantee that the nodes will be read before the edges
void readNodes(vector<string>& sequence) {
    ifstream file(filename);
    if (file.is_open()){
        int i = 0;
        string line, id;
        while (getline(file, line)){
            if (line[0] == 'S'){
                int j = 2;
                while (line[j] != '\t'){
                    id.push_back(line[j]);
                    j++;
                }
                umap[id] = i;
                id.clear();
                j++;
                sequence.push_back("");//make new element of sequence vector
                while (line[j] != '\t' && line[j] != '\n' && line[j] != '\0'){
                    sequence[sequence.size()-1].push_back(line[j]);
                    j++;
                }
                i++; 
            }
        }
        file.close(); // Close the file when you're done with it
    } 
    else std::cout << "Error: file not found" << endl;  
}

void readEdges (int N, vector<vector<int>> adj[3], vector<int>& input_deg){
    //make input_deg and adj of size = N
    input_deg.resize(N, 0);
    for (auto i = 0; i < 3; i++){
        adj[i].resize(N);
    }
    ifstream file(filename);
    if (file.is_open()){
        string line;
        while (getline(file, line)){
            if (line[0] == 'L'){
                int j = 2;
                string from;
                int from_orientation;
                string to;
                int to_orientation;

                while (line[j] != '\t'){
                    from.push_back(line[j]);
                    j++;
                }
                j++;
                while (line[j] != '\t'){
                    if (line[j] == '+'){
                        from_orientation = 0;
                    }
                    else if (line[j] == '-'){
                        from_orientation = 1;
                    }
                    j++;
                }
                j++;
                while (line[j] != '\t'){
                    to.push_back(line[j]);
                    j++;
                }
                j++;
                while (line[j] != '\t' && line[j] != '\n' && line[j] != '\0'){
                    if (line[j] == '+'){
                        to_orientation = 0;
                    }
                    else if (line[j] == '-'){
                        to_orientation = 1;
                    }
                    j++;
                }
                adj[0][umap[from]].push_back(from_orientation);
                adj[1][umap[from]].push_back(umap[to]);
                input_deg[umap[to]]++;
                adj[2][umap[from]].push_back(to_orientation);
            }
        }
        file.close();
    }
    else std::cout << "Error: file not found" << endl;
}

void printNodes(int N, vector<string> sequence, vector<int> input_deg){
    for (auto i = 0; i < N; i++){
        std::cout << i << "\t: " << sequence[i] << ",\t"<< "input_deg = " << input_deg[i] << endl;
    }
}

void printEdges(int N, vector<vector<int>> adj[3]){
    for (auto i = 0; i < N; i++){
        std::cout << i << "\t: ";
        for (auto j = 0; j < adj[0][i].size(); j++){
            std::cout << adj[0][i][j] << ", ";
            std::cout << adj[1][i][j] << ", ";
            std::cout << adj[2][i][j] << "; ";
        }
        std::cout << endl;
    }
}

//II. Analisi del Grafo

//verify if u is connected to p, if it is, there is a cycle and remove the back-edge making the graph direct
bool DFS_cycle_d(int u, vector<vector<int>> adj[3], vector<int>& input_deg, vector<bool> visitato[2], vector<bool> recStack[2], int pm){  //u = node of search -> is there p again?
    if (visitato[pm][u] == false){
        visitato[pm][u] = true; // mark the current node as visited
        recStack[pm][u] = true; // mark the current node as part of recursion stack 
        //bool risultato = false; 
        // Recur for all the vertices adjacent to this vertex 
        if (adj[0][u].size() == 0){
            return false;
        }

        for (int i = 0; i < adj[0][u].size(); i++){
            //verify cycle for acyclic graph
            if (!visitato[pm][adj[1][u][i]] && (adj[0][u][i] == pm)){
                if (DFS_cycle_d(adj[1][u][i], adj, input_deg, visitato, recStack, adj[2][u][i])){
                    return true;
                }
            } else if (recStack[0][adj[1][u][i]] || recStack[1][adj[1][u][i]]){ //if the node is already in the recursion stack
                //remove the edge and adjust input_deg
                for (int k = 0; k < 3; k++){
                    adj[k][u][i] = adj[k][u].back();
                    adj[k][u].pop_back();
                }
                input_deg[adj[1][u][i]]--;
                return true;  
            }   
        }
    }
    return false;
}

bool cycle_d(int N, vector<vector<int>> adj[3], vector<int>& input_deg, vector<bool> visitato[2], vector<bool> recStack[2]){
    visitato[0].resize(N);
    visitato[1].resize(N);
    recStack[0].resize(N);
    recStack[1].resize(N);
    bool risultatofin = false;
    for (auto j = 0; j < N; j++){
            visitato[0][j] = false;
            visitato[1][j] = false;
            recStack[0][j] = false;
            recStack[1][j] = false;
    }
    for (auto i = 0; i< N; i++){
        if (DFS_cycle_d(i, adj, input_deg, visitato, recStack, 0)){
            risultatofin = true;
        }
        if (DFS_cycle_d(i, adj, input_deg, visitato, recStack, 1)){
            risultatofin = true;
        }
    }
    return risultatofin;
}

//III. Stampa dei Cammini del DAG

void fillSource(int N, vector<int> input_deg, vector<int>& source){
    for (auto i = 0; i < N; i++){
        if (input_deg[i] == 0){
            source.push_back(i);
        }
    }
}

void DFS_dest(int source, int u, vector<vector<int>> adj[3], vector<bool> visitato[2], vector<vector<int>>& destinations, int pm){
    visitato[pm][u] = true;
    int length = adj[0][u].size();
    if (length == 0){
        destinations[source].push_back(u);
    } else {
        for (int i = 0; i < length; i++){
            if(!visitato[pm][adj[1][u][i]] && (adj[0][u][i] == pm)) {
                DFS_dest(source, adj[1][u][i], adj, visitato, destinations, adj[2][u][i]);
            }
        }
    }
    
}

void fillDestinations(int N, vector<vector<int>> adj[3], vector<bool> visitato[2], vector<int> source, vector<vector<int>>& destinations){
    visitato[0].resize(N);
    visitato[1].resize(N);
    destinations.resize(source.size());

    if (source.size() == 0){
        std::cout << "Non ci sono destinazioni perché non ci sono sorgenti" << endl;
    }
    for (auto i = 0; i < source.size(); i++) {
        for (auto j = 0; j < N; j++) {
            visitato[0][j] = false;
            visitato[1][j] = false;
        }
        DFS_dest(i, source[i], adj, visitato, destinations, 0);
        DFS_dest(i, source[i], adj, visitato, destinations, 1);
    }
}

void DFS_paths(int& stampe, int x, vector<string> sequence, string path, int u, int d, vector<vector<int>> adj[3], int pm){ //goes deep untill d in every way, then prints string
    if (u == d){
        if (pm == 0){
            path += sequence[u];
        } else{
            for (int j = sequence[u].size()-1; j >= 0; j--){
                if (sequence[u][j] == 'A'){
                    path.push_back('T');
                }
                else if (sequence[u][j] == 'T'){
                    path.push_back('A');
                }
                else if (sequence[u][j] == 'C'){
                    path.push_back('G');
                }
                else if (sequence[u][j] == 'G'){
                    path.push_back('C');
                }    
            }
        }
        std::cout << "stampa n. "<< (stampe+1) << endl;
        std::cout << path << ";" << endl;
        stampe++;
        if (stampe == x){
            return;
        }
    } else {
        for (int i = 0; i < adj[1][u].size(); i++){
            if (adj[0][u][i] == pm){
                string original_path = path;
                if (pm == 0){
                    path += sequence[u];
                } else{
                    for (int j = sequence[u].size()-1; j >= 0; j--){
                        if (sequence[u][j] == 'A'){
                            path.push_back('T');
                        }
                        else if (sequence[u][j] == 'T'){
                            path.push_back('A');
                        }
                        else if (sequence[u][j] == 'C'){
                            path.push_back('G');
                        }
                        else if (sequence[u][j] == 'G'){
                            path.push_back('C');
                        }    
                    }
                }
                if (stampe == x){
                return;
                }
                DFS_paths(stampe, x, sequence, path, adj[1][u][i], d, adj, adj[2][u][i]);
                path = original_path; //restore the path to its state before the recursive call
            }
        }
    }
}

void printxPaths(int x, vector<string> sequence, int s, int d, vector<vector<int>> adj[3]){ //s = source, d = destination
    if (x == 0){
        return;
    }
    if (s == d){
        std::cout << "Il nodo sorgente è anche il nodo destinazione: " << sequence[s] << endl;
        return;
    }
    if (adj[1][s].size() == 0){
        std::cout << "Nessun percorso parte da "<< s << "figuriamoci arrivare in d" << endl;
        return;
    }
    int stampe = 0;
    string pathp;
    for (int j = 0; j < sequence[s].size(); j++){
                pathp.push_back(sequence[s][j]);
            }
    string pathm;
    for (int j = sequence[s].size()-1; j >= 0; j--){
        if (sequence[s][j] == 'A'){
            pathm.push_back('T');
        }
        else if (sequence[s][j] == 'T'){
            pathm.push_back('A');
        }
        else if (sequence[s][j] == 'C'){
            pathm.push_back('G');
        }
        else if (sequence[s][j] == 'G'){
            pathm.push_back('C');
        }    
    }
    for (auto i = 0; i < adj[1][s].size(); i++){
        if (adj[0][s][i] == 0){
            DFS_paths(stampe, x, sequence, pathp, adj[1][s][i], d, adj, adj[2][s][i]);
        } else {
            DFS_paths(stampe, x, sequence, pathm, adj[1][s][i], d, adj, adj[2][s][i]);
        }
    }
}


int main(){
    readNodes(sequence);
    int N = sequence.size();
    std::cout << "N = " << N << endl;

    readEdges(N, adj, input_deg);

    //printNodes(N, sequence, input_deg); //nodes and relative input_deg before removing back-edges
    //printEdges(N, adj);

   if (cycle_d(N, adj, input_deg, visitato, recStack))
        std::cout << "Era un grafo ciclico, ora non lo è più" << endl;
    else
        std::cout << "Era e rimane un grafo aciclico" << endl;

    fillSource(N, input_deg, source);

    //sources
    if (source.size() == 0){
        std::cout << "Non ci sono sorgenti [fine programma]" << endl;
        return 0;
    }
    else {
        std::cout << "Ci sono " << source.size() << " sorgenti"<< endl;
    }

    fillDestinations(N, adj, visitato, source, destinations);

    //s = source, d = destination
    srand(time(NULL));
    int rands = rand() % source.size(); //random sourse in range
    int s = source[rands];
    int randd = rand() % destinations[rands].size(); //random destination in range
    int d = destinations[rands][randd];

    std::cout << "Percorsi da " << s << " a " << d << ": " << endl;

    int x = 42; //number of paths to print
    printxPaths(x, sequence, s, d, adj);
    

    return 0;
}