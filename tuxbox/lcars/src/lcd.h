#ifndef __lcddisplay__
#define __lcddisplay__

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string>
#include <dbox/lcd-ks0713.h>

using namespace std;

class lcddisplay
{
	private:
		unsigned char raw[132][64];
		unsigned char lcd[LCD_ROWS][LCD_COLS];
		int fd;
		string	iconBasePath;
	public:
		
		enum
		{
			PIXEL_ON = LCD_PIXEL_ON,
			PIXEL_OFF = LCD_PIXEL_OFF,
			PIXEL_INV = LCD_PIXEL_INV
		};
		
		lcddisplay();
		~lcddisplay();

		int invalid_col(int x);
		int invalid_row(int y);
		void convert_data();
		int sgn(int arg);
		void setIconBasePath(string bp){iconBasePath=bp;};
		void update();
		int loadFont(std::string filename);
		void writeToFile(std::string filename);
		void loadFromFile(std::string filename);
		void setTextSize(int size);
		void putText(int x, int y, int font, std::string text);
		void writeTolcddisplay();
		void draw_point (int x,int y, int state);
		void draw_line (int x1, int y1, int x2, int y2, int state);
		void draw_fill_rect (int left,int top,int right,int bottom,int state);
		void draw_rectangle (int left,int top, int right, int bottom, int linestate,int fillstate);
		void draw_polygon(int num_vertices, int *vertices, int state);
		void draw_char(int x, int y, char c);
		void draw_string(int x, int y, char *string);

		void paintIcon(string filename, int x, int y, int col);
};

#endif
