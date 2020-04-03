#include <iostream>
#include <vector>
#include <windows.h>
#include <glut.h>
#define PI 3.142
using namespace std;
int winSizeX = 1200, winSizeY = 600;

int strokeSize = 15;
int MaxStrokeSize = 100, MinStrokeSize = 10;


float branchAngle = 65.0;
float MaxBranchAngle = 73.0, MinBranchAngle = 50.0;

int rootLength = 50.0;
int MaxRootLength = 200.0, MinRootLength = 10.0;

bool toggle_branch_angle_anim = false;
int toggle_branch_angle_anim_state = 0;
float branchAngle_anim = 0;


bool toggle_root_length_anim = false;
int toggle_root_length_anim_state = 0;
int rootLength_anim = 0;




//color of border
GLfloat r = 0, g = 0, b = 0;
GLfloat Leaf_r = 0, Leaf_g = 0.7, Leaf_b = 0.3;
bool hasStart = false;

class Point {
public:
	int x, y;
	Point(int _x=0, int _y=0):x(_x), y(_y){}
};



int selectRotateScale = 0;
int selectTranslateScale = 0;

//class color
class Color {
protected:
	float red, green, blue;
public:
	Color() :red(0), green(0), blue(0) { glColor3f(red, green, blue); }
	Color(GLfloat r, GLfloat g, GLfloat b) {
		red = r;
		green = g;
		blue = b;
		glColor3f(red, green, blue);
	}
	void setRed(GLfloat r) { red = r; }
	void setGreen(GLfloat g) { green = g; }
	void setBlue(GLfloat b) { blue = b; }
	GLfloat getRed() { return red; }
	GLfloat getGreen() { return green; }
	GLfloat getBlue() { return blue; }
};

class Fractal {
public:
	Point pos;
	int strokeWidth;
	Color col;
	Color col_Leaf;
	float tree_angle;
	float branch_angle;
	int root_length;
	
	Fractal()
	{
		strokeWidth = 0;
		root_length = 50;
		tree_angle = 3.14159 / 2.0;
		branch_angle = 65.0 / 100.0;
	}
	void drawPythagorasTree()
	{
		if (strokeWidth == 0) return;
		branch(pos.x, pos.y, root_length+rootLength_anim, tree_angle, branch_angle+branchAngle_anim/100, strokeWidth);
	}

	void branch(int x, int y, float len, float tree_angle, float branch_angle, int strokeSz)
	{
		int x2, y2;
		float childlen;
		float childangle;

		x2 = x - len * cos(tree_angle);
		y2 = y + len * sin(tree_angle);

		glLineWidth(strokeSz);
		glColor3f(col.getRed(), col.getGreen(), col.getBlue());
		//draw branches
		glBegin(GL_LINES);
		glVertex2f(x, y);
		glVertex2f(x2, y2);
		glEnd();
		
		if (col_Leaf.getRed() != 1 || col_Leaf.getGreen() != 1 || col_Leaf.getBlue() != 1)
		{//draw leaves
			glColor3f(col_Leaf.getRed(), col_Leaf.getGreen(), col_Leaf.getBlue());
			glBegin(GL_POLYGON);
			glVertex2f(x2, y2);
			glVertex2f(x2 - 2, y2 + 2);
			glVertex2f(x2 - 2, y2 + 4);
			glColor3f(0.5, 0.5, 0.5);
			glVertex2f(x2, y2 + 6);
			glVertex2f(x2 + 2, y2 + 4);
			glColor3f(col_Leaf.getRed(), col_Leaf.getGreen(), col_Leaf.getBlue());

			glVertex2f(x2 + 2, y2 + 2);
			glEnd();
		}
		
		//reset back color, dont want to affect branches' color
		glColor3f(col.getRed(), col.getGreen(), col.getBlue());

		if (len > 3)
		{
			strokeSz *= 0.7;
			childlen = len * branch_angle;
			childangle = 3.14159 / 2.0 * branch_angle;

			branch(x2, y2, childlen, tree_angle - childangle, branch_angle, strokeSz);
			branch(x2, y2, childlen, tree_angle + childangle, branch_angle, strokeSz);
		}

	}

	void drawSelectedSymbol()
	{
		float amplitude = 5.0f;
		float freq = 0.02f;


		
		glPushMatrix();
		
		glTranslatef(pos.x, pos.y, 0);
		glRotatef(selectRotateScale, 0, 1, 0);
		glTranslatef(-pos.x, -pos.y , 0);
		
		glTranslatef(0, sin(selectTranslateScale * PI * freq) * amplitude, 0);
		
		glBegin(GL_POLYGON);
		glColor3f(1, 0, 0);
		glVertex2f(pos.x, pos.y - 10);
		glColor3f(0.5, 0.5, 0.5);
		glVertex2f(pos.x-5, pos.y - 30);
		glColor3f(1, 0, 0);
		glVertex2f(pos.x+5, pos.y - 30);
		glEnd();
		glPopMatrix();
	}
};


Fractal currentFractal;
vector<Fractal> fractalList;
vector<Fractal>::iterator fractalList_iterator;

Fractal *selectedFractal;
int currentSelectedFractal_Index;

bool hasLeftButtonClicked = false;
bool hasMiddleButtonClicked = false;

int drawBranches = 0;

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		hasStart = true;
		hasLeftButtonClicked = true;
		currentFractal.pos.x = x;
		currentFractal.pos.y = winSizeY - y;
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
		hasMiddleButtonClicked = true;
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
		hasMiddleButtonClicked = false;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		hasLeftButtonClicked = false;
		//add the curve
		fractalList.push_back(currentFractal);
		currentSelectedFractal_Index=fractalList.size() - 1;
		//allow for the next input
		//reset current fractal
		//just make it out of the scene
		currentFractal.pos = Point(2000, 2000);
	}
	glutPostRedisplay();
}


void motion(int x, int y)
{
	if (hasLeftButtonClicked)
	{
		currentFractal.pos.x = x;
		currentFractal.pos.y = winSizeY-y;
	}

	if (hasMiddleButtonClicked)
	{
		selectedFractal->pos.x =x;
		selectedFractal->pos.y = winSizeY - y;
	}

	glutPostRedisplay();
}


void display_string(int x, int y, char* string, int font)
{
	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		if (font == 1)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
		if (font == 2)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
		if (font == 3)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
		if (font == 4)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, string[i]);
	}
}


void displayKeyInfo()
{
	glColor3f(0, 0, 0);

	char colInfo1[] = "Color Adjust:";
	char colInfo2[] = "r/R: Increase/Decrease red";
	char colInfo3[] = "g/G: Increase/Decrease green";
	char colInfo4[] = "b/B: Increase/Decrease blue";

	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 3.2 / 10, colInfo1, 2);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 3.6 / 10, colInfo2, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 4.0 / 10, colInfo3, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 4.4 / 10, colInfo4, 3);

	char info1[] = "Key:";
	char info2[] = "Left Button: Spawn Tree";
	char info3[] = "Middle Button: Move Selected Tree";
	char info4[] = "Right Button: Menu List";
	char info5[] = "SPACE: Select Tree";
	char info6[] = "+/=: Increase Root Width";
	char info7[] = "-/_: Decrease Root Width";
	char info8[] = "w/s: Increase/Decrease Root Length";
	char info9[] = "a/d: Increase/Decrease Branch Angle";
	char info10[] = "Del: Delete Previous";
	char info11[] = "R/r: Erase All";
	char info12[] = "ESC: Set Window To Original Position";

	char title[] = "Simple Fractal Tree";
	display_string(winSizeX * 4 / 10, winSizeY - winSizeY * 1.0 / 10, title, 1);

	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 5.2 / 10, info1, 2);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 5.6 / 10, info2, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 6.0 / 10, info3, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 6.4 / 10, info4, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 6.8 / 10, info5, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 7.2 / 10, info6, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 7.6 / 10, info7, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 8.0 / 10, info8, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 8.4 / 10, info9, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 8.8 / 10, info10, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 9.2 / 10, info11, 3);
	display_string(winSizeX * 8 / 10, winSizeY - winSizeY * 9.6 / 10, info12, 3);

}

// Routine to convert floating point to char string.
void floatToString(char* destStr, int precision, float val)
{
	sprintf(destStr, "%f", val);
	destStr[precision] = '\0';
}

void indicator()
{
	char R_Text[10], G_Text[10], B_Text[10];
	char Col_R_Text[] = "R: ", Col_G_Text[] = "G: ", Col_B_Text[] = "B: ";
	floatToString(R_Text, 3, r);
	floatToString(G_Text, 3, g);
	floatToString(B_Text, 3, b);

	char CurrentColor_Text[] = "Stem Color";
	char SizeIndicator_Text[] = "Root Width";
	char noLeaves_Text[] = "Leaves: No";
	char GreenLeaves_Text[] = "Leaves: Green";
	char RedLeaves_Text[] = "Leaves: Red";
	char SakuraLeaves_Text[] = "Leaves: Sakura";
	char ColdLeaves_Text[] = "Leaves: Cold";

	display_string(winSizeX * 0.5 / 10, winSizeY * 2.2 / 10, Col_R_Text, 2);
	display_string(winSizeX * 0.5 / 10, winSizeY * 1.8 / 10, Col_G_Text, 2);
	display_string(winSizeX * 0.5 / 10, winSizeY * 1.4 / 10, Col_B_Text, 2);

	display_string(winSizeX * 0.8 / 10, winSizeY * 2.2 / 10, R_Text, 2);
	display_string(winSizeX * 0.8 / 10, winSizeY * 1.8 / 10, G_Text, 2);
	display_string(winSizeX * 0.8 / 10, winSizeY * 1.4 / 10, B_Text, 2);

	display_string(winSizeX * 0.5 / 10, winSizeY * 0.7 / 10, CurrentColor_Text, 2);
	display_string(winSizeX * 1.8 / 10, winSizeY * 0.7 / 10, SizeIndicator_Text, 2);

	if (Leaf_r == 1.0f && Leaf_g == 1.0f && Leaf_b == 1.0f)
		display_string(winSizeX * 4.0 / 10, winSizeY * 0.7 / 10, noLeaves_Text, 2);
	else if (Leaf_r == 0 && Leaf_g == 0.7f && Leaf_b == 0.3f)
		display_string(winSizeX * 4.0 / 10, winSizeY * 0.7 / 10, GreenLeaves_Text, 2);
	else if (Leaf_r == 1.0f && Leaf_g == 0.0f && Leaf_b == 0.0f)
		display_string(winSizeX * 4.0 / 10, winSizeY * 0.7 / 10, RedLeaves_Text, 2);
	else if (Leaf_r == 1.0f && Leaf_g == 0.72f && Leaf_b == 0.77f)
		display_string(winSizeX * 4.0 / 10, winSizeY * 0.7 / 10, SakuraLeaves_Text, 2);
	else if (Leaf_r == 0.5f && Leaf_g == 0.6f && Leaf_b == 1.0f)
		display_string(winSizeX * 4.0 / 10, winSizeY * 0.7 / 10, ColdLeaves_Text, 2);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	glVertex2f(winSizeX * 1.3 / 10, winSizeY * 0.5 / 10);
	glVertex2f(winSizeX * 1.55 / 10, winSizeY * 0.5 / 10);
	glVertex2f(winSizeX * 1.55 / 10, winSizeY * 1.0 / 10);
	glVertex2f(winSizeX * 1.3 / 10, winSizeY * 1.0 / 10);
	glEnd();


	
	glLineWidth(strokeSize);
	glBegin(GL_LINES);
	glVertex2f(winSizeX * 2.8 / 10, winSizeY * 0.75 / 10);
	glVertex2f(winSizeX * 3.8 / 10, winSizeY * 0.75 / 10);
	glEnd();


}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	if (!fractalList.empty()) {
		fractalList_iterator = fractalList.begin();

		while (fractalList_iterator != fractalList.end())
		{
			fractalList_iterator->drawPythagorasTree();
			fractalList_iterator++;
		}
		
	}

	if (hasStart) {
		currentFractal.strokeWidth = strokeSize;
		currentFractal.root_length = rootLength;
		currentFractal.branch_angle = branchAngle/100.0;
		currentFractal.col = Color(r, g, b);
		currentFractal.col_Leaf = Color(Leaf_r, Leaf_g, Leaf_b);
		currentFractal.drawPythagorasTree();
	}

	displayKeyInfo();
	indicator();

	if (!fractalList.empty())
	{
		selectedFractal = &fractalList.at(currentSelectedFractal_Index);
		selectedFractal->drawSelectedSymbol();
	}

	glutPostRedisplay();
	glFlush();
}





void reshape(int w, int h)
{
	winSizeX = w;       //make sure to reset the window size as the mouse motion to the points will be affected
	winSizeY = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (float)w, 0.0, (float)h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//keyboardfunction
void NormalKey(GLubyte key, GLint x, GLint y)
{
	switch (key)
	{
	case 27:
		glutPositionWindow(10, 30);
		glutReshapeWindow(1200, 600);
		glutRemoveMenuItem(5);
		glutAddMenuEntry("Full Screen", 29);
		break;
	case 127:

		if (!fractalList.empty())
		{
			fractalList.erase(fractalList.begin()+currentSelectedFractal_Index);
			if (!fractalList.empty())
				currentSelectedFractal_Index = (currentSelectedFractal_Index + 1) % fractalList.size();
		}
		break;

	case ' ':
		currentSelectedFractal_Index = (currentSelectedFractal_Index + 1) % fractalList.size();
		break;

	case 8:
		while (!fractalList.empty()) {
			fractalList.pop_back();
		}

		break;
	case '+':
	case '=':

		if (strokeSize < MaxStrokeSize)
		{
			strokeSize++;
			if (selectedFractal != NULL)
				if (selectedFractal->strokeWidth < MaxStrokeSize)
					selectedFractal->strokeWidth++;
		}
		break;
	case '-':
	case '_':
		if (strokeSize > MinStrokeSize)
		{
			strokeSize--;
		}
		if (selectedFractal != NULL)
			if (selectedFractal->strokeWidth > MinStrokeSize)
				selectedFractal->strokeWidth--;
		break;

	case 'a':
	case 'A':

		if (!toggle_branch_angle_anim)
		{
			if (branchAngle < MaxBranchAngle)
				branchAngle++;

			if (selectedFractal != NULL)
				if (selectedFractal->branch_angle*100 < MaxBranchAngle)
				{
					selectedFractal->branch_angle += 0.01;
				}
		}

		break;

	case 'd':
	case 'D':
		if (!toggle_branch_angle_anim)
		{
			if (branchAngle > MinBranchAngle)
				branchAngle--;
		
			if (selectedFractal != NULL)
				if (selectedFractal->branch_angle * 100 > MinBranchAngle)
				{
					selectedFractal->branch_angle -= 0.01;
				}
		}
		break;

	case 'w':
	case 'W':
		if (!toggle_root_length_anim)
		{
			if (rootLength < MaxRootLength)
				rootLength++;
		
			if (selectedFractal != NULL)
				if (selectedFractal->root_length < MaxRootLength)
				{
					selectedFractal->root_length ++;
				}

		}
		break;

	case 's':
	case 'S':
		if (!toggle_root_length_anim)
		{
			if (rootLength > MinRootLength)
				rootLength--;
		
			if (selectedFractal != NULL)
				if (selectedFractal->root_length > MinRootLength)
				{
					selectedFractal->root_length --;
				}
		}
		break;

	case 'R':
		if (r > 0.1)
			r -= 0.1;
		break;
	case 'r':
		if (r < 1)
			r += 0.1;
		break;

	case 'G':
		if (g > 0.1)
			g -= 0.1;
		break;
	case 'g':
		if (g < 1)
			g += 0.1;
		break;

	case 'B':
		if (b > 0.1)
			b -= 0.1;
		break;
	case 'b':
		if (b < 1)
			b += 0.1;
		break;
	


	default: break;
	}

	if (selectedFractal != NULL)
		selectedFractal->col = Color(r, g, b);

	glutPostRedisplay();
}

void SpecialInput(int key, int x, int y)
{

	switch (key)
	{
	case GLUT_KEY_UP:
		selectedFractal->pos.y+=5;


		break;
	case GLUT_KEY_DOWN:
		selectedFractal->pos.y-=5;

		break;

	case GLUT_KEY_LEFT:
		selectedFractal->pos.x-=5;

		break;

	case GLUT_KEY_RIGHT:
		selectedFractal->pos.x+=5;

		break;
	}

	glutPostRedisplay();
}


void Timer(int)
{
	if (toggle_branch_angle_anim)
	{
		branchAngle = 65.0;

		if (toggle_branch_angle_anim_state==0)

			if (branchAngle_anim < MaxBranchAngle - branchAngle)
			{
				branchAngle_anim++;
			}
			else
				toggle_branch_angle_anim_state = 1;

		if (toggle_branch_angle_anim_state == 1)
			if (branchAngle_anim > MinBranchAngle - branchAngle)
			{
				branchAngle_anim--;
			}
			else
				toggle_branch_angle_anim_state = 0;
	}
	else
	{
		branchAngle_anim = 0;
	}
	if (toggle_root_length_anim)
	{
		rootLength = 50.0;
		if (toggle_root_length_anim_state == 0)
			if (rootLength_anim < MaxRootLength - rootLength)
			{
				rootLength_anim++;
			}
			else
				toggle_root_length_anim_state = 1;

		if (toggle_root_length_anim_state == 1)
			if (rootLength_anim > MinRootLength - rootLength)
			{
				rootLength_anim--;
			}
			else
				toggle_root_length_anim_state = 0;
	}
	else
	{
		rootLength_anim = 0;
	}

	selectRotateScale++;
	if (selectRotateScale > 45)
		selectRotateScale = 0;
	selectTranslateScale++;
	glutTimerFunc(1000 / 60, Timer, 0);
}

//Menu List
void MainMenu(int id) {

	if (id == 28)
		exit(0);

	if (id == 29) {
		glutRemoveMenuItem(5);
		glutFullScreen();
		glutAddMenuEntry("Exit Full Screen", 30);
	}
	if (id == 30) {
		glutRemoveMenuItem(5);
		glutPositionWindow(10, 30);
		glutReshapeWindow(1200, 600);
		glutAddMenuEntry("Full Screen", 29);
	}

}

void leavesmenu(int id)
{
	if (id == 1)
	{
		Leaf_r = 1, Leaf_g = 1, Leaf_b = 1;
	}
	if (id == 2)
	{
		Leaf_r = 0, Leaf_g = 0.7, Leaf_b = 0.3;
	}
	if (id == 3)
	{
		Leaf_r = 1, Leaf_g = 0, Leaf_b = 0;
	}
	if (id == 4)
	{
		Leaf_r = 1, Leaf_g = 0.72, Leaf_b = 0.77;
	}
	if (id == 5)
	{
		Leaf_r = 0.5, Leaf_g = 0.6, Leaf_b = 1.0;
	}

	if (selectedFractal != NULL)
		selectedFractal->col_Leaf = Color(Leaf_r, Leaf_g, Leaf_b);
		
}

void animationmenu(int id)
{
	if (id == 2)
		toggle_branch_angle_anim = !toggle_branch_angle_anim;

	if (id == 3)
		toggle_root_length_anim = !toggle_root_length_anim;
}

void colormenu(int id)
{
	//white
	if (id == 18) {
		r = g = b = 1;
	}
	//black
	if (id == 19) {
		r = g = b = 0;
	}
	//red
	if (id == 20) {
		r = 1;
		g = b = 0;
	}
	//orange
	if (id == 21) {
		r = 1;
		g = 0.5;
		b = 0;

	}
	//yellow
	if (id == 22) {
		r = g = 1;
		b = 0;
	}
	//green
	if (id == 23) {
		g = 1;
		b = r = 0;
	}
	//blue
	if (id == 24) {
		b = 1;
		r = g = 0;
	}
	//purple
	if (id == 25) {
		r = b = 1;
		g = 0;
	}
	//brown
	if (id == 26) {
		r = 0.5;
		g = b = 0;
	}
	//pink
	if (id == 27) {
		r = b = 1;
		g = 0.7;
	}


	if (selectedFractal != NULL)
		selectedFractal->col = Color(r, g, b);
}


//Menu Func
void MenuList()
{
	int Leaf_Color_submenu= glutCreateMenu(leavesmenu);
	glutAddMenuEntry("No Leaves", 1);
	glutAddMenuEntry("Green Leaves", 2);
	glutAddMenuEntry("Red Leaves", 3);
	glutAddMenuEntry("Sakura Leaves", 4);
	glutAddMenuEntry("Cold Leaves", 5);

	int Color_submenu = glutCreateMenu(colormenu);
	glutAddMenuEntry("White", 18);
	glutAddMenuEntry("Black", 19);
	glutAddMenuEntry("Red", 20);
	glutAddMenuEntry("Orange", 21);
	glutAddMenuEntry("Yellow", 22);
	glutAddMenuEntry("Green", 23);
	glutAddMenuEntry("Blue", 24);
	glutAddMenuEntry("Purple", 25);
	glutAddMenuEntry("Brown", 26);
	glutAddMenuEntry("Pink", 27);


	int Animation_submenu = glutCreateMenu(animationmenu);
	glutAddMenuEntry("Toggle Leaves Animate", 2);
	glutAddMenuEntry("Toggle Grow Animate", 3);

	glutCreateMenu(MainMenu);
	glutAddSubMenu("Leaves Color", Leaf_Color_submenu);
	glutAddSubMenu("Animation", Animation_submenu);
	glutAddSubMenu("Stem Color", Color_submenu);
	glutAddMenuEntry("Exit", 28);
	glutAddMenuEntry("Full Screen", 29);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(winSizeX, winSizeY);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Fractal Tree");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(NormalKey);
	glutSpecialFunc(SpecialInput);
	glutTimerFunc(1000 / 60, Timer, 0);
	MenuList();

	glutMainLoop();
	return 0;
}