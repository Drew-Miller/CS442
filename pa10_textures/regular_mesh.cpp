#include <cassert>

#include "render_stats.h"
#include "regular_mesh.h"

void RegularMesh::allocateBuffers(void)
{
    //
    // ASSIGNMENT (PA10)
    //
    // In addition to your previous (PA06) solution, do the following:
    //
    // - Call glGenBuffers() to create `textureCoordinatesBufferId`.
    //
    CHECK_GL(glGenBuffers(1, &vertexPositionsBufferId));
    CHECK_GL(glGenBuffers(1, &indexBufferId));
    CHECK_GL(glGenBuffers(1, &vertexNormalBufferId));
    CHECK_GL(glGenBuffers(1, &textureCoordinatesBufferId));
}


void RegularMesh::updateBuffers(void)
{
    //
    // ASSIGNMENT (PA10)
    //
    // In addition to your previous (PA06) solution, do the following:
    //
    // - If `textureCoordinates` is not NULL:
    //
    //   * Call glBindBuffer() to bind `textureCoordinatesBufferId` to
    //     GL_ARRAY_BUFFER, treating `textureCoordinates` similarly
    //     to what you did with vertex positions and vertex normals.
    //
    //   * Call glBufferData() to associate it with
    //     `textureCoordinates`.
    //
    // As before, for maximum efficiency make it GL_STATIC_DRAW.
    // Remember that glBufferData() always expects buffer sizes in
    // bytes.
    //
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexPositionsBufferId));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions[0]) * nVertices,
        vertexPositions, GL_STATIC_DRAW));

    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBufferId));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexNormals[0]) * nVertices,
        vertexNormals, GL_STATIC_DRAW));

    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, indexBufferId));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexIndices[0]) * nVertexIndices,
        vertexIndices, GL_STATIC_DRAW));

    if(textureCoordinates){
      CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, textureCoordinatesBufferId));
      // there are nVertices of these
      CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates[0]) * nVertices,
          textureCoordinates, GL_STATIC_DRAW));
    }
}


bool RegularMesh::pointsAreDistinct(void)
//
// helper: Make sure all vertices in a regular mesh are distinct.
//
{
    for (int iVertex = 0; iVertex < nVertices; iVertex++) {
        for (int jVertex = 0; jVertex < iVertex; jVertex++) {
            //
            // Interesting programming note: The body of this loop
            // originally looked like this:
            //
            //   Vector3 diff = vertexPositions[iVertex]
            //            - vertexPositions[jVertex];
            //   if (       fabs(diff.u.g.x) < EPSILON
            //           && fabs(diff.u.g.y) < EPSILON
            //           && fabs(diff.u.g.z) < EPSILON)
            //       return false;
            //
            // But rewriting it as below cut the time by 60%:
            //
            double dx = vertexPositions[iVertex].u.g.x
                - vertexPositions[jVertex].u.g.x;
            double dy = vertexPositions[iVertex].u.g.y
                - vertexPositions[jVertex].u.g.y;
            double dz = vertexPositions[iVertex].u.g.z
                - vertexPositions[jVertex].u.g.z;
            if (     -EPSILON < dx && dx < EPSILON
                  && -EPSILON < dy && dy < EPSILON
                  && -EPSILON < dz && dz < EPSILON)
                return false;
        }
    }
    return true;
}


void RegularMesh::quadBoundary(int iLeft, int jLower, Point3 p[4])
//
// possible helper: returns (in p[]) the four points bounding the two
// faces of the quad in CCW order. It takes account of wrapping in
// both the i and j directions.
//
{
    int iRight, jUpper;

    //
    // It is a bug to request the boundary of the "rightmost
    // rectangles" (i.e. those whose lower left corner is
    // `iLower` = `nI`-1) *unless* we're wrapping in i (i.e.
    // `wrapI` is true).
    //
    assert(0 <= iLeft && iLeft < nI - 1 + wrapI);
    //
    // Likewise for the "topmost" rectangle, `jLower`, `nI`, and
    // `wrapJ`, respectively.
    //
    assert(0 <= jLower && jLower < nJ - 1 + wrapJ);

    //
    // When wrapI is true, the iLeft = 0 vertices form the boundary of
    // the rightmost rectangles.
    //
    if (wrapI && iLeft == nI-1)
        iRight = 0;
    else
        iRight = iLeft+1;

    //
    // Likewise when wrapJ is true for the topmost rectangles.
    //
    if (wrapJ && jLower == nJ-1)
        jUpper = 0;
    else
        jUpper = jLower+1;

    p[0] = vertexPositions[vertexIndex(iLeft,  jLower)];
    p[1] = vertexPositions[vertexIndex(iRight, jLower)];
    p[2] = vertexPositions[vertexIndex(iRight, jUpper)];
    p[3] = vertexPositions[vertexIndex(iLeft,  jUpper)];
}


const void RegularMesh::render(void)
{
    //
    // ASSIGNMENT (PA10)
    //
    // Modify your previous (PA06) solution here as follows:
    //
    // - If and only if `textureCoordinates` is not NULL, configure
    //   and enable texture coordinates. This will be similar to what
    //   you did to configure the vertex normal buffer (in PA06), with
    //   these exceptions:
    //
    //   * Call glBindBuffer() using `textureCoordinatesBufferId`
    //     instead of `vertexNormalBufferId`.
    //
    //   * Call ShaderProgram::getCurrentAttributeIndex() to get
    //     `textureCoordinatesAttributeIndex`, the attribute index (i.e.
    //     "handle") of the "textureCoordinates" attribute in the current
    //     shader program.
    //
    //   * Use `textureCoodinatesAttrbuteIndex` in the
    //     glEnableVertexAttribArray() and glVertexAttribPointer()
    //     calls instead of `vnai`. Remember
    //     that vertex coordinates consist of two double values per
    //     vertex. As before, be careful here.
    //
    //   The logical place to do this is right after you enable the
    //   vertex normal buffer.
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
       CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBufferId));
       CHECK_GL(glEnableVertexAttribArray(vnai));
       CHECK_GL(glVertexAttribPointer(
                    vnai, // index of attribute
                    3, // # of elements per attribute
                    GL_DOUBLE, // type of each component
                    GL_FALSE,  // don't normalized fixed-point values
                    0, // offset between consecutive generic vertex attributes
                    BUFFER_OFFSET(0)));
    }

    if(textureCoordinates){
      GLint tcai = ShaderProgram::getCurrentAttributeIndex("textureCoordinates");

      if(tcai != NO_SUCH_ATTRIBUTE){
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, textureCoordinatesBufferId));
        CHECK_GL(glEnableVertexAttribArray(tcai));
        CHECK_GL(glVertexAttribPointer(
                     tcai, // index of attribute
                     2, // # of elements per attribute
                     GL_DOUBLE, // type of each component
                     GL_FALSE,  // don't normalized fixed-point values
                     0, // offset between consecutive generic vertex attributes
                     BUFFER_OFFSET(0)));
      }
    }

    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId));

    for(int j = 0; j < nJ - 1 + wrapJ; j++){
      renderTriangleStrip(j);
    }
}


void RegularMesh::setTextureCoordinates(const double uScale,
                                        const double vScale)
//
// Uses the topological (`nI x nJ`) mesh to set the RegularMesh's
// `textureCoordinates`.
//
{
    int iTextureVertex = 0;
    textureCoordinates = new Point2[nVertices];
    double dU = uScale / (nI - 1);
    double dV = vScale / (nJ - 1);
    for (int j = 0; j < nJ; j++) {
        for (int i = 0; i < nI; i++) {
            textureCoordinates[iTextureVertex].u.g.x = i * dU;
            textureCoordinates[iTextureVertex].u.g.y = j * dV;
            iTextureVertex++;
        }
    }
    assert(iTextureVertex == nVertices);
}


const void RegularMesh::renderTriangleStrip(const int j) const
{
    //
    // Copy your previous (PA06) solution here.
    //
    int nIndicesInStrip = (nI + wrapI) * 2;
    // byte offset from the start of the glElementArray
    int byteOffset = j * (sizeof(vertexIndices[0]) * nIndicesInStrip);

    CHECK_GL(glDrawElements(GL_TRIANGLE_STRIP, nIndicesInStrip, GL_UNSIGNED_INT, BUFFER_OFFSET(byteOffset)));

    int nTrianglesInStrip = nIndicesInStrip - 2;
    renderStats.ctVertices += nTrianglesInStrip * 3;

    renderStats.ctTrianglesInRegularMeshes += nTrianglesInStrip;
    renderStats.ctTriangleStrips++;
}


void RegularMesh::createVertexIndices(void)
{
    int nIndicesPerTriangleStrip = 2 * (nI + wrapI);
    int nTriangleStrips = nJ - 1 + wrapJ;
    nVertexIndices = nIndicesPerTriangleStrip * nTriangleStrips;
    vertexIndices = new unsigned int [nVertexIndices];
    int iVertexIndices = 0;
    for (int j = 0; j < nJ - 1 + wrapJ; j++) {
        int jTop = j + 1;
        if (jTop >= nJ) {
            assert(jTop == nJ && wrapJ); // should be the only time this happens
            jTop = 0;
        }
        for (int i = 0; i < nI; i++) {
            vertexIndices[iVertexIndices++] = vertexIndex(i, jTop);
            vertexIndices[iVertexIndices++] = vertexIndex(i, j);
        }
        if (wrapI) {
            vertexIndices[iVertexIndices++] = vertexIndex(0, jTop);
            vertexIndices[iVertexIndices++] = vertexIndex(0, j);
        }
    }
    assert(iVertexIndices == nVertexIndices);
}


RegularMesh::RegularMesh(Point3 *vertexPositions_, Vector3 *vertexNormals_,
           int nI_, int nJ_, bool wrapI_, bool wrapJ_)
    : nI(nI_), nJ(nJ_), wrapI(wrapI_), wrapJ(wrapJ_)
{
    nVertices = nI * nJ;
    vertexPositions = new Point3[nVertices];
    vertexNormals = new Vector3[nVertices];
    for (int i = 0; i < nVertices; i++) {
        vertexPositions[i] = vertexPositions_[i];
        vertexNormals[i] = vertexNormals_[i];
    }
    textureCoordinates = NULL;

    // This enforces our requirement for distinct mesh points and thus
    // prevents later trouble.

    // For regular meshes, we create face normals for all the
    // vertices. If the vertices of a face aren't distinct, however,
    // the normals are undefined. Hence, we have a check here to make
    // sure they're distinct. (We don't do this for irregular meshes,
    // since it's possible for vertices to be repeated.)
    assert(pointsAreDistinct());

    createVertexIndices();
    createFaceNormalsAndCentroids();

    allocateBuffers();
    //
    // Since we're handling transforms in the vertex shader, we only
    // need to download the buffers once, here in the constructor,
    // rather than in RegularMesh::render().
    //
    updateBuffers();
}


const void RegularMesh::createFaceNormalsAndCentroids(void)
{
    //
    // Copy your previous (PA06) solution here.
    //
    int iFaces = nI + wrapI - 1;
    int jFaces = nJ + wrapJ - 1;

    nFaces = iFaces * jFaces * 2;

    // create the faceNormals and faceCentroids
    faceNormals = new Vector3[nFaces];
    faceCentroids = new Point3[nFaces];

    for(int i = 0; i < iFaces; i++){
      for (int j = 0; j < jFaces; j++) {
          // trianles within each face
          int ulIndex = faceIndex(i, j, true);
          int lrIndex = faceIndex(i, j, false);

          Point3 *p = new Point3[4];

          // get the quad boundary into the point p;
          quadBoundary(i, j, p);

          // create the centroids and the normals
          faceCentroids[ulIndex] = triangleCentroid(p[0], p[2], p[3]);
          faceCentroids[lrIndex] = triangleCentroid(p[0], p[1], p[2]);

          faceNormals[ulIndex] = faceNormal(p[0], p[2], p[3]);
          faceNormals[lrIndex] = faceNormal(p[0], p[1], p[2]);

          // delete the points we allocated for quadboundary
          delete[] p;
      }
    }
}
