
#include <iostream>
#include <unordered_map>
#include <map>
#include <algorithm>

struct edge {
	int a, b;
	edge(int aa = -1, int bb = -1) :a(aa), b(bb) {}
	bool operator==(const edge & e)const 
	{
		return (a == e.a && b == e.b) || (a == e.b && b == e.a);
	}

	bool operator<(const edge& e)const 
	{
		return e.a < e.b;
	}
};

struct tri 
{
	edge t[3];
	tri() {}
	tri(edge e1, edge e2, edge e3) {
		t[0] = e1;
		t[1] = e2;
		t[2] = e3;
	}
};
std::unordered_map<int, tri> t2e;
std::map<edge, std::vector<int>> e2t;
std::vector<int> vis;
void swap(edge& e) {
	int t = e.a;
	e.a = e.b;
	e.b = t;
}
void make_tri(tri& t,const edge & first_edge) {
	bool change = false;
	for (int i = 0; i < 3; i++) {
		if (first_edge == t.t[i] && first_edge.a == t.t[i].a) {
			change = true;
			break;
		}
	}

	if(change)
	for (int i = 0; i < 3; i++) 
		swap(t.t[i]);

	std::sort(t.t, t.t + 3, [](const edge& e1, const edge& e2) {return e1.a < e2.a; });
	
}
void dfs(int id,const edge & e) {
	vis[id] = 1;
	make_tri(t2e[id], e);
	for (int i = 0; i < 3; i++) {
		const auto &e = t2e[id].t[i];
		int t1 = e2t[e][0];
		int t2 = e2t[e][1];
		int t;
		if (t1 != id)
			t = t1;
		if (t2 != id)
			t = t2;

		if (vis[t] == 0) {
			dfs(t, e);
		}
	}
}
int main()
{
	int n,m;
	std::cin >> n >> m;
	vis.resize(m);
	for (int i = 0; i < m++; i++) {
		int a, b, c;
		std::cin >> a >> b >> c;
		t2e[i] = tri(edge(a,b),edge(b,c),edge(c,a));
		e2t[edge(a, b)].push_back(i);
		e2t[edge(b, c)].push_back(i);
		e2t[edge(c, a)].push_back(i);
	}

	dfs(0,t2e[0].t[0]);

	for (int i = 0; i < m; i++) 
	{
		const auto& t = t2e[i].t;
		std::cout << t[0].a << " " << t[1].a << " " << t[2].a << std::endl;
	}

	return 0;

}