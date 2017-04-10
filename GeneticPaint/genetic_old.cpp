#include "genetic.h"
using namespace cv;
using namespace std;

Mat Chromosome::target;

Chromosome::Chromosome(vector<Triangle> &triangles) {
	// allocate the memory
	this->length = sizeof(Triangle) * triangles.size();
	this->triangles = triangles.size();
	this->gene = new unsigned char[length];
	memcpy(this->gene, triangles.data(), length);
}

Chromosome::~Chromosome() {
	// get back memory
	//delete[] gene;
}

Mat Chromosome::render() {
	Mat image = Mat::zeros(target.rows, target.cols, CV_8UC3);
	for (size_t i = 0; i < triangles; ++i) {
		Triangle tr;
		memcpy(&tr, &gene[i * sizeof(Triangle)], sizeof(Triangle));
		for (int i = 0; i < 6; i += 2) {
			if (tr.v[i] >= target.cols) tr.v[i] = target.cols - 1;
			if (tr.v[i + 1] >= target.rows) tr.v[i + 1] = target.rows - 1;
		}
		for (int i = 0; i < 3; ++i) {
			if (tr.c[i] > 255) tr.c[i] = 255;
		}
		if (tr.alpha < 0) tr.alpha = 0;
		if (tr.alpha > 1) tr.alpha = 1;
		render_triangle(image, tr);
	}
	/*cv::imshow("tmp", image);
	cv::waitKey();*/
	return image;
}

float Chromosome::ability_to_adapt() {
	Mat image = render();
	double error = 0.0;
	for (int r = 0; r < image.rows; ++r) {
		unsigned char *img_data = image.ptr<unsigned char>(r);
		unsigned char *tar_data = target.ptr<unsigned char>(r);
		for (int c = 0; c < image.cols; ++c) {
			for (int k = 0; k < 3; ++k) {
				int delta = abs((int)img_data[c * 3 + k] - (int)tar_data[c * 3 + k]);
				float err = (float)delta / 255.f;
				//printf("%f\n", err);
				error += err;
			}
		}
	}
	error /= 3.;
	error /= image.rows * image.cols;
	this->p = 1.f - (float)error;
	return this->p;
}

void Chromosome::init(std::string target_image) {
	target = imread(target_image);
}

void Chromosome::select(Group &population) {
	FOR(i, population.size()) {
		population[i].ability_to_adapt();
	}
	vector<float> p_sum(population.size());
	vector<bool> mask(population.size());
	p_sum[0] = population[0].p;
	mask[0] = false;
	for (size_t i = 1; i < population.size(); ++i) {
		p_sum[i] = p_sum[i - 1] + population[i].p;
		mask[i] = false;
	}
	//LOG("all %f\n", p_sum[p_sum.size() - 1]);

	if (population.size() > 0) {
		Group next_generation;
		Yuki::Random random;
		next_generation.push_back(population[0]);
		for (size_t i = 1; i < population.size(); ++i) {
			float rate = random.random<float>(p_sum[p_sum.size() - 1]);
			int l = 0, r = p_sum.size() - 1;
			int k = -1;
			while (l <= r) {
				int mid = (l + r) / 2;
				//if (l == r) k = mid;
				if (rate <= p_sum[mid]) {
					k = mid;
					r = mid - 1;
				}
				else {
					//k = mid + 1;
					l = mid + 1;
				}
			}
			//LOG("%d %f %f\n", k, p_sum[k], rate);
			next_generation.push_back(population[k]);
			auto &last = next_generation[next_generation.size() - 1];
			last.gene = new unsigned char[last.length];
			printf("%d\n", last.length);
			memcpy(last.gene, population[k].gene, last.length);
			mask[k] = true;
		}
		for (size_t i = 0; i < population.size(); ++i) {
			delete[] population[i].gene;
		}
		population = next_generation;
	}
}

void Chromosome::cross(Group &parents, float cross_rate) {
	Yuki::Random random;
	for (int i = 0; i < parents.size(); i += 2) {
		float rate = random.random<float>(1);
		if (rate <= cross_rate) {
			int mi = i, fi = i + 1;
			auto &male = parents[mi];
			auto &female = parents[fi];

			int cp = random.random<int>(male.length);
			for (int k = cp; k < male.length; ++k) {
				if (male.gene[k] != female.gene[k]) {
					male.gene[k] ^= female.gene[k] ^= male.gene[k] ^= female.gene[k];
				}
			}

		}
	}
}

void  Chromosome::mutate(Group &population, float mutate_rate) {
	Yuki::Random random;
	for (int i = 0; i < population.size(); ++i) {
		float rate = random.random<float>(1);
		if (rate <= mutate_rate) {
			auto &chrom = population[i];
			int cp = random.random<int>(chrom.length * 8);

			auto gene = chrom.gene[cp / 8];
			unsigned char bit = 1 << (7 - cp % 8);
			if ((gene >> (7 - cp % 8)) & 1) {
				gene &= ~bit;
			}
			else {
				gene |= bit;
			}

			chrom.gene[cp / 8] = gene;
		}
	}
}

Group Chromosome::first_generation(int n_triangle, int n_chrom) {
	Group population;
	for (int i = 0; i < n_chrom; ++i) {
		vector<Triangle> tr_list;
		for (int j = 0; j < n_triangle; ++j)
			tr_list.emplace_back(random_triangle(target.cols, target.rows));
		population.emplace_back(tr_list);
	}
	return population;
}