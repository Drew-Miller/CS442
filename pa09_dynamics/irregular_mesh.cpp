#include <cassert>

#include "controller.h"
#include "check_gl.h"
#include "geometry.h"
#include "irregular_mesh.h"
#include "obj_io.h"
#include "render_stats.h"
#include "shader_programs.h"


// helper (could be static)
void fitInBbox(Point3 *p, const int nP,
                      const Point3 qMin, const Point3 qMax)
//
// scales (uniformly) and translates the points in `p` to fit in an
// axis-aligned bounding box defined by `qMin` and `qMax`.
//
{
    Point3 pMin, pMax;

    assert(nP > 0);
    pMin = pMax = p[0];
    for (int iP = 0; iP < nP; iP++) {
        for (int d = 0; d < 3; d++) {
            if (p[iP].u.a[d] < pMin.u.a[d])
                pMin.u.a[d] = p[iP].u.a[d];
            else if (p[iP].u.a[d] > pMax.u.a[d])
                pMax.u.a[d] = p[iP].u.a[d];
        }
    }
    // `pMin` and `pMax` now bound the points

    Point3 pCtr = (pMax + pMin) / 2.0;
    Point3 qCtr = (qMax + qMin) / 2.0;
    Vector3 dP = pMax - pCtr;
    Vector3 dQ = qMax - qCtr;

    // Find the smallest scale value.
    double scale = -1.0;
    for (int d = 0; d < 3; d++) {
        assert(dP.u.a[d] >= 0.0);
        if (dP.u.a[d] > 0.0) {
            double newScale = dQ.u.a[d] / dP.u.a[d];
            if (scale == -1.0 || newScale < scale)
                scale = newScale;
        }
    }
    if (scale == -1.0) // All points are identical, so don't scale.
        scale = 1.0;

    // Scale all coordinates in all dimensions by the smallest scale
    // value and add qCtr.
    for (int iP = 0; iP < nP; iP++)
        p[iP] = scale * (p[iP] - pCtr) + qCtr;
}


void IrregularMesh::allocateBuffers(void)
{
    //
    // Copy your previous (PA05) solution here.
    //
    CHECK_GL(glGenBuffers(1, &vertexPositionsBufferId));

    // Call glGenBuffers() for `vertexNormalBufferId` and `faceNormalBufferId`
    CHECK_GL(glGenBuffers(1, &vertexNormalBufferId));
  CHECK_GL(glGenBuffers(1, &faceNormalBufferId));
}


IrregularMesh::IrregularMesh(Point3 *vertexPositions_, Vector3 *vertexNormals_,
                             int nVertices_)
{
    nVertices = nVertices_;
    vertexPositions = vertexPositions_;
    vertexNormals = vertexNormals_;
    assert(nVertices % 3 == 0); // irregular mesh assumes 3 vertices/face
    nFaces = nVertices / 3;
    assert(nFaces * 3 == nVertices);

    createFaceNormalsAndCentroids();

    allocateBuffers();
    //
    // Since we're handling transforms in the vertex shader, we only
    // need to download the buffers once, here in the constructor,
    // rather than in IrregularMesh::render().
    //
    updateBuffers();
}


const void IrregularMesh::render(void)
{
    //
    // Copy your previous (PA05) solution here.
    //
    GLint vpai = ShaderProgram::getCurrentAttributeIndex("vertexPosition");

    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexPositionsBufferId));
    CHECK_GL(glEnableVertexAttribArray(vpai));
    CHECK_GL(glVertexAttribPointer(
                 vpai, // index of attribute
                 3, // # of elements per attribute
                 GL_DOUBLE, // type of each component
                 GL_FALSE,  // don't normalized fixed-point values
                 0, // offset between consecutive generic vertex attributes
                 BUFFER_OFFSET(0)));


      // face/vertex normals
     GLint vnai = ShaderProgram::getCurrentAttributeIndex("vertexNormal");

     if(vnai != NO_SUCH_ATTRIBUTE){
       if(controller.useVertexNormals){
         CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBufferId));
       }
       else{
         CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, faceNormalBufferId));
       }

       CHECK_GL(glEnableVertexAttribArray(vnai));
       CHECK_GL(glVertexAttribPointer(
                    vnai, // index of attribute
                    3, // # of elements per attribute
                    GL_DOUBLE, // type of each component
                    GL_FALSE,  // don't normalized fixed-point values
                    0, // offset between consecutive generic vertex attributes
                    BUFFER_OFFSET(0)));
    }

    renderTriangles();
}


const void IrregularMesh::renderTriangles(void) const
{
    //
    // Copy your previous (PA03) solution here.
    //
    CHECK_GL(glDrawArrays(GL_TRIANGLES, 0, 3 * nFaces));

    renderStats.ctTrianglesInIrregularMeshes += nFaces;
    renderStats.ctVertices += (3 * nFaces);
}



const void IrregularMesh::createFaceNormalsAndCentroids(void)
{
    int iVertex = 0;

    assert(nFaces * 3 == nVertices);
    faceNormals = new Vector3[nFaces];
    faceCentroids = new Point3[nFaces];
    for (int iFace = 0; iFace < nFaces; iFace++) {
        faceNormals[iFace] = faceNormal(
            vertexPositions[iVertex],
            vertexPositions[iVertex + 1],
            vertexPositions[iVertex + 2]);
        faceCentroids[iFace] = (
            vertexPositions[iVertex]
            + vertexPositions[iVertex + 1]
            + vertexPositions[iVertex + 2]) / 3.0;
        iVertex += 3;
    }
}


IrregularMesh *IrregularMesh::read(const string fname)
{
    vector<Point3> vertexPositionsVector;
    vector<Vector3> vertexNormalsVector;
    vector<Point2> textureCoordinatesVector;
    vector<Face> facesVector;

    if (!readObj(fname, vertexPositionsVector, vertexNormalsVector,
                 textureCoordinatesVector, facesVector)) {
        cerr << "Unable to read OBJ file \"" << fname << "\" -- exiting\n";
        exit(EXIT_FAILURE);
    }

    //
    // The vectors readObj() returns follow the OBJ format, but this
    // is not entirely compatible with what the OpenGL array-drawing
    // routines. In particular, OpenGL (indirect) indexing cannot
    // handle the (up to) 3 distinct indices (position, texture,
    // normal) in OBJ face specifications, so we must eliminate the
    // indirection of the OBJ indices.
    //

    int nFaces = facesVector.size();
    int nVertices = 3 * nFaces; // 3 vertices / (triangular) face
    Point3 *vertexPositions = new Point3[nVertices];
    Vector3 *vertexNormals = new Vector3[nVertices];

    int iVertex = 0;
    for (int iFace = 0; iFace < nFaces; iFace++) {
        Face face = facesVector[iFace];
        vertexPositions[iVertex] = vertexPositionsVector[
            face.faceVertex0.positionIndex];
        assert(face.faceVertex0.normalIndex != OBJ_INDEX_DEFAULTED);
        vertexNormals[iVertex] = vertexNormalsVector[
                face.faceVertex0.normalIndex];
        iVertex++;
        vertexPositions[iVertex] = vertexPositionsVector[
            face.faceVertex1.positionIndex];
        assert(face.faceVertex1.normalIndex != OBJ_INDEX_DEFAULTED);
        vertexNormals[iVertex] = vertexNormalsVector[
                face.faceVertex1.normalIndex];
        iVertex++;
        vertexPositions[iVertex] = vertexPositionsVector[
            face.faceVertex2.positionIndex];
        assert(face.faceVertex2.normalIndex != OBJ_INDEX_DEFAULTED);
        vertexNormals[iVertex] = vertexNormalsVector[
                face.faceVertex2.normalIndex];
        iVertex++;
    }
    assert(iVertex == nVertices);
    Face *faces = new Face[nFaces];
    copy(facesVector.begin(), facesVector.end(), faces);

    // make the mesh fit in a 1.5 x 1.5 x 1.5 bounding box
    fitInBbox(vertexPositions, nVertices,
              Point3(-0.75, -0.75, -0.75),
              Point3( 0.75,  0.75,  0.75)
        );

    IrregularMesh *irregularMesh = new IrregularMesh(
        vertexPositions, vertexNormals, nVertices);

    return irregularMesh;
}


void IrregularMesh::updateBuffers(void)
{
    //
    // Copy your previous (PA05) solution here.
    //
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexPositionsBufferId));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions[0]) * nVertices,
        vertexPositions, GL_STATIC_DRAW));

    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBufferId));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexNormals[0]) * nVertices,
        vertexNormals, GL_STATIC_DRAW));


    // BIND the face Normal vector to the vertices of the faces
    Vec3 *faceNormalOfVertex = new Vec3[nVertices];

    for (int iFace = 0; iFace < nFaces; iFace++) {
          faceNormalOfVertex[iFace * 3 + 0] = faceNormals[iFace];
          faceNormalOfVertex[iFace * 3 + 1] = faceNormals[iFace];
          faceNormalOfVertex[iFace * 3 + 2] = faceNormals[iFace];
    }

    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, faceNormalBufferId));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(faceNormalOfVertex[0]) * nVertices,
    faceNormalOfVertex, GL_STATIC_DRAW));

    delete faceNormalOfVertex;
}
