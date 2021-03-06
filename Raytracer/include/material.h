#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
#include "constant.h"
#include "ray.h"
#include "hit.h"

// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Material {

public:

  // CONSTRUCTORS & DESTRUCTOR
  Material(const Vec3f &d_color) { diffuseColor = d_color; }
  Material(Material& m) { diffuseColor = m.diffuseColor; }
  virtual ~Material() {}

  // ACCESSORS
  virtual Vec3f getDiffuseColor() const { return diffuseColor; }
  virtual Vec3f getSpecularColor() const = 0;
  virtual Vec3f getReflectColor() const = 0;
  virtual Vec3f getTransparentColor() const = 0;
  virtual float getIndexOfRefraction() const = 0;
  virtual Vec3f Shade (const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor, const float& disToLight) const = 0;
  virtual void glSetMaterial(void) const = 0;

protected:

  // REPRESENTATION
  Vec3f diffuseColor;
  
};

class PhongMaterial:public Material {
public:
	PhongMaterial(
		const Vec3f &d_color,
		const Vec3f &s_color,
		float e,
		const Vec3f& r_color,
		const Vec3f& t_color,
		float index):Material(d_color),specularColor(s_color),exponent(e), reflectiveColor(r_color),transparentColor(t_color),indexOfRefraction(index) {}
	~PhongMaterial() {}
	Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor, const float& disToLight) const;
	Vec3f getSpecularColor() const { return specularColor; }
	Vec3f getReflectColor() const { return reflectiveColor; }
	Vec3f getTransparentColor() const { return transparentColor; }
	float getIndexOfRefraction() const { return indexOfRefraction; }
	void glSetMaterial(void) const;
protected:
	Vec3f specularColor;
	float exponent;
	Vec3f reflectiveColor;
	Vec3f transparentColor;
	float indexOfRefraction;
};

class ColorMaterial :public Material {
public:
	ColorMaterial(const Vec3f& color) :Material(color) {}
	~ColorMaterial() {}
	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor, const float& disToLight) const { return diffuseColor; }
	Vec3f getSpecularColor() const { return Vec3f(0.0f, 0.0f, 0.0f); }
	Vec3f getReflectColor() const { return Vec3f(0.0f, 0.0f, 0.0f); }
	Vec3f getTransparentColor() const { return Vec3f(0.0f, 0.0f, 0.0f); }
	float getIndexOfRefraction() const { return 0; }
	void glSetMaterial(void) const {
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(diffuseColor.r(), diffuseColor.g(), diffuseColor.b());
		//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
		glDisable(GL_COLOR_MATERIAL);
	}
};

// ====================================================================
// ====================================================================

#endif
