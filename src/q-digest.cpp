#include <bits/stdc++.h>

using namespace std;

class Node {
  public:
    int64_t weight;
    Node* left;
    Node* right;
    int32_t left_bound;
    int32_t right_bound;
    Node(int w) {
      weight = w;
    }
};

class QDigest {
  private:
    Node* construct_tree(int32_t lb, int32_t rb){
      Node* v = new Node(0);
      node_counter++;
      v->left_bound = lb;
      v->right_bound = rb;
      if(lb == rb){
        return v;
      }
      int mid = lb + (rb - lb)/2;
      v->left = construct_tree(lb, mid);
      v->right = construct_tree(mid + 1, rb);
      return v;
    }

    int64_t totalSubTreeWeight(Node* v) {
      int64_t total = 0;
      if(!(v->left == NULL && v->right == NULL))
        total += totalSubTreeWeight(v->left) + totalSubTreeWeight(v->right);
      total += v->weight;
      return total;
    }

    void setAllSubNodes(Node* v){
      if(v->left != NULL)
        setAllSubNodes(v->left);
      if(v->right != NULL)
        setAllSubNodes(v->right);
      delete v;
      node_counter--;
    }

  public:
    Node* root;
    int64_t total_weight;
    int64_t weight;
    int64_t capacity;
    uint64_t node_counter = 0;
    int32_t _U;
    double _eps;

    void init(double eps, int32_t U) {
      root = new Node(0);
      node_counter++;
      total_weight = 0;
      _eps = eps;
      _U = pow(2, ceil(log2(U)));
      root = construct_tree(0, U);
    }

    bool isLeftSubTree(Node* v, int32_t x) {
      if(x <= v->left->right_bound) return true;
      else return false;
    }

    void update(int32_t x, int64_t w) {
      Node *v = root;
      total_weight += w;
      capacity = ceil(((double)_eps*total_weight)/(log2(_U)));
      while(!(v->left == NULL && v->right == NULL)) {
        if(v->weight < capacity){
          int64_t d = min(capacity - v->weight, w);
          v->weight += d;
          w = w - d;
        }
        if(!w) break;
        if(isLeftSubTree(v, x)){
          v = v->left;
        }
        else{
          v = v->right;
        }
      }
      v->weight += w;
    }

    int64_t query(int32_t x) {
      Node* v = root;
      int64_t r = 0;
      while(!(v->left == NULL && v->right == NULL)) {
        Node* u = v->left;
        if(isLeftSubTree(v, x)){
          v = u;
        }
        else{
          r = r + totalSubTreeWeight(u);
          v = v->right;
        }
      }
      return r;
    }

    void compress(Node* v, int64_t d = 0) {
      capacity = ceil(((double)_eps*total_weight)/(log2(_U)));
      if(v->weight == 0) return;
      if((v->left == NULL && v->right == NULL)) {
        v->weight = v->weight - d;
      }
      else {
        if(((totalSubTreeWeight(v) - d)) > capacity) {
          d = d + capacity - v->weight;
          v->weight = capacity;
          Node* u = v->left;
          int64_t totalUweight = totalSubTreeWeight(u);
          compress(v->left, min(d, totalUweight));
          compress(v->right, max(d - totalUweight, (int64_t)0));
        }
        else {
          v->weight = totalSubTreeWeight(v) - d;
          setAllSubNodes(v->right);
          setAllSubNodes(v->left);
          v->right = NULL;
          v->left = NULL;
        }
      }
    }
    int64_t post_traversal(int64_t target_rank, int64_t &initial_rank, Node* v) {
      if(v == NULL) return 0;
      
      int64_t quantile_estimate = post_traversal(target_rank, initial_rank, v->left);
      if(initial_rank >= target_rank) return quantile_estimate;
      quantile_estimate = post_traversal(target_rank, initial_rank, v->right);
      if(initial_rank >= target_rank) return quantile_estimate;

      initial_rank += v->weight;
      quantile_estimate = v->right_bound;
      return quantile_estimate;
    }

    int32_t quantil(int64_t target){
      int64_t initial = 0;
      return post_traversal(target, initial, root);
    }
};

void printBT(const std::string& prefix, const Node* node, bool isLeft)
{
    if( node != nullptr )
    {
        std::cout << prefix;

        std::cout << (isLeft ? "├──" : "└──" );

        std::cout << node->left_bound << ' ' << node->right_bound << std::endl;
        
        printBT( prefix + (isLeft ? "│   " : "    "), node->left, true);
        printBT( prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}

void printBT(const Node* node)
{
    printBT("", node, false);    
}


int main(int argc, char **argv) {
  double eps;
  map<int64_t, int64_t> true_rank;
  int32_t U;
  int target_col;
  string query_file;
  string input_file;
  uint64_t cnt = 0;
  bool isRank = false;
  bool isQuant = false;
  for(int i = 0; i < argc; i++) {
    if(!strcmp(argv[i], "--eps")){
      eps = stod(argv[i+1]);
      i++;
    }
    else if(!strcmp(argv[i], "--val")) {
      target_col = stoi(argv[i+1]);
      i++;
    }
    else if(!strcmp(argv[i],"--help")) {
      ifstream help_file("q-digest.txt");
      string line;
      while(getline(help_file, line)){
        cout << line << endl;
      }
      return 0;
    }
    else if(!strcmp(argv[i], "--univ")) {
      U = stoul(argv[i+1]); 
      i++;
    }
    else if(!strcmp(argv[i], "--in")) {
      query_file = argv[i+1];
      i++;
    }
    else if(!strcmp(argv[i], "rank")) {
      isRank = true;
    }
    else if(!strcmp(argv[i], "quant")) {
      isQuant = true;
    }
    else{
      input_file = argv[i];
    }
  }

  QDigest q;
  q.init(eps, U);

  ifstream network_flows(input_file);
  string line;
  time_t start, end;
  double total_time = 0.0;

  getline(network_flows, line);
  
  if(network_flows.is_open()) {
    while(getline(network_flows, line)) {
      stringstream column(line);
      int i_column = -1;
      while(getline(column, line, ',')) {
        i_column++;
        if(i_column == target_col) {
          int32_t x = stoul(line);
          int64_t w = 1;
          if(x <= U){
            q.update(x, w);
            true_rank[x] += w;
          }
          if(!(q.total_weight % 100000)){
            q.compress(q.root);
          }
          total_time += double(end-start);
        }
      }
    }
  }

  if(isRank){
    ifstream rank_queries(query_file);
    if(rank_queries.is_open()) {
      while(getline(rank_queries, line)) {
        int32_t x = stoul(line);
        int64_t qry;
        qry = q.query(x);
        cout << qry << endl;
      }
    }
  }
  else {
    ifstream quantile_queries(query_file);
    if(quantile_queries.is_open()) {
      while(getline(quantile_queries, line)) {
        int64_t qnt = stoul(line);
        int64_t qry;
        qry = q.quantil(qnt);
        cout << qry << endl;
        cout << q.query(qry) << endl;
      }
    }
  }
}
