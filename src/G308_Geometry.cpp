//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee
//
// This software is provided 'as-is' for assignment of COMP308
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include "G308_Geometry.h"
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#include <iostream>

using namespace std;

int numVert, numNorm, numUV, numFace;

G308_Geometry::G308_Geometry(void) {
	m_pVertexArray = NULL;
	m_pNormalArray = NULL;
	m_pUVArray = NULL;
	m_pTriangles = NULL;

	mode = G308_SHADE_POLYGON;

	m_nNumPoint = m_nNumUV = m_nNumPolygon = 0;
	m_glGeomListPoly = m_glGeomListWire = 0;
}

G308_Geometry::~G308_Geometry(void) {
	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
}

//-------------------------------------------------------
// This function read obj file having
// triangle faces consist of vertex v, texture coordinate vt, and normal vn
// e.g. f v1/vt1/vn1 v2/vt1/vn2 v3/vt3/vn3
//
// [Assignment1]
// You can revise the following function for reading other variations of obj file
// 1) f v1//vn1 v2//vn2 v3//vn3 ; no texture coordinates such as bunny.obj
// 2) f v1 v2 v3 ; no normal and texture coordinates such as dragon.obj
// The case 2) needs additional functions to build vertex normals
//--------------------------------------------------------
void G308_Geometry::ReadOBJ(const char *filename) {
	FILE* fp;
	char mode, vmode;
	char str[200];
	//int v1, v2, v3, n1, n2, n3, t1, t2, t3;
	int v1=-1, v2=-1, v3=-1, n1=-1, n2=-1, n3=-1, t1=-1, t2=-1, t3=-1;

	float x, y, z;
	float u, v;

	numVert = numNorm = numUV = numFace = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
		printf("Error reading %s file\n", filename);
	else
		printf("Reading %s file\n", filename);

	// Check number of vertex, normal, uvCoord, and Face
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
				numUV++;
			else if (vmode == 'n') // normal
				numNorm++;
			else if (vmode == ' ') // vertex
				numVert++;
			break;
		case 'f': /* faces */
			numFace++;
			break;
		}
	}

	m_nNumPoint = numVert;
	m_nNumUV = numUV;
	m_nNumPolygon = numFace;
	m_nNumNormal = numNorm;

	printf("Number of Point %d, UV %d, Normal %d, Face %d\n", numVert, numUV,
			numNorm, numFace);
	//-------------------------------------------------------------
	//	Allocate memory for array
	//-------------------------------------------------------------

	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	m_pVertexArray = new G308_Point[m_nNumPoint];

	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	m_pNormalArray = new G308_Normal[m_nNumNormal];

	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	m_pUVArray = new G308_UVcoord[m_nNumUV];

	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
	m_pTriangles = new G308_Triangle[m_nNumPolygon];

	//-----------------------------------------------------------
	//	Read obj file
	//-----------------------------------------------------------
	numVert = numNorm = numUV = numFace = 0;

	fseek(fp, 0L, SEEK_SET);
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
			{
				sscanf(str, "vt %f %f", &u, &v);
				m_pUVArray[numUV].u = u;
				m_pUVArray[numUV].v = v;
				numUV++;
			} else if (vmode == 'n') // normal
			{
				sscanf(str, "vn %f %f %f", &x, &y, &z);
				m_pNormalArray[numNorm].x = x;
				m_pNormalArray[numNorm].y = y;
				m_pNormalArray[numNorm].z = z;
				numNorm++;
			} else if (vmode == ' ') // vertex
			{
				sscanf(str, "v %f %f %f", &x, &y, &z);
				m_pVertexArray[numVert].x = x;
				m_pVertexArray[numVert].y = y;
				m_pVertexArray[numVert].z = z;
				numVert++;
			}
			break;
		case 'f': /* faces : stored value is index - 1 since our index starts from 0, but obj starts from 1 */
			int scans;
			scans = sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			if (scans != 9){
				scans = sscanf(str, "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
				if (scans != 6){
					scans = sscanf(str, "f %d/%d/ %d/%d/ %d/%d/", &v1, &t1, &v2, &t2, &v3, &t3);
				} if (scans != 6){
					scans = sscanf(str, "f %d%*[ /]%d%*[ /]%d%*[/]", &v1, &v2, &v3);
					if (scans != 3){
						//scans = sscanf(str, "f %d %d %d". &v1, &v2,
						printf("\nsscanf() returned: %d\n", scans);
						printf("There is a problem with the line:\n%s", str);
						printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n", v1, t1, n1, v2, t2, n2, v3, t3, n3);
						// should possibly have the application exit here
					}
				}
			}


			// Vertex indicies for triangle:
			if (numVert != 0) {
				m_pTriangles[numFace].v1 = v1 - 1;
				m_pTriangles[numFace].v2 = v2 - 1;
				m_pTriangles[numFace].v3 = v3 - 1;
			}

			// Normal indicies for triangle
			if (numNorm != 0) {
				m_pTriangles[numFace].n1 = n1 - 1;
				m_pTriangles[numFace].n2 = n2 - 1;
				m_pTriangles[numFace].n3 = n3 - 1;
			}

			// UV indicies for triangle
			if (numUV != 0) {
				m_pTriangles[numFace].t1 = t1 - 1;
				m_pTriangles[numFace].t2 = t2 - 1;
				m_pTriangles[numFace].t3 = t3 - 1;
			}

			numFace++;
			break;
		default:
			break;
		}
	}

	// if the previous code had loaded no vertex normals, then calculate them
	if (numNorm == 0){
	printf("normal = 0\n");
		G308_Point* normAvg = new G308_Point[numVert];
		int* numAvg = new int[numVert];
		for (int i=0; i<numFace; ++i){
			G308_Triangle t = m_pTriangles[i];
			// first vert in the face
			G308_Point p = normAvg[t.v1];
			// this line actually needs to calculate the surface normal...
			G308_Point a = m_pVertexArray[t.v1]; // will reuse this to represent the normal
			G308_Point b = m_pVertexArray[t.v2]; // this will end up b-a
			G308_Point c = m_pVertexArray[t.v3]; // this will end up c-a
			b.x=b.x-a.x;	b.y=b.y-a.y;	b.z=b.z-a.z;
			c.x=c.x-a.x;	c.y=c.y-a.y;	c.z=c.z-a.z;
			a.x=(b.y*c.z) - (b.z*c.y);
			a.y=(b.z*c.x) - (b.x*c.z);
			a.z=(b.x*c.y) - (b.y*c.x);

			float normCoef = 1/sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
			//if (!(i%100)) printf("%f\t%f\t%f\t%f\n", a.x, a.y, a.z, normCoef);
			a.x*=normCoef;
			a.y*=normCoef;
			a.z*=normCoef;

			//printf("%f\t%f\t%f\n", a.x, a.y, a.z);

			numAvg[t.v1]++;
			/*if (p.x==0){
				p.x=t.t1.x;
				p.y=t.t1.y;
				p.z=t.t1.z;
			} else {*/
				normAvg[t.v1].x+=a.x;
				normAvg[t.v1].y+=a.y;
				normAvg[t.v1].z+=a.z;
			//}
			// second vert in the face
			p = normAvg[t.v2];
			//v = m_pVertexArray[t.v2];
			numAvg[t.v2]++;
			normAvg[t.v2].x+=a.x;
			normAvg[t.v2].y+=a.y;
			normAvg[t.v2].z+=a.z;
			// third vert in the face
			p = normAvg[t.v3];
			//v = m_pVertexArray[t.v3];
			numAvg[t.v3]++;
			normAvg[t.v3].x+=a.x;
			normAvg[t.v3].y+=a.y;
			normAvg[t.v3].z+=a.z;
			//printf("%f\t%f\t%f\n", normAvg[t.v3].x, p.y, p.z);

		}
		// average the normals
		for (int i=0; i<numVert; ++i){
			G308_Point * p = &normAvg[i];
			//printf("%d\t%f\t%f\t%f\n", numAvg[i], p.x, p.y, p.z);
			p->x/=(float)numAvg[i];
			p->y/=(float)numAvg[i];
			p->z/=(float)numAvg[i];
			float normCoef = 1/sqrt(p->x*p->x + p->y*p->y + p->z*p->z);
			if (!(i%100)) printf("%f\t%f\t%f\t%f\n", p->x, p->y, p->z, normCoef);
			p->x*=normCoef;
			p->y*=normCoef;
			p->z*=normCoef;
			//printf("%d\t%f\t%f\t%f\n", numAvg[i], p.x, p.y, p.z);
		}
		numNorm=numVert;
		m_pNormalArray = normAvg;
		// apply the average normals to the triangles
		for (int i=0; i<numFace; ++i){
			G308_Triangle * t = &m_pTriangles[i];
			t->n1=t->v1;
			t->n2=t->v2;
			t->n3=t->v3;
			//printf("%d\t%d\t%d\t%f\t%f\t%f\n", m_pTriangles[i].n1, m_pTriangles[i].n2, m_pTriangles[i].n3, m_pNormalArray[t->n1].x, m_pNormalArray[t->n2].x, m_pNormalArray[t->n3].x);
		}

		//delete[] normAvg;
		delete[] numAvg;
	}


	fclose(fp);
	printf("Reading OBJ file is DONE.\n");

}

//--------------------------------------------------------------
// [Assignment1]
// Fill the following function to create display list
// of the obj file to show it as polygon
//--------------------------------------------------------------
void G308_Geometry::CreateGLPolyGeometry() {
	cout << "\nCreating PolyGeometry!\n";
	if (m_glGeomListPoly != 0){
		glDeleteLists(m_glGeomListPoly, 1);
	}
	// Assign a display list; return 0 if err
	m_glGeomListPoly = glGenLists(1);
	glNewList(m_glGeomListPoly, GL_COMPILE);

	//YOUR CODE GOES HERE
	// .....
	int i=0;
	//numNorm=0;
	for (i=0; i< numFace; ++i){
		glBegin( GL_TRIANGLES );
			G308_Triangle tri = m_pTriangles[i];
			if (numUV) glTexCoord2fv((float*)&m_pUVArray[tri.t1]);
			if (numNorm) glNormal3fv((float*)&m_pNormalArray[tri.n1]);
			glVertex3fv((float*)&m_pVertexArray[tri.v1]);

			if (numUV) glTexCoord2fv((float*)&m_pUVArray[tri.t2]);
			if (numNorm) glNormal3fv((float*)&m_pNormalArray[tri.n2]);
			glVertex3fv((float*)&m_pVertexArray[tri.v2]);

			if (numUV) glTexCoord2fv((float*)&m_pUVArray[tri.t3]);
			if (numNorm) glNormal3fv((float*)&m_pNormalArray[tri.n3]);
			glVertex3fv((float*)&m_pVertexArray[tri.v3]);
		glEnd();
	}


//	glutSolidTeapot(5.0);


	glEndList();
}


//--------------------------------------------------------------
// [Assignment1]
// Fill the following function to create display list
// of the obj file to show it as wireframe
//--------------------------------------------------------------
void G308_Geometry::CreateGLWireGeometry() {
	cout << "\nCreating WireGeometry!\n";

	if (m_glGeomListWire != 0){
		glDeleteLists(m_glGeomListWire, 1);
	}
	// Assign a display list; return 0 if err
	m_glGeomListWire = glGenLists(1);
	glNewList(m_glGeomListWire, GL_COMPILE);

	//YOUR CODE GOES HERE
	// .....
	int i;
	for (i=0; i<numFace; ++i){
		G308_Triangle tri = m_pTriangles[i];
		glBegin( GL_LINE_LOOP );
			if (numUV) glTexCoord2fv((float*)&m_pUVArray[tri.t1]);
			glNormal3fv((float*)&m_pNormalArray[tri.n1]);
			glVertex3fv((float*)&m_pVertexArray[tri.v1]);

			if (numUV) glTexCoord2fv((float*)&m_pUVArray[tri.t2]);
			glNormal3fv((float*)&m_pNormalArray[tri.n2]);
			glVertex3fv((float*)&m_pVertexArray[tri.v2]);

			if (numUV) glTexCoord2fv((float*)&m_pUVArray[tri.t3]);
			glNormal3fv((float*)&m_pNormalArray[tri.n3]);
			glVertex3fv((float*)&m_pVertexArray[tri.v3]);
		glEnd();
	}
	//MY CODE ENDS HERE

	//glutWireTeapot(5.0);



	glEndList();
}


void G308_Geometry::toggleMode() {
	if (mode == G308_SHADE_POLYGON) {
		mode = G308_SHADE_WIREFRAME;
	} else {
		mode = G308_SHADE_POLYGON;
	}
}


void G308_Geometry::RenderGeometry() {

	if (mode == G308_SHADE_POLYGON) {
		//-------------------------------------------------------------
		// [Assignment1] :
		// Polygon mesh by glut. Comment this out to when moving onto displaying your .obj
		//glShadeModel(GL_FLAT);
		//glutSolidTeapot(5.0);

		// [Assignment1] :
		// Uncomment the next line to display the polygon of your own .obj file
		//glRotatef(xrot,1,0,0);
		glCallList(m_glGeomListPoly);


	} else if (mode == G308_SHADE_WIREFRAME) {
		//-------------------------------------------------------------
		// [Assignment1] :
		// Wireframe mesh by glut. Comment this out to when moving onto displaying your .obj
		//glShadeModel(GL_SMOOTH);
		//glutWireTeapot(5.0);

		// [Assignment1] :
		// Uncomment the next line to display the wireframe of your own .obj file
		//glRotatef(xrot,1,0,0);
		glCallList(m_glGeomListWire);

	} else {
		printf("Warning: Wrong Shading Mode. \n");
	}

}

















