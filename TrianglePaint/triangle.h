#pragma once

#include "utils/yuki.h"
#include "const.h"
#include <opencv2/opencv.hpp>
#include <vector>
typedef unsigned char byte;

double difference_between(cv::Mat img0, cv::Mat img1);
double similarity_between(cv::Mat img0, cv::Mat img1);

class Vertex {
private:

public:
	Vertex() : x(0), y(0) {}
	byte x, y;

	byte &operator[](int index) {
		CHECK(index >= 0 && index < 2);
		if (index == 0) return x;
		else return y;
	}
	const byte &operator[](int index) const {
		CHECK(index >= 0 && index < 2);
		if (index == 0) return x;
		else return y;
	}

	void mutate();
};

class Color {
public:
	Color() : r(0), g(0), b(0), a(0) {}
	byte r, g, b, a;

	byte &operator[](int index) {
		CHECK(index >= 0 && index < 4);
		if (index == 0) return r;
		else if (index == 1) return g;
		else if (index == 2) return b;
		else return a;
	}
	const byte &operator[](int index) const {
		CHECK(index >= 0 && index < 4);
		if (index == 0) return r;
		else if (index == 1) return g;
		else if (index == 2) return b;
		else return a;
	}

	void mutate();
};

// gene struct for triangle mesh
class Triangle {
private:
public:
	Triangle() {}
	Triangle(const Triangle &cp) {
		FOR(i, 3) v[i] = cp.v[i];
		color = cp.color;
	}
	Triangle &operator=(const Triangle &cp) {
		FOR(i, 3) v[i] = cp.v[i];
		color = cp.color;
		return *this;
	}
	// tree vertex
	Vertex v[3];
	// Color
	Color color;

	void mutate(bool vertex = true, bool color = true);
};

class Triangles {
private:
	std::vector<Triangle> list;

public:
	Triangles() : list(0) {}
	Triangles(const Triangles &cp) : list(cp.list) {}

	void push_back(const Triangle &tr) {
		list.push_back(tr);
	}

	size_t size() const { return list.size(); }
	Triangle &operator[](int index) {
		CHECK(index >= 0 && index < list.size());
		return list[index];
	}
	const Triangle &operator[](int index) const {
		CHECK(index >= 0 && index < list.size());
		return list[index];
	}

	void add();
	void remove();
	void move();
	void mutate();
	cv::Mat render();
	double difference_with(cv::Mat &image) {
		auto img = render();
		return difference_between(img, image);
	}
};

Vertex   random_vertex(int width, int height);
Color	 random_color();
Triangle random_triangle(int width, int height);

void print(Triangle &tr);
void draw_line(int x1, int y1, int x2, int y2, std::vector<std::pair<int, int>> &range);
void render_triangle(cv::Mat canvas, const Triangle &triangle);
inline void render_triangles(cv::Mat canvas, const Triangles &triangles) {
	for (int i = triangles.size() - 1; i >= 0; --i) {
		render_triangle(canvas, triangles[i]);
	}
}


void allocate_and_copy(byte *&mem, const Triangles &triangle);
void copy_from_memory(Triangles &triangle, const byte *mem);

