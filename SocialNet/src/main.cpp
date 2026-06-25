#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>
#include <chrono>
using namespace std;


// ------------------- POST + AVL TREE -------------------
struct Post {
    long long timestamp;
    string content;

    Post(string text) {
        // current time in milliseconds
        timestamp = chrono::duration_cast<chrono::milliseconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();
        content = text;
    }
};

struct Node {
    Post post;
    Node* left;
    Node* right;
    int height;

    Node(string text)
        : post(text), left(nullptr), right(nullptr), height(0) {} //make before constructor
};

class AVLTree {
private:
    Node* root;

    int height(Node* n) {
        if(!n){
            return -1;
        } 
        return n->height;
    }

    int getBalance(Node* n) {
        if(!n){
            return 0;
        }
        return height(n->left) - height(n->right);
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;

        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        return y;
    }

    Node* insert(Node* node, Post p) {
        if(!node){
            return new Node(p.content);
        }
        if(p.timestamp < node->post.timestamp){
            node->left = insert(node->left, p);
        }
        else if(p.timestamp > node->post.timestamp){
            node->right = insert(node->right, p);
        }
        else{// same timestamp
            // compare content to avoid losing posts
            if(p.content < node->post.content){
                node->left = insert(node->left, p);
            }
            else{
                node->right = insert(node->right, p);
            }
        }
        node->height = max(height(node->left), height(node->right)) + 1;
        int balance = getBalance(node);

        // Left-Left
        if (balance > 1 && p.timestamp < node->left->post.timestamp){
            return rotateRight(node);
        }
        // Right-Right
        if (balance < -1 && p.timestamp > node->right->post.timestamp){
            return rotateLeft(node);
        }
        // Left-Right
        if (balance > 1 && p.timestamp > node->left->post.timestamp) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right-Left
        if (balance < -1 && p.timestamp < node->right->post.timestamp) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    void printLatest(Node* node, int &N) {
        if(!node || N <= 0){
            return;
        }
        // reverse in-order: newest first
        printLatest(node->right, N);
        if(N>0){
            cout<<node->post.timestamp<<": "<<node->post.content<<endl;
            N--;
        }
        printLatest(node->left, N);
    }

    int countNodes(Node* node) {
        if(!node){
            return 0;
        }
        return 1 + countNodes(node->left) + countNodes(node->right);
    }

public:
    AVLTree() {
        root = nullptr;
    }

    void insert(Post p) {
        root = insert(root, p);
    }

    void printLatestN(int N) {
        int total = countNodes(root);
        if(N==-1 || N>total){
            N = total;
        }
        printLatest(root, N);
    }
};


//---------------------SOCIALNET+GRAPH----------------------

class SocialNet{
private:
    unordered_map<string,vector<string>> adj;
    unordered_map<string,string> ognames;
    unordered_map<string,AVLTree> posts;

    string toLower(string s){
        for(char &c:s){
            c=tolower(c);
        }
        return s;
    }

public:
    void ADD_USER(string username){
        string lower=toLower(username);

        if(adj.find(lower)==adj.end()){
            adj[lower]={};
            ognames[lower]=username;
            cout<<"Added User: "<<username<<" Successfully"<<endl;
        }else{
            cout<<"User Already Exists"<<endl;
        }
        cout<<endl;
    }    

    void LIST_FRIENDS(string username){
        string lower=toLower(username);

        if(adj.find(lower)==adj.end()){
            cout<<"Username Does Not Exist"<<endl;
            cout<<endl;
            return;
        }
        vector<string> friends=adj[lower];
        if(friends.size()==0){
            cout<<"No Friends Exist"<<endl;
            cout<<endl;
            return;
        }
        sort(friends.begin(),friends.end());
        cout<<"Friends of "<<ognames[lower]<<" are: ";
        for(int i=0;i<friends.size();i++){
            cout<<ognames[friends[i]]<<" ";
        }
        cout<<endl;
        cout<<endl;
        return;
    }

    void ADD_FRIEND(string username1, string username2){
        string u1 = toLower(username1);
        string u2 = toLower(username2);

        if(adj.find(u1)==adj.end() || adj.find(u2)==adj.end() ){
            cout<<"One or Both of Users does Not Exist"<<endl;
            cout<<endl;
            return;
        }
        if(u1==u2){
            cout<<"Can not friend yourself"<<endl;
            cout<<endl;
            return;
        }
        vector<string> &friends1 = adj[u1];
        for (string f : friends1) {
            if (f==u2) {
                cout<<ognames[u1]<<" and "<<ognames[u2]<<" are already friends"<< endl;
                cout<<endl;
                return;
            }
        }
        adj[u1].push_back(u2);
        adj[u2].push_back(u1);
        cout<<ognames[u1]<<" and "<<ognames[u2]<<" are now friends"<< endl;
        cout<<endl;
    }

    int DEGREES_OF_SEPARATION(string username1, string username2){
        string u1 = toLower(username1);
        string u2 = toLower(username2);

        if(adj.find(u1)==adj.end() || adj.find(u2)==adj.end() ){
            cout<<"One or Both of Users does Not Exist"<<endl;
            cout<<endl;
            return -1;
        }
        if(u1==u2){
            cout<<endl;
            return 0;
        }
        unordered_map<string,bool> visited;
        unordered_map<string,int> degree;
        queue<string> q;

        q.push(u1);
        visited[u1]=true;
        degree[u1]=0;

        while(!q.empty()){
            string current=q.front();
            q.pop();

            for(string fr:adj[current]){
                if(!visited[fr]){
                    q.push(fr);
                    visited[fr]=true;
                    degree[fr]=degree[current]+1;

                    if(fr==u2){
                        cout<<"Degree of Separation between "<<ognames[u1]<<" and "<<ognames[u2]<<" is "<< degree[fr]<<endl;
                        cout<<endl;
                        return degree[fr];
                    }
                }
            }
        }
        cout<<"No Connection between "<<ognames[u1]<<" and "<<ognames[u2]<<endl;
        cout<<endl;
        return -1;
    }

    void SUGGEST_FRIENDS(string username, int N){

        string u=toLower(username);

        if(adj.find(u)==adj.end()){
            cout<<"Username Does Not Exist"<<endl;
            cout<<endl;
            return;
        }
        if(N<=0){
            cout<<endl;
            return;
        }
        unordered_map<string,bool> directfriend;
        unordered_map<string,int> mutual;

        directfriend[u]=true;
        for(string f:adj[u]){
            directfriend[f]=true;
        }

        for(string f:adj[u]){
            for(string fof:adj[f]){
                if(!directfriend[fof]){
                    if (mutual.find(fof) == mutual.end()){
                        mutual[fof] = 1;
                    }else{
                        mutual[fof] = mutual[fof] + 1;
                    }    
                }
            }
        }
        if (mutual.empty()) {
            cout<<"No friend suggestions for "<<ognames[u]<<endl;
            cout<<endl;
            return;
        }

        vector<pair<string,int>> suggestions;
        for (auto it = mutual.begin(); it != mutual.end(); it++) {
            suggestions.push_back(make_pair(it->first, it->second));
        }
        sort(suggestions.begin(), suggestions.end(), [&](pair<string, int> a, pair<string, int> b) {
            if(a.second!=b.second){
                return a.second>b.second; 
            }
            return ognames[a.first]<ognames[b.first]; 
        });
        
        cout<<"Friend suggestions for "<<ognames[u]<<":"<<endl;
        int count=0;
        for (int i=0;i<suggestions.size() && count<N;i++) {
            string name=suggestions[i].first;
            int mutuals=suggestions[i].second;

            cout<<ognames[name]<<" ("<<mutuals<<" mutual friends)"<<endl;
            count++;
        }
        cout<<endl;
    }

    void ADD_POST(string username, string content){
        string u=toLower(username);

        if(adj.find(u)==adj.end()){
            cout<<"Username Does Not Exist"<<endl;
            cout<<endl;
            return;
        }

        Post newPost(content);
        posts[u].insert(newPost);

        cout<<"Post Added for "<<ognames[u]<<" Successfully"<<endl;
        cout<<endl;
    }

    void OUTPUT_POSTS(string username,int N) {
        string u = toLower(username);

        if(adj.find(u)==adj.end()) {
            cout<<"User does not exist"<<endl;
            cout<<endl;
            return;
        }

        cout<<"Latest posts by "<<ognames[u]<<":"<<endl;
        posts[u].printLatestN(N);
        cout<<endl;
}


};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    SocialNet sn;
    string command;
    string username;
    while (true) {
        cin >> command;

        if(command=="ADD_USER") {
            cin >> username;
            sn.ADD_USER(username);
        }

        else if(command=="LIST_FRIENDS") {
            cin >> username;
            sn.LIST_FRIENDS(username);
        }
        
        else if(command=="ADD_FRIEND"){
            string username1,username2;
            cin>>username1>>username2;
            sn.ADD_FRIEND(username1,username2);
        }

        else if (command == "DEGREES_OF_SEPARATION") {
            string username1, username2;
            cin >> username1 >> username2;
            sn.DEGREES_OF_SEPARATION(username1, username2);
        }

        else if(command=="SUGGEST_FRIENDS") {
            string username;
            int N;
            cin>>username>>N;
            sn.SUGGEST_FRIENDS(username,N);
        }

        else if(command=="ADD_POST") {
            string username;
            cin>>username;

            cin.ignore(); // flush leftover newline

            string content;
            getline(cin,content); 

            sn.ADD_POST(username,content); 
        }   

        else if (command=="OUTPUT_POSTS"){
            string username;
            int N;
            cin >> username >> N;
            sn.OUTPUT_POSTS(username,N);
        }
        
        else if(command=="EXIT") {
            break;
        }
    }
    return 0;
}
