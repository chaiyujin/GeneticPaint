#pragma once

#include "triangle.h"
#include "polygon.h"

template <class T>
class Group;

template <class T>
class Chromosome {
public:
	T shape;
	double error;

	Chromosome() : error(INFINITY) {}
	Chromosome(const T &s) : shape(s), error(INFINITY) {}

	cv::Mat render() { shape.render(img); }

	void cross_with(Chromosome &chrom) {
		int len_this = chrom.shape.size();
		int len_that = shape.size();
		int len = len_this;
		if (len_that < len) len = len_that;
		int i = Tools::random_int(0, len);
		while (i < shape.size() || i < chrom.shape.size()) {
			if (i < shape.size() && i < chrom.shape.size()) {
				T tmp;
				tmp = shape[i];
				shape[i] = chrom.shape[i];
				chrom.shape[i] = tmp;
			}
			else if (i >= shape.size()) {
				shape.push_back(chrom.shape[i]);
			}
			else if (i >= chrom.shape.size()) {
				chrom.shape.push_back(shape[i]);
			}
			++i;
		}
		while (shape.size() > len_this) shape.pop_back();
		while (chrom.shape.size() > len_that) chrom.shape.pop_back();
	}
	void mutate() {	shape.mutate(); }
};

template <class T>
inline bool cmp(const Chromosome<T> &c0, const Chromosome<T> &c1) {
	return c0.error < c1.error;
}

template <class T>
class Group {
public:
	cv::Mat target_image;
	std::vector<Chromosome> chroms;

	Group(cv::Mat target) : target_image(target), chroms(0) {}
	void generate_chromosome(int n_chromosome) {
		FOR(i, n_chromosome) {
			push_back(Chromosome<T>());
		}
	}
	void push_back(const Chromosome<T> &n) {
		chroms.push_back(n);
		cv::Mat img = n.render();
		chroms[chroms.size() - 1].error = difference_between(img, target_image);
	}
	Chromosome &operator[](int index) {
		CHECK(index >= 0 && index < chroms.size());
		return chroms[index];
	}
	const Chromosome &operator[](int index) const {
		CHECK(index >= 0 && index < chroms.size());
		return chroms[index];
	}
	size_t size() const { return chroms.size(); }
	
	Group evolve(float cross_rate) {
		Group next(target_image);
		select(next);
		next.cross(cross_rate);
		next.mutate();
		Group all(target_image);
		for (int i = 0; i < chroms.size(); ++i) {
			all.chroms.push_back(chroms[i]);
		}
		for (int i = 0; i < next.size(); ++i) {
			all.push_back(next[i]);
		}
		sort(all.chroms.begin(), all.chroms.end(), cmp);
		while (all.size() > size()) all.chroms.pop_back();
		return all;
	}
	void select(Group &next) {
		for (int i = 0; i < chroms.size(); ++i) {
			next.chroms.push_back(chroms[i]);
		}
	}
	void cross(float rate) {
		Yuki::Random random;
		for (int i = 0; i < chroms.size(); ++i) {
			float rate = random.random<float>(1);
			if (rate <= cross_rate) {
				int mi = i, fi = i;
				while (mi == fi) {
					mi = random.random<int>(chroms.size());
					fi = random.random<int>(chroms.size());
				}
				auto &male = chroms[mi];
				auto &female = chroms[fi];
				male.cross_with(female);
			}
		}
	}
	void mutate() {
		FOR(i, chroms.size()) {
			chroms[i].mutate();
		}
	}
};
