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
		std::cout << "[WARNING]Grid::Grid Coordinate (" << x << "," << y << "," << z << ") out of range" << std::endl;
		return false;
	}
	return cells[x][y][z];
}

void Grid::SetCell(int x, int y, int z, bool opaque)
{
	if (x < 0 || x >= nx || y < 0 || y >= ny || z < 0 || z >= nz)
	{
		std::cout << "[WARNING]Grid::Grid Coordinate (" << x << "," << y << "," << z << ") out of range" << std::endl;
		return ;
	}
	cells[x][y][z] = opaque;
}

bool Grid::intersect(const Ray& r, Hit& h, float tmin, float tmax)
{
	// Unrealized
	paint();
	return false;
}

bool  Grid::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)
{
	// Unrealized
	return false;
}

void  Grid::paint(void)
{
	std::cout << "[DEBUG]Grid::Opaqueness Array:" << std::endl;
	for (int i = 0; i < nx; i++)
	{
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