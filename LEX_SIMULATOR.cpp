#include <bits/stdc++.h>
using namespace std;

#define len(s) (int)s.length()
#define EPSILON '#'

string inp , RE;
int n , m;

typedef struct node{
   bool flg;
   map<char , set<node*>> edges;
   node(){
       flg = false;
       edges.clear();
   };
} node;

node *base(char label) {
   node *s = new node();
   node* f = new node();
   f->flg = true;
   s->edges[label].insert(f);
   return s;
}

bool dfs(node *cur , int id) {
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
   for(auto to : cur->edges[label]) {
       if(dfs(to , id + 1))
           return true;
   }
   return false;
}

set<node*> get_final(node *nfa) {
   set<node*>nodes;
   map<node* , int>vis;
   function<void(node*)> f = [&](node* cur) {
       vis[cur] = 1;
       if(cur->flg)
           nodes.insert(cur);
       for(auto vals : cur->edges)
           for(auto to : vals.second)
               if(!vis.count(to))
                   f(to);
   };

   f(nfa);
   return nodes;
}

node* concatenate(node* nfa_1 , node* nfa_2) {
    node *s = new node();
    s->edges[EPSILON].insert(nfa_1);
    set<node *> nodes = get_final(nfa_1);
    for (auto y : nodes) {
        assert(y->flg);
        y->flg = false;
        y->edges[EPSILON].insert(nfa_2);
    }
    return s;
}

node *star(node *nfa) {
   set<node*>f = get_final(nfa);
   nfa->flg = true;
   for(auto u : f)
       u->edges[EPSILON].insert(nfa);
   return nfa;
}

node *or_operator(node *a1 , node *a2) {
   node *s = new node();
   s->edges[EPSILON].insert(a1);
   s->edges[EPSILON].insert(a2);
   node *f = new node();
   for(auto u : get_final(a1)) {
       u->flg = false;
       u->edges[EPSILON].insert(f);
   }
   for(auto u : get_final(a2)) {
       u->flg = false;
       u->edges[EPSILON].insert(f);
   }
   f->flg = true;
   return s;
}

node* plus_operator(node *a1) {
    set<node*>f = get_final(a1);
    for(auto u : f)
        u->edges[EPSILON].insert(a1);
    return a1;
}

node *build(int l , int r) {
    if(r - l == 2) 
        return base(RE[l + 1]);

    stack<int>st;
    vector<array<int , 2>>rbs , nxt;
    vector<int>depth(10 * m);
    for(int i = l + 1 ; i < r ; i++) {
        if(RE[i] == '(') {
            if(!st.empty()) 
                depth[i] = depth[st.top()] + 1;
            else depth[i] = 1;
            st.push(i);
        }
        else if(RE[i] == ')') {
            auto t = st.top();
            st.pop();
            rbs.push_back({t , i});
        }
    }
    
    for(auto u : rbs) 
        if(depth[u[0]] == 1) nxt.push_back(u);
    
    vector<node*>c;
    int lst = -1;
    for(auto u : nxt) {
        node* nfa = build(u[0] , u[1]);
        if(u[1] + 1 < m) {
            if(RE[u[1] + 1] == '*') nfa = star(nfa);
            else if(RE[u[1] + 1] == '+') nfa = plus_operator(nfa);
        }
        if(lst != -1 and RE[lst + 1] == '|') {
            auto bck = c.back();
            c.pop_back();
            nfa = or_operator(bck, nfa);
        }
        lst = u[1];
        c.push_back(nfa);
    }
    node* cur = c[0];
    for(int i = 1 ; i < (int)c.size() ; i++)
        cur = concatenate(cur, c[i]);
    return cur;
}


int32_t main() {
    freopen("input.txt" , "r" , stdin);
    freopen("output.txt" , "w+" , stdout);

    int q; cin >> q;
    vector<node*>nfa;
    for(int i = 0 ; i < q ; i++) {
        string x; cin >> x;
        RE = x;
        m = len(RE);
        node *s = build(0 , m - 1);
        nfa.push_back(s);
    }

    cin >> inp;
    string subs = inp , tokens;
    int lst = 0;

    while(lst < len(subs)) {
        int match = -1 , now = -1;
        for(int j = len(subs) - lst  ; j >= 1 ; j--) {
            inp = subs.substr(lst , j);
            n = len(inp);
            for(int i = 0 ; i < q ; i++) {
                if(match != -1) break;
                if(dfs(nfa[i] , 0)) {
                    match = j;
                    now = i;
                }
            }
        }
        if(match == -1) {
            tokens.push_back('@');
            tokens += subs[lst];
            lst++;
        } else {
            tokens.push_back('$');
            now++;
            tokens.push_back((char)('0' + now));
            lst = lst + match;
        }
    }
    tokens.push_back('#');
    cout << tokens << '\n';
}
