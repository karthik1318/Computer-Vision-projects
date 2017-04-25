#ifndef __STAFF_DETECTOR_H
#define __STAFF_DETECTOR_H

#include <vector>

class StaffDetector {
	int _staff_height;
	std::vector<std::vector<int> > _groups;
public:
	StaffDetector(const std::vector<int>&);

	int staff_height() const;

	const std::vector<std::vector<int> >& groups() const;
};

#endif
