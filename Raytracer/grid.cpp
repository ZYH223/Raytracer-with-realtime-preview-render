#include "grid.h"
#include "raytree.h"

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
	material = new PhongMaterial (
		Vec3f(1.0f, 1.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 0.0f),
		1,
		Vec3f(0.0f, 0.0f, 0.0f),
		Vec3f(0.0f, 0.0f, 0.0f),
		0
	);
	debugMaterials = new Material*[10];
	for (int i = 0; i < 10; i++) {
		debugMaterials[i] = new PhongMaterial(
			Vec3f(1.0f - i * 0.1f, 1.0f - i * 0.1f, 1.0f - i * 0.1f),
			Vec3f(0.0f, 0.0f, 0.0f),
			1,
			Vec3f(0.0f, 0.0f, 0.0f),
			Vec3f(0.0f, 0.0f, 0.0f),
			0);
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
	delete material;
	for (int i = 0; i < 10; i++) {
		delete debugMaterials[i];
	}
	delete[] debugMaterials;
}

Vec3f Grid::GetLength() const
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

void Grid::initializeRayMarch(MarchingInfo& mi, const Ray& r, float tmin) const
{
	mi.init(GetLength(), r.getDirection(), tmin);
	// x_near = min(x_min, x_max) x_far = max(x_min, x_max) 考虑到光线方向与轴向未必一致
	// near = max(x_near, y_near, z_near) 选择与三个近平面交点中t最大的，最接近包围盒
	// far = min(x_far, y_far, z_far) 选择与三个远平面交点中t最小的，最接近包围盒
	float t_near = -FLT_MAX, t_far = FLT_MAX;
	Vec3f normal(1.0f, 0.0f, 0.0f);
	if (fabs(r.getDirection().Dot3(normal)) > EPSILON) {// 如不相交则不做处理
		float t1 = (bb->getMin().x() - normal.Dot3(r.getOrigin())) / (normal.Dot3(r.getDirection()));
		float t2 = (bb->getMax().x() - normal.Dot3(r.getOrigin())) / (normal.Dot3(r.getDirection()));
		float t_xnear = t1 < t2 ? t1 : t2, t_xfar = t1 < t2 ? t2 : t1;
		t_near = t_near < t_xnear ? t_xnear : t_near;
		t_far = t_far > t_xfar ? t_xfar : t_far;
	}
	normal = Vec3f(0.0f, 1.0f, 0.0f);
	if (fabs(r.getDirection().Dot3(normal)) > EPSILON) {// 如不相交则不做处理
		float t1 = (bb->getMin().y() - normal.Dot3(r.getOrigin())) / (normal.Dot3(r.getDirection()));
		float t2 = (bb->getMax().y() - normal.Dot3(r.getOrigin())) / (normal.Dot3(r.getDirection()));
		float t_ynear = t1 < t2 ? t1 : t2, t_yfar = t1 < t2 ? t2 : t1;
		t_near = t_near < t_ynear ? t_ynear : t_near;
		t_far = t_far > t_yfar ? t_yfar : t_far;
	}
	normal = Vec3f(0.0f, 0.0f, 1.0f);
	if (fabs(r.getDirection().Dot3(normal)) > EPSILON) {// 如不相交则不做处理
		float t1 = (bb->getMin().z() - normal.Dot3(r.getOrigin())) / (normal.Dot3(r.getDirection()));
		float t2 = (bb->getMax().z() - normal.Dot3(r.getOrigin())) / (normal.Dot3(r.getDirection()));
		float t_znear = t1 < t2 ? t1 : t2, t_zfar = t1 < t2 ? t2 : t1;
		t_near = t_near < t_znear ? t_znear : t_near;
		t_far = t_far > t_zfar ? t_zfar : t_far;
	}
	if (t_near > tmin) {
		Vec3f intersect = r.pointAtParameter(t_near + EPSILON);// 找到交点
		// 算出交点所在的网格
		float grid_x = nx * ((intersect.x() - bb->getMin().x()) / (bb->getMax().x() - bb->getMin().x())),
			grid_y = ny * ((intersect.y() - bb->getMin().y()) / (bb->getMax().y() - bb->getMin().y())),
			grid_z = nz * ((intersect.z() - bb->getMin().z()) / (bb->getMax().z() - bb->getMin().z()));
		int i = (int)grid_x, j = (int)grid_y, k = (int)grid_z;
		// 计算从交点到三个维度上的下一格剩余的距离
		Vec3f remain(mi.getSign().x() > 0 ? 1-(grid_x - i) : grid_x - i,
			mi.getSign().y() > 0 ? 1-(grid_y - j) : grid_y - j,
			mi.getSign().z() > 0 ? 1-(grid_z - k) : grid_z - k);
		//float remain_x = grid_x - i, remain_y = grid_y - j, remain_z = grid_z - k;
		mi.setOrigin(t_near, t_far, i, j, k, mi.getIncrements() * remain);
	}
	else if (t_far > tmin && t_far != FLT_MAX) { // 源点在包围盒内，此时首个交点为源点
		Vec3f intersect = r.pointAtParameter(tmin + EPSILON);
		float grid_x = nx * ((intersect.x() - bb->getMin().x()) / (bb->getMax().x() - bb->getMin().x())),
			grid_y = ny * ((intersect.y() - bb->getMin().y()) / (bb->getMax().y() - bb->getMin().y())),
			grid_z = nz * ((intersect.z() - bb->getMin().z()) / (bb->getMax().z() - bb->getMin().z()));
		int i = (int)grid_x, j = (int)grid_y, k = (int)grid_z;
		Vec3f remain(1 - (grid_x - i), 1 - (grid_y - j), 1 - (grid_z - k));
		//float remain_x = grid_x - i, remain_y = grid_y - j, remain_z = grid_z - k;
		mi.setOrigin(tmin, t_far, i, j, k, mi.getIncrements() * remain);
	}
	else { //不相交或者在源点后方
		mi.setTMin(-FLT_MAX);
	}
}

bool Grid::intersect(const Ray& r, Hit& h, float tmin, float tmax)
{
	//RayTree::Activate();
	MarchingInfo mi;
	initializeRayMarch(mi, r, EPSILON);
	int materialIndex = 0;
	while (!mi.end() && 0 <= mi.getI() && mi.getI() < nx && 0 <= mi.getJ() && mi.getJ() < ny && 0 <= mi.getK() && mi.getK() < nz) {
		Material* m = debugMaterials[materialIndex];
		Vec3f center = GetCoordinate(mi.getI(), mi.getJ(), mi.getK());
		float x = center.x(), y = center.y(), z = center.z(), dx = 0.5f * GetLength().x(), dy = 0.5f * GetLength().y(), dz = 0.5f * GetLength().z();
		// 绘制每个途径Cell的六个面
		RayTree::AddHitCellFace(Vec3f(x + dx, y - dy, z - dz), Vec3f(x + dx, y - dy, z + dz), Vec3f(x + dx, y + dy, z + dz), Vec3f(x + dx, y + dy, z - dz), Vec3f(1.0f, 0.0f, 0.0f), m);
		RayTree::AddHitCellFace(Vec3f(x - dx, y - dy, z - dz), Vec3f(x - dx, y - dy, z + dz), Vec3f(x - dx, y + dy, z + dz), Vec3f(x - dx, y + dy, z - dz), Vec3f(-1.0f, 0.0f, 0.0f), m);
		RayTree::AddHitCellFace(Vec3f(x - dx, y + dy, z - dz), Vec3f(x - dx, y + dy, z + dz), Vec3f(x + dx, y + dy, z + dz), Vec3f(x + dx, y + dy, z - dz), Vec3f(0.0f, 1.0f, 0.0f), m);
		RayTree::AddHitCellFace(Vec3f(x - dx, y - dy, z - dz), Vec3f(x - dx, y - dy, z + dz), Vec3f(x + dx, y - dy, z + dz), Vec3f(x + dx, y - dy, z - dz), Vec3f(0.0f, -1.0f, 0.0f), m);
		RayTree::AddHitCellFace(Vec3f(x - dx, y - dy, z + dz), Vec3f(x - dx, y + dy, z + dz), Vec3f(x + dx, y + dy, z + dz), Vec3f(x + dx, y - dy, z + dz), Vec3f(0.0f, 0.0f, 1.0f), m);
		RayTree::AddHitCellFace(Vec3f(x - dx, y - dy, z - dz), Vec3f(x - dx, y + dy, z - dz), Vec3f(x + dx, y + dy, z - dz), Vec3f(x + dx, y - dy, z - dz), Vec3f(0.0f, 0.0f, -1.0f), m);

		// 绘制穿过的面
		if (mi.getNextThrough() == 1) RayTree::AddEnteredFace(Vec3f(x + dx, y - dy, z - dz), Vec3f(x + dx, y - dy, z + dz), Vec3f(x + dx, y + dy, z + dz), Vec3f(x + dx, y + dy, z - dz), Vec3f(1.0f, 0.0f, 0.0f), m);// right
		else if (mi.getNextThrough() == 2) RayTree::AddEnteredFace(Vec3f(x - dx, y - dy, z - dz), Vec3f(x - dx, y - dy, z + dz), Vec3f(x - dx, y + dy, z + dz), Vec3f(x - dx, y + dy, z - dz), Vec3f(-1.0f, 0.0f, 0.0f), m);// left
		else if (mi.getNextThrough() == 3) RayTree::AddEnteredFace(Vec3f(x - dx, y + dy, z - dz), Vec3f(x - dx, y + dy, z + dz), Vec3f(x + dx, y + dy, z + dz), Vec3f(x + dx, y + dy, z - dz), Vec3f(0.0f, 1.0f, 0.0f), m);// top
		else if (mi.getNextThrough() == 4) RayTree::AddEnteredFace(Vec3f(x - dx, y - dy, z - dz), Vec3f(x - dx, y - dy, z + dz), Vec3f(x + dx, y - dy, z + dz), Vec3f(x + dx, y - dy, z - dz), Vec3f(0.0f, -1.0f, 0.0f), m);// bottom
		else if (mi.getNextThrough() == 5) RayTree::AddEnteredFace(Vec3f(x - dx, y - dy, z + dz), Vec3f(x - dx, y + dy, z + dz), Vec3f(x + dx, y + dy, z + dz), Vec3f(x + dx, y - dy, z + dz), Vec3f(0.0f, 0.0f, 1.0f), m);// front
		else if (mi.getNextThrough() == 6) RayTree::AddEnteredFace(Vec3f(x - dx, y - dy, z - dz), Vec3f(x - dx, y + dy, z - dz), Vec3f(x + dx, y + dy, z - dz), Vec3f(x + dx, y - dy, z - dz), Vec3f(0.0f, 0.0f, -1.0f), m);// back
		else if (mi.getNextThrough() != 0) std::cout << "[WARNING]Grid::intersect Unknown lastThrough index " << mi.getNextThrough() << "of Cell(" << mi.getI() << ',' << mi.getJ() << ',' << mi.getK() << ")" << std::endl;// 0表示不绘制，除此以外为异常值
		mi.nextCell();
		materialIndex = (materialIndex+1) % 10;
	}
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
	/*GLfloat specular[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };    
	float glexponent = 1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);*/
	material->glSetMaterial();
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