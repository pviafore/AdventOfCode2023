#include <algorithm>
#include <ostream>
#include <span>
#include <unordered_map>
namespace graph {

    template <typename Node>
    class Graph {
    public:

        void addEdge(Node source, Node destination) {
            graph[source].push_back(destination);
        }

        friend std::ostream& operator<<(std::ostream& os, const Graph& graph) {
            for(auto [node, neighbors]: graph.graph){
                os << node << " is connected to: ";
                std::ranges::copy(neighbors, std::ostream_iterator<Node>(os, ", "));
                os << "\n";
            }
            return os;
        }

        std::vector<Node> at(const Node& node) const{
            if (! std::ranges::contains(graph | std::views::keys, node)){
                return {};
            }
            return graph.at(node);
        }

        std::vector<Node> getKeys() const {
            std::vector<Node> out;
            std::ranges::copy(graph | std::views::keys, std::back_inserter(out));
            return out;
        }


    private:
        std::unordered_map<Node, std::vector<Node>> graph;

    };

}