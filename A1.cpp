#include <bits/stdc++.h>
#define EPSILON 'E'
using namespace std;

string inp , RE;
int n , m; // strlen

typedef struct node{
   bool flg;
   map<char , set<node*>> edges;
   node(){
       flg = false;
       edges.clear();
   };
} node;

// Simple base case NFA node
node *base_nfa(char label) {
   node *s = new node();
   node* f = new node();
   f->flg = true;
   s->edges[label].insert(f);
   return s;
}

bool dfs(node *cur , int id) {
   // EPSILON Transitions from node
//    cout << "ID : " << id << " " << cur->flg << '\n';
   if(cur->edges.count(EPSILON)) {
       for(auto to : cur->edges[EPSILON])
           if(dfs(to , id))
               return true;
   }
   if(id == n) 
       return cur->flg;
   char label = inp[id];
   if(!cur->edges.count(label))
       return false;
   // label transitions from node
   for(auto to : cur->edges[label]) {
       if(dfs(to , id + 1))
           return true;
   }
   return false;
}

set<node*> get_final(node *nfa) {
   set<node*>final_nodes;
   map<node* , int>vis;
   auto f = [&](auto &&f , node* cur) -> void{
       vis[cur] = 1;
       if(cur->flg)
           final_nodes.insert(cur);
       for(auto vals : cur->edges)
           for(auto to : vals.second)
               if(!vis.count(to))
                   f(f , to);
   };

   f(f , nfa);
   return final_nodes;
}

node* concatenate(node* nfa_1 , node* nfa_2) {
   set<node*>final_nodes = get_final(nfa_1);
   for(auto y : final_nodes) {
       assert(y->flg);
       y->flg = false;
       y->edges[EPSILON].insert(nfa_2);
   }
   return nfa_1;
}

node *star_operator(node *nfa) {
   set<node*>f = get_final(nfa);
   nfa->flg = true;
   for(auto x : f)
       x->edges[EPSILON].insert(nfa);
   return nfa;
}

node *or_operator(node *a1 , node *a2) {
   node *s = new node();
   s->edges[EPSILON].insert(a1);
   s->edges[EPSILON].insert(a2);
   node *f = new node();
   for(auto x : get_final(a1)) {
       x->flg = false;
       x->edges[EPSILON].insert(f);
   }
   for(auto x : get_final(a2)) {
       x->flg = false;
       x->edges[EPSILON].insert(f);
   }
   f->flg = true;
   return s;
}

node* plus_operator(node *a1) {
    set<node*>f = get_final(a1);
    for(auto x : f)
        x->edges[EPSILON].insert(a1);
    return a1;
}

node *build(int l , int r) {
    // base case 
    // cout << "building : " << l << ' ' << r << '\n';
    if(r - l == 2) {
        // cout << "sent base nfa at " << l << ' ' << r << " with symbol " << RE[l + 1] << '\n';
        return base_nfa(RE[l + 1]);
    }
    stack<int>st;
    vector<array<int , 2>>rbs , nxt;
    vector<int>depth(m);
    for(int i = l + 1 ; i < r ; i++) {
        if(RE[i] == '(') {
            if(!st.empty()) depth[i] = depth[st.top()] + 1;
            else depth[i] = 1;
            st.push(i);
        }
        else if(RE[i] == ')') {
            auto t = st.top();
            st.pop();
            rbs.push_back({t , i});
        }
    }
    node *cur = new node();
    int pre = -1;
    sort(begin(rbs) , end(rbs) , [&](auto a , auto b) {
        if(a[0] != b[0]) return a[0] < b[0];
        return a[1] > b[1];
    });
    
    for(auto &[u , v] : rbs) 
        if(depth[u] == 1) nxt.push_back({u , v});
    
    for(auto &[u , v] : nxt) {
        node* nfa = build(u , v);
        if(v + 1 < m) {
            if(RE[v + 1] == '*') nfa = star_operator(nfa);
            else if(RE[v + 1] == '+') nfa = plus_operator(nfa);
        }
        if(pre == -1) cur = nfa;
        else cur = concatenate(cur , nfa);
        pre = 0;
    }
    return cur;
}


int32_t main() {
    cin >> RE;
    cin >> inp;
    m = RE.length();
    node *s = build(0 , m - 1);
    string temp = inp;
    string ans = "";
    int lst = 0;
    while(lst < (int)temp.length()) {
        int mx_match = -1;
        for(int j = (int)temp.length() - lst  ; j >= 1 ; j--) {
            inp = temp.substr(lst , j);
            n = inp.length();
            if(dfs(s , 0)) {
                mx_match = j;
                break;
            }
        }
        if(mx_match == -1) {
            ans += "@";
            ans += temp[lst];
            lst++;
        } else {
            ans += "$";
            ans += temp.substr(lst , mx_match);
            lst = lst + mx_match;
        }
    }
    ans += "#";
    cout << ans << '\n';
}
