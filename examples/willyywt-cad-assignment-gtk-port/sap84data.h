#pragma once
#include "framework.h"
#include "hw_utils.h"

struct Joint {
	double x;
	double y;
};

struct Plane {
	int a;
	int b;
	int c;
	int d;
};

/* The homework says class CQuadElement. Make it happy. */
typedef Plane CQuadElement;

Joint Joint_add(Joint a, Joint b);
Joint Joint_minus(Joint a, Joint b);
Joint Joint_multiply(Joint a, double n);
Joint Joint_average(Joint a, Joint b);
Joint Joint_average4(Joint a, Joint b, Joint c, Joint d);
/* Calculate area of triangle OAB. The area is signed: Counter Clockwise returns positive while Clockwise returns negative. */
double Joint_area(Joint O, Joint A, Joint B);
/* Copy src to dst */
void Joint_cpy(Joint* dst, const Joint* src);
/* Swap the two points */
void Joint_swap(Joint* a, Joint* b);
/* Reorder joints to Counter Clockwise */
void Joint_CCL(Joint* a, Joint* b, Joint* c, Joint* d);
struct SAP84Data {
	/* The joints and points have consecutive index starting from 1, 
	i.e. joints[0] has index 1 of joints and planes[0] has index 1 of planes
	*/
	std::vector<Joint> joints;
	std::vector<Plane> planes;
};

struct SAP84Data_error {
	int line_count;
	int char_count;
	const char* message;
};

SAP84Data* SAP84_read(FILE* file, SAP84Data_error** error);
void SAP84_delete(SAP84Data* data);
