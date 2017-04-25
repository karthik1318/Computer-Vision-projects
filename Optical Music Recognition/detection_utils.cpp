#include <fstream>
#include <cmath>

#include "detection_utils.h"
#include "SImageIO.h"
#include "DrawText.h"

#define BETWEEN(X,Y,Z) (((X) <= (Y)) && ((Y) <= (Z)))

void overlay_rectangle(
		SDoublePlane &input,
		int _top, int _left, int _bottom, int _right,
		double graylevel, int width)
{
	using std::min;
	using std::max;
	for(int w=-width/2; w<=width/2; w++) {
		int top = _top+w, 
		left = _left+w,
		right=_right+w,
		bottom=_bottom+w;

		// if any of the coordinates are out-of-bounds, truncate them 
		top = min( max( top, 0 ), input.rows()-1);
		bottom = min( max( bottom, 0 ), input.rows()-1);
		left = min( max( left, 0 ), input.cols()-1);
		right = min( max( right, 0 ), input.cols()-1);

		// draw top and bottom lines
		for(int j=left; j<=right; j++)
			input[top][j] = input[bottom][j] = graylevel;
		// draw left and right lines
		for(int i=top; i<=bottom; i++)
			input[i][left] = input[i][right] = graylevel;
	}
}

void draw_line(SDoublePlane& R, SDoublePlane& G, SDoublePlane& B, 
			int x1, int y1, int x2, int y2,
			double r, double g, double b, int thickness) {
	//Source: http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B
	
	bool steep = std::abs(y2 - y1) > std::abs(x2 - x1);
	if(steep) {
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
 
	if(x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	float dx = x2 - x1;
	float dy = fabs(y2 - y1);
	float error = dx / 2.0f;
	int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;
	int maxX = (int)x2;

	for(int x=(int)x1; x<maxX; x++) {
		if(steep && BETWEEN(0, y, R.rows()) && BETWEEN(0, x, R.cols())) {
			for (int curY = y - thickness/2; curY <= y + thickness/2; curY++) {
				R[curY][x] = r;
				G[curY][x] = g;
				B[curY][x] = b;

			}
		} else if (!steep && BETWEEN(0, x, R.rows()) && BETWEEN(0, y, R.cols())){
			for (int curX = x - thickness/2; curX <= x + thickness/2; curX++) {
				R[curX][y] = r;
				G[curX][y] = g;
				B[curX][y] = b;
			}
		}
 
		error -= dy;
		if(error < 0) {
			y += ystep;
			error += dx;
		}
	}
}

void  write_detection_txt(const std::string &filename, const std::vector<struct DetectedSymbol> &symbols)
{
	std::ofstream ofs(filename.c_str());

	for(size_t i=0; i<symbols.size(); i++)
	{
		const DetectedSymbol &s = symbols[i];
		ofs << s.row << " " << s.col << " " << s.width << " " << s.height << " ";
		if(s.type == NOTEHEAD)
			ofs << "filled_note " << s.pitch;
		else if(s.type == EIGHTHREST)
			ofs << "eighth_rest _";
		else 
			ofs << "quarter_rest _";
		ofs << " " << s.confidence << std::endl;
	}
}

// Function that outputs a visualization of detected symbols
void  write_detection_image(const std::string &filename,
					const std::vector<DetectedSymbol> &symbols,
					const SDoublePlane &input)
{
	SDoublePlane output_planes[3];
	for(int i=0; i<3; i++)
		output_planes[i] = input;

	for(size_t i=0; i<symbols.size(); i++)
	{
		const DetectedSymbol &s = symbols[i];

		overlay_rectangle(output_planes[s.type], s.row, s.col, 
								s.row+s.height-1, s.col+s.width-1, 255, 2);
		overlay_rectangle(output_planes[(s.type+1) % 3], s.row,
								s.col, s.row+s.height-1, s.col+s.width-1, 0, 2);
		overlay_rectangle(output_planes[(s.type+2) % 3], s.row,
								s.col, s.row+s.height-1, s.col+s.width-1, 0, 2);

		if(s.type == NOTEHEAD)
		{
			char str[] = {s.pitch, 0};
			draw_text(output_planes[0], str, s.row, s.col+s.width+1, 0, 2);
			draw_text(output_planes[1], str, s.row, s.col+s.width+1, 0, 2);
			draw_text(output_planes[2], str, s.row, s.col+s.width+1, 0, 2);
		}
	}

	SImageIO::write_png_file(filename.c_str(),
					output_planes[0], output_planes[1], output_planes[2]);
}


