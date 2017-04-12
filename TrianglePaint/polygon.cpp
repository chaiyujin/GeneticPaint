#include "polygon.h"
using namespace cv;

void Polygon::add() {
	if (vertices.size() < Tools::Max_Vertices) {
		int pos = Tools::random_int(1, vertices.size() - 1);
		auto &prev = vertices[pos - 1];
		auto &next = vertices[pos + 1];
		Vertex vet;
		vet.x = (prev.x + next.x) / 2;
		vet.y = (prev.y + next.y) / 2;
		vertices.push_back(Vertex());
		for (int i = pos; i + 1 < vertices.size(); ++i) {
			vertices[i + 1] = vertices[i];
		}
		vertices[pos] = vet;
	}
}

void Polygon::remove() {
	if (vertices.size() <= 3) return;
	int pos = Tools::random_int(0, vertices.size());
	for (int i = pos; i + 1 < vertices.size(); ++i) {
		vertices[i] = vertices[i + 1];
	}
	vertices.pop_back();
}

void Polygon::mutate(bool mutate_vertex, bool mutate_color) {
	if (Tools::will_mutate(Tools::Add_Vertex_Rate)) {
		add();
	}
	if (Tools::will_mutate(Tools::Remove_Vertex_Rate)) {
		remove();
	}
	FOR(i, vertices.size()) {
		vertices[i].mutate();
	}
		
	
	color.mutate();
}

void Polygon::render_on(cv::Mat canvas, double render_scale) const {

	std::vector<std::pair<int, int>> range;
	std::pair<int, int> min_max;
	min_max.first = canvas.cols + 1;
	min_max.second = -1;

	for (int k = 0; k < canvas.rows; ++k)
		range.push_back(min_max);

	for (int i = 0; i < size(); ++i) {
		int x1 = vertices[i].x * render_scale;
		int y1 = vertices[i].y * render_scale;
		int j = i + 1; if (j >= size()) j %= size();
		int x2 = vertices[j].x * render_scale;
		int y2 = vertices[j].y * render_scale;
		draw_line(x1, y1, x2, y2, range);
	}

	float alpha = color.a / 255.f;

	//#pragma omp parallel for schedule(dynamic)
	for (int r = 0; r < canvas.rows; ++r) {
		byte *data = canvas.ptr<byte>(r);
		min_max = range[r];
		for (int c = min_max.first; c <= min_max.second; ++c) {
			for (int k = 0; k < 3; ++k) {
				data[c * 3 + k] = (byte)(data[c * 3 + k] * (1 - alpha) + color[2 - k] * alpha);
			}
		}
	}
}

/*-------------------------------------*/
void Polygons::add() {
	list.push_back(Polygon());
	/*int pos = Tools::random_int(0, list.size());
	for (int i = list.size() - 1; i > pos; --i) {
		Polygon tmp = list[i];
		list[i] = list[i - 1];
		list[i - 1] = tmp;
	}*/
	/*if (list.size() == 0 || Tools::will_mutate(Tools::Add_New_Polygon_Rate)) {
	}
	else {
		int pos = Tools::random_int(0, list.size());
		Polygon new_one = list[pos];
		new_one.mutate(true, false);
		list.push_back(new_one);
	}*/
}

void Polygons::remove() {
	if (list.size() == 0) return;
	int pos = Tools::random_int(0, list.size());
	for (int i = pos; i + 1 < list.size(); ++i) {
		list[i] = list[i + 1];
	}
	list.pop_back();
}

void Polygons::move() {
	if (list.size() < 3) return;
	int l = 0, r = 0;
	while (l == r) {
		l = Tools::random_int(0, list.size());
		r = Tools::random_int(0, list.size());
		if (l > r) { int t = l; l = r; r = t; }
	}
	Polygon tmp = list[l];
	for (int i = l; i + 1 <= r; ++i) {
		list[i] = list[i + 1];
	}
	list[r] = tmp;
}

void Polygons::mutate() {
	if (Tools::will_mutate(Tools::Add_Polygon_Rate)) {
		this->add();
	}

	if (Tools::will_mutate(Tools::Remove_Polygon_Rate)) {
		this->remove();
	}

	if (Tools::will_mutate(Tools::Move_Polygon_Rate)) {
		this->move();
	}
	for (int i = 0; i < list.size(); ++i)
		list[i].mutate();
}

Mat Polygons::render(double render_scale) const {
	Mat img(Tools::Max_Height * render_scale, Tools::Max_Width * render_scale, CV_8UC3, Scalar(0, 0, 0));
	for (int i = 0; i < list.size(); ++i) {
		list[i].render_on(img, render_scale);
	}
	return img;
}