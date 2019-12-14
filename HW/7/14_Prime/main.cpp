#include <fstream>
#include <vector>
#include <list>
#include <limits>
#include <set>


int main() {
    std::ifstream in_stream;
    std::ofstream out_stream;
    in_stream.open("kruskal.in");
    out_stream.open("kruskal.out");
    
    int n, m;
    in_stream >> n >> m;
    
    // read graph
    int from_v, to_v, weight;
    std::vector<std::list<std::pair<int, int> > > graph(n);
    for (int i = 0; i < m; ++i)
    {
        in_stream >> from_v >> to_v >> weight;
        graph[from_v - 1].push_back({to_v - 1, weight});
        graph[to_v - 1].push_back({from_v - 1, weight});
    }
    
    // find MST
    std::vector<bool> used(n, false);
    std::vector<int> min_weight_to_span(n, std::numeric_limits<int>::max());
    min_weight_to_span[0] = 0;
    std::set<std::pair<int, int> > min_weight_from_vertex_set;
    min_weight_from_vertex_set.insert ({0, 0});
    long long int mst_weight = 0;
    for (int i = 0; i < n; ++i)
    {
        // find vertex with min weight to span
        int vertex = min_weight_from_vertex_set.begin() -> second;
        min_weight_from_vertex_set.erase(min_weight_from_vertex_set.begin());
        
        // add vertex to span
        mst_weight += min_weight_to_span[vertex];
        used[vertex] = true;
    
        // update min_weight_to_span with vertex
        for (auto vertex_weight_pair: graph[vertex])
        {
            int cur_v = vertex_weight_pair.first;
            if (used[cur_v])
                continue;
            
            weight = vertex_weight_pair.second;
            if (weight < min_weight_to_span[cur_v])
            {
                min_weight_from_vertex_set.erase({min_weight_to_span[cur_v], cur_v});
                min_weight_to_span[cur_v] = weight;
                min_weight_from_vertex_set.insert({min_weight_to_span[cur_v], cur_v});
            }
            min_weight_to_span[vertex_weight_pair.first] = std::min(
                    min_weight_to_span[vertex_weight_pair.first],
                    vertex_weight_pair.second
            );
        }
    }
    
    out_stream << mst_weight;
    
    // clean
    in_stream.close();
    out_stream.close();
    
    return 0;
}