#pragma once
#include "triangle.h"

class Polygon {
private:
public:
	std::vector<Vertex> vertices;
	Color color;

	Polygon() : vertices(0) {
		Vertex vet = random_vertex(Tools::Max_Width, Tools::Max_Height);
		FOR(i, 3) {
			Vertex vn;
			vn.x = vet.x + Tools::random_int(-3, 4);
			vn.y = vet.y + Tools::random_int(-3, 4);
			vn.x = Tools::clamp(vn.x, 0, Tools::Max_Width - 1);
			vn.y = Tools::clamp(vn.y, 0, Tools::Max_Height - 1);
			vertices.push_back(vn);
		}
		color = random_color();
	}

	size_t size() const { return vertices.size(); }
	Vertex &operator[](int index) {
		CHECK(index >= 0 && index < vertices.size());
		return vertices[index];
	}
	const Vertex &operator[](int index) const {
		CHECK(index >= 0 && index < vertices.size());
		return vertices[index];
	}

	void add();
	void remove();
	//void move();
	void mutate(bool vertex = true, bool color = true);

	void render_on(cv::Mat canvas, double render_scale = 1) const;
};

class Polygons {
private:
	std::vector<Polygon> list;

public:
	Polygons() : list(0) {}
	Polygons(const Polygons &cp) : list(cp.list) {}

	void push_back(const Polygon &tr) {
		list.push_back(tr);
	}
	void pop_back() {
		list.pop_back();
	}

	size_t size() const { return list.size(); }
	size_t size_of_vertices() const {
		size_t sum = 0;
		for (int i = 0; i < list.size(); ++i) {
			sum += list[i].size();
		}
		return sum;
	}
	Polygon &operator[](int index) {
		CHECK(index >= 0 && index < list.size());
		return list[index];
	}
	const Polygon &operator[](int index) const {
		CHECK(index >= 0 && index < list.size());
		return list[index];
	}

	void add();
	void remove();
	void move();
	void mutate();
	cv::Mat render(double render_scale = 1) const;
	double difference_with(cv::Mat &image) {
		auto img = render();
		return difference_between(img, image);
	}
};