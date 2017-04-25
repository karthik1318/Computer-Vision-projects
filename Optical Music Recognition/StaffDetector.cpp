#include <algorithm>
#include <numeric>

#include "StaffDetector.h"

StaffDetector::StaffDetector(const std::vector<int>& offsets) {
	std::vector<int> lines(offsets);
	std::sort(lines.begin(), lines.end());

	std::vector<int> diff_offsets;
    for (std::vector<int>::iterator it = lines.begin() + 1; it != lines.end(); it++) {
        diff_offsets.push_back(std::abs(*it - *(it - 1)));
    }

	//Average the smallest 5 diff_offsets to get staff height;
	std::sort(diff_offsets.begin(), diff_offsets.end());
    _staff_height = std::accumulate(diff_offsets.begin(), diff_offsets.begin() + 5, 0) / 5.0;

	for (int i=0; i < lines.size();) {
		std::vector<int> curGroup;
		curGroup.push_back(lines[i]);
		while (++i < lines.size()) {
			if (lines[i] - lines[i-1] > 3 * _staff_height) {
				break;
			} else {
				curGroup.push_back(lines[i]);
			}
		}
		_groups.push_back(curGroup);
	}
}

int StaffDetector::staff_height() const {
	return _staff_height;
}

const std::vector<std::vector<int> >& StaffDetector::groups() const {
	return _groups;
}
