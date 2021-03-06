// include glCanvas.h to access the preprocessor variable SPECULAR_FIX
#include "glCanvas.h"  

#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication 
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

#include "material.h"
// ====================================================================
// Set the OpenGL parameters to render with the given material
// attributes.
// ====================================================================

void PhongMaterial::glSetMaterial(void) const {

	GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat specular[4] = {
	  getSpecularColor().r(),
	  getSpecularColor().g(),
	  getSpecularColor().b(),
	  1.0 };
	GLfloat diffuse[4] = {
	  getDiffuseColor().r(),
	  getDiffuseColor().g(),
	  getDiffuseColor().b(),
	  1.0 };

	// NOTE: GL uses the Blinn Torrance version of Phong...      
	float glexponent = exponent;
	if (glexponent < 0) glexponent = 0;
	if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX 

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

	// OPTIONAL: 3 pass rendering to fix the specular highlight 
	// artifact for small specular exponents (wide specular lobe)

	if (SPECULAR_FIX_WHICH_PASS == 0) {
		// First pass, draw only the specular highlights
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

	}
	else if (SPECULAR_FIX_WHICH_PASS == 1) {
		// Second pass, compute normal dot light 
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
	}
	else {
		// Third pass, add ambient & diffuse terms
		assert(SPECULAR_FIX_WHICH_PASS == 2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
	}

#endif
}

Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor, const float &disToLight) const
{
	// 衰减在光源处做了处理，这里不再处理
	float attenuation = 1.0f;// 常数衰减（无衰减）
	//float attenuation = isfinite(disToLight) ? 1.0f / sqrt(disToLight) : 1.0f;// 开
	//float attenuation = isfinite(disToLight) ? 1.0f/(disToLight) : 1.0f;// 线性反比的衰减系数
	//float attenuation = isfinite(disToLight) ? 1.0f/(disToLight*disToLight) : 1.0f;// 平方反比的衰减系数
	// Blinn-Torrance方法，借助半程向量计算镜面反射项
	Vec3f h = -1.0f*ray.getDirection() + dirToLight;
	h.Normalize();
	float cosalpha = hit.getNormal().Dot3(h);
	float costheta = hit.getNormal().Dot3(dirToLight);
	Vec3f Ls(0.0f, 0.0f, 0.0f), Ld(0.0f, 0.0f, 0.0f);
	if (cosalpha > EPSILON) Ls = specularColor * pow(cosalpha, exponent) * lightColor * attenuation;
	if (costheta > EPSILON) Ld = diffuseColor * costheta * lightColor * attenuation;
	return Ls + Ld;
}

// ====================================================================
// ====================================================================