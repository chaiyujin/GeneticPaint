#pragma once
#include "triangle.h"
#include <string>

class Group;

class Chromosome {
	friend class Group;
private:
	byte *data;
	int n_triangle;
	int size_in_byte;
	int size_in_bit;
public:
	Chromosome() : data(NULL), n_triangle(0), size_in_byte(0), size_in_bit(0) {}
	Chromosome(const Triangles &triangles);

	cv::Mat render(int width, int height);
	void cross_with(Chromosome &chrom, int position, bool byte_mode = true);
	void mutate(int position, bool byte_mode = true);

	int size(bool byte_mode = true) {
		if (byte_mode) return size_in_byte;
		else return size_in_bit;
	}

	void destroy() {
		delete[] data;
	}
};

class Group {
private:
	static cv::Mat target_image;
	
	std::vector<Chromosome> chroms;
	std::vector<double> similarity;

	void calc_similarity();

public:
	static void init_target(std::string image_path);
	static Group first_generation(int n_chromosome, int n_triangles);

	Group() : chroms(0), similarity(0) {}

	Group evolve(float cross_rate, float mutate_rate);
	Group select();
	void cross(float rate);
	void mutate(float rate);

	Chromosome &operator[](int index) {
		CHECK(index >= 0 && index < chroms.size());
		return chroms[index];
	}
	const Chromosome &operator[](int index) const {
		CHECK(index >= 0 && index < chroms.size());
		return chroms[index];
	}

	int best_chromosome_index();
	double similarity_at(int index) {
		if (similarity.size() == 0) calc_similarity();
		CHECK(index >= 0 && index < similarity.size());
		return similarity[index];
	}
	size_t size() const { return chroms.size(); }
	cv::Mat render(int index) {
		CHECK(index >= 0 && index < chroms.size());
		return chroms[index].render(target_image.cols, target_image.rows);
	}

	void destroy() {
		FOR(i, chroms.size()) {
			chroms[i].destroy();
		}
		chroms.clear();
	}
};