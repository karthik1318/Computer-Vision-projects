#ifndef SYMBOLS_H
#define SYMBOLS_H

typedef enum {
	NOTEHEAD=0,
	QUARTERREST=1,
	EIGHTHREST=2
} Type;


class DetectedSymbol {
public:
	int row, col, width, height;
	int x, y;
	Type type;
	char pitch;
	double confidence;
};

#endif
