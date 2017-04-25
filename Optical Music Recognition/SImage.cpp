#include "SImage.h"

SDoublePlane& SDoublePlane::operator*=(double val) {
	for (int i=0; i<_rows * _cols; i++) {
		data_area[i] *= val;
	}

	return *this;
}

SDoublePlane& SDoublePlane::operator*=(const SDoublePlane& other) {
	if (_rows != other.rows() || _cols != other.cols()) {
		throw "Can not Add another image of different dimension.";
	}
	for (int i=0; i<_rows * _cols; i++) {
		data_area[i] *= other.data_area[i];
	}

	return *this;
}

SDoublePlane& SDoublePlane::operator+=(double val) {
	for (int i=0; i<_rows * _cols; i++) {
		data_area[i] += val;
	}

	return *this;
}

SDoublePlane& SDoublePlane::operator+=(const SDoublePlane& other) {
	if (_rows != other.rows() || _cols != other.cols()) {
		throw "Can not Add another image of different dimension.";
	}
	for (int i=0; i<_rows * _cols; i++) {
		data_area[i] += other.data_area[i];
	}

	return *this;
}



SDoublePlane operator*(SDoublePlane one, const SDoublePlane& two) {
	if (one.rows() != two.rows() || one.cols() != two.cols()) {
		throw "Can not Add another image of different dimension.";
	}
	one *= two;
	return one;
}

SDoublePlane operator+(SDoublePlane one, const SDoublePlane& two) {
	if (one.rows() != two.rows() || one.cols() != two.cols()) {
		throw "Can not Add another image of different dimension.";
	}
	one += two;
	return one;
}

SDoublePlane operator*(SDoublePlane one, double two) {
	one *= two;
	return one;
}

SDoublePlane operator+(SDoublePlane one, double two) {
	one += two;
	return one;
}


