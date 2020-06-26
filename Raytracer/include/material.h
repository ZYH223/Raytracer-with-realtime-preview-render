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
  virtual Vec3f Shade (const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const = 0;
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
		float index):Material(d_color),specularColor(s_color),exponent(e),refractionColor(r_color),transparentColor(t_color),indexOfRefraction(index) {}
	~PhongMaterial() {}
	Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const;
	Vec3f getSpecularColor() const { return specularColor; }
	void glSetMaterial(void) const;
protected:
	Vec3f specularColor;
	float exponent;
	Vec3f refractionColor;
	Vec3f transparentColor;
	float indexOfRefraction;
};

// ====================================================================
// ====================================================================

#endif
