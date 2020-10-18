#include "grid.h"

Grid::Grid(BoundingBox* bb, int nx, int ny, int nz)
{
	this->bb = bb;
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;;
	cells = new bool**[nx];
	for (int i = 0; i < nx; i++)
	{
		cells[i] = new bool* [ny];
		for (int j = 0; j < ny; j++)
		{
			cells[i][j] = new bool[nz];
			memset(cells[i][j], 0, sizeof(bool) * nz);
		}
	}
}

Grid::~Grid()
{
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			delete[] cells[i][j];
		}
		delete[] cells[i];
	}
	delete[] cells;
}

Vec3f Grid::GetLength()
{
	return Vec3f((bb->getMax().x()-bb->getMin().x())/nx, (bb->getMax().y() - bb->getMin().y())/ny, (bb->getMax().z() - bb->getMin().z())/nz);
}

bool Grid::GetCell(int x, int y, int z)
{
	if (x < 0 || x >= nx || y < 0 || y >= ny || z < 0 || z >= nz)
	{
		std::cout << "[WARNING](Grid::GetCell)Grid Coordinate (" << x << "," << y << "," << z << ") out of range" << std::endl;
		return false;
	}
	return cells[x][y][z];
}

void Grid::SetCell(int x, int y, int z, bool opaque)
{
	if (x < 0 || x >= nx || y < 0 || y >= ny || z < 0 || z >= nz)
	{
		std::cout << "[WARNING](Grid::SetCell)Grid Coordinate (" << x << "," << y << "," << z << ") out of range" << std::endl;
		return ;
	}
	cells[x][y][z] = opaque;
}

Vec3f Grid::GetCoordinate(int x, int y, int z)
{
	if (x < 0 || x >= nx || y < 0 || y >= ny || z < 0 || z >= nz) {
		std::cout << "[WARNING]Grid::GetCoordinate Unlawful index (" << x << "," << y << "," << z << ")" << std::endl;
		return Vec3f();
	}
	Vec3f length = GetLength();
	return Vec3f(x - nx / 2.0f + 0.5f, y - ny / 2.0f + 0.5f, z - nz / 2.0f + 0.5f) * length;
}

bool Grid::intersect(const Ray& r, Hit& h, float tmin, float tmax)
{
	// Unrealized
	paint();
	return false;
}

bool Grid::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)
{
	// Unrealized
	return false;
}

void Grid::print() {
	std::cout << "[DEBUG](Grid::paint)Opaqueness Array:" << std::endl;
	for (int i = 0; i < nx; i++)
	{
		std::cout << "Layer" << i << std::endl;
		for (int j = 0; j < ny; j++)
		{
			for (int k = 0; k < nz; k++)
			{
				std::cout << (cells[i][j][k] ? 1 : 0) << ' ';
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

void Grid::paint(void)
{
	// 初始化网格渲染的材质
	GLfloat specular[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };    
	float glexponent = 1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);
	// 绘制网格的几何
	int dir[][3] = {
		{-1, 0, 0},
		{1, 0, 0},
		{0, -1, 0},
		{0, 1, 0},
		{0, 0, -1},
		{0, 0, 1}
	};
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			for (int k = 0; k < nz; k++)
			{
				if (cells[i][j][k]) {
					for (int diri = 0; diri < 6; diri++) {
						int ni = i + dir[diri][0], nj = j + dir[diri][1], nk = k + dir[diri][2];
						if ( ni < 0 || ni >= nx || nj < 0 || nj >= ny || nk < 0 || nk >= nz || !cells[ni][nj][nk] ) {// 仅当处于整个网格的边界，或者occupaied&unoccupied交界处才绘制网格
							Vec3f length = GetLength(), center = GetCoordinate(i, j, k) + 0.5f * length * Vec3f(dir[diri][0], dir[diri][1], dir[diri][2]);
							glPushMatrix();
							glTranslatef(center.x(), center.y(), center.z());
							glScalef(length.x(), length.y(), length.z());
							//glScalef(0.1f, 0.1f, 0.5f);
							if (dir[diri][0] < 0) glRotatef(180.0f, 0, 1.0f, 0);
							else if (dir[diri][1] < 0) glRotatef(-90.0f, 0, 0, 1.0f);
							else if (dir[diri][1] > 0) glRotatef(90.0f, 0, 0, 1.0f);
							else if (dir[diri][2] < 0) glRotatef(90.0f, 0, 1.0f, 0);
							else if (dir[diri][2] > 0) glRotatef(-90.0f, 0, 1.0f, 0);
							glBegin(GL_QUADS);
							glNormal3f(1.0f, 0.0f, 0.0f);
							glVertex3f(0.0f, -0.5f, -0.5f);
							glVertex3f(0.0f, 0.5f, -0.5f);
							glVertex3f(0.0f, 0.5f, 0.5f);
							glVertex3f(0.0f, -0.5f, 0.5f);
							glEnd();
							glPopMatrix();
						}
					}
				}
				
			}
		}
	}

}