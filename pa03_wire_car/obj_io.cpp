// This package performs input (and, perhaps, eventually) output of
// the Wavefront OBJ file format. It actually supports only a small
// part of the OBJ format: irregular meshes.

#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <vector>

#include "geometry.h"
#include "obj_io.h"

using namespace std;

#define MAX_LINE 4096 // length of the longest possible line in the input file


static FaceVertex parseFaceVertexSpec(const char *faceVertexSpec,
                                 const int latestVertexIndex)
// parses an OBJ vertex specification as part of a face specification,
// which includes a vertex index and maybe vertex normal and texture
// coordinate indices: "vi", "vi//vn", or "vi/vt/vn", where
// "vi", "vt", and "vn" are all integers.
{
    int positionIndex, textureIndex, normalIndex;

    // Note that in what follows, our indices are 0-based, while OBJ's
    // are 1-based. We convert the latter to the former here.
    if (sscanf(faceVertexSpec,
               "%d/%d/%d", &positionIndex, &textureIndex, &normalIndex) == 3) {
        // positionIndex can be negative, so it's set below (et seq.)
        normalIndex--;
        textureIndex--;
    } else if (sscanf(faceVertexSpec,
                      "%d//%d", &positionIndex, &normalIndex) == 2) {
        textureIndex = OBJ_INDEX_DEFAULTED;
        normalIndex--; // our indices are 0-based, OBJ's are 1-based
    } else if (sscanf(faceVertexSpec, "%d", &positionIndex) == 1) {
        textureIndex = OBJ_INDEX_DEFAULTED;
        normalIndex = OBJ_INDEX_DEFAULTED;
    } else
        assert(0); // shouldn't happen

    // In OBJ, negative position indices to refer to the most
    // recently-defined vertex indices.
    if (positionIndex < 0)
        positionIndex = latestVertexIndex + 1 + positionIndex;
    else
        positionIndex--;

    return FaceVertex(positionIndex, textureIndex);
}


static int readContinuedLine(char *line, int maxLength, FILE *f)
// reads a line from `f`, skipping blank and comment lines, and
// allowing line continuations
{
    int len;
    char *token;

    for (;;) {
        if (fgets(line, maxLength, f) == NULL)
            return EOF;
        len = strlen(line);
        assert(line[len-1] == '\n');
        // truncate newline
        line[--len] = '\0';

        // assume blank line in either of these cases -- skip to next
        // line
        if (sscanf(line, "%ms", &token) == EOF)
            continue;
        if (sscanf(line, "%ms", &token) == 0)
            continue;

        // look for a comment line
        if (token[0] == '#') {
            free(token);
            continue; // found comment -- skip to next line
        }

        free(token); // no longer needed

        if (len == 0 || (len > 0 && line[len-1] != '\\'))
            return len; // line is not continued

        // prepare to append the next line to `line`
        line += len;
        maxLength -= len;
    }
}


bool readObj(const string fname,
             vector<Point3>& vertexPositions, vector<Vector3>& vertexNormals,
             vector<Point2>& textureCoordinates,
             vector<Face>& faces)
// reads a Wavefront OBJ file
{
    char line[MAX_LINE];
    char *token, *restOfLine;
    int ctToken;
    int ctFaceSpec;
    FILE *f;

    int nVertices = 0;
    int nLines = 0; // number of lines read so far

    f = fopen(fname.c_str(), "r");
    if (!f)
        return false;
    for (;;)
    {
        if (readContinuedLine(line, MAX_LINE, f) == EOF)
            break;
        nLines++;
        if (sscanf(line, "%ms%n", &token, &ctToken) == 0)
            continue; // assume blank line
        restOfLine = &line[ctToken]; // pointer to chars after the token
        assert(token[0] != '#'); // comments already taken care of
        if (strcmp(token, "v") == 0) { // vertex coordinates (3D)
            Point3 p;
            sscanf(restOfLine, "%lf%lf%lf", &p.u.g.x, &p.u.g.y, &p.u.g.z);
            vertexPositions.push_back(p);
            nVertices++;
        } else if (strcmp(token, "vn") == 0) { // vertex normals (3D)
            Vector3 n;
            sscanf(restOfLine, "%lf%lf%lf", &n.u.g.x, &n.u.g.y, &n.u.g.z);
            vertexNormals.push_back(n.normalized());
        } else if (strcmp(token, "vt") == 0) { // texture coordinates (2D)
            Point2 p;
            sscanf(restOfLine, "%lf%lf", &p.u.g.x, &p.u.g.y);
            textureCoordinates.push_back(p);
        } else if (strcmp(token, "f") == 0) { // face indices

            // OBJ's vertex specification string (malloc'd)
            char *faceVertexSpec;

            // extract all FaceVertex (specifications) on the line
            vector<FaceVertex> faceVertices;
            while (sscanf(restOfLine,
                          "%ms%n", &faceVertexSpec, &ctFaceSpec) == 1) {
                restOfLine[ctFaceSpec] = '\0';
                FaceVertex faceVertex
                    = parseFaceVertexSpec(faceVertexSpec, nVertices-1);
                faceVertices.push_back(faceVertex);
                free(faceVertexSpec);
                restOfLine = &restOfLine[ctFaceSpec+1];
            }

            // Apply a fan tessellation if there are more than 3
            // FaceVertices. (If there are only two FaceVertices, the
            // face is ignored.)
            for (unsigned int i = 1; i < faceVertices.size() - 1; i++) {
                Face face = Face(
                    faceVertices[0], faceVertices[i], faceVertices[i+1]);
                faces.push_back(face);
            }
        } else {
            fprintf(stderr,
                    "%s:%d:\n"
                    "     warning: OBJ '%s' data unknown or not (yet) supported"
                    " -- ignoring\n",
                    fname.c_str(), nLines, token);
        }
        free(token);
    }

    return true;
}


#ifdef TEST

int main(int argc, char *argv[])
{
    vector<Point3> vertexPositions;
    vector<Vector3> vertexNormals;
    vector<Point2> textureCoordinates;
    vector<Face> faces;

    assert(argc == 2);

    readObj(string(argv[1]), vertexPositions, vertexNormals, textureCoordinates,
            faces);
    vector< vector<int> > incidentFaceIndicesOfVertexIndex;
    int nVertices = vertexPositions.size();
    incidentFaceIndicesOfVertexIndex.resize(nVertices);

    getIncidentFaceIndices(faces, incidentFaceIndicesOfVertexIndex);
    printf("%d vertexPositions:\n", nVertices);
    for (int i = 0; i < nVertices; i++) {
        printf("  %2d:  %6.3f %6.3f %6.3f,", i,
               vertexPositions[i].u.g.x,
               vertexPositions[i].u.g.y,
               vertexPositions[i].u.g.z);
        int n = incidentFaceIndicesOfVertexIndex[i].size();
        printf("  incident on %d faces: ", n);
        for (int j = 0; j < n; j++) {
            printf(" %d", incidentFaceIndicesOfVertexIndex[i][j]);
        }
        printf("\n");
    }
    printf("%d vertex normals: (before replacing defaults)\n",
           (int) vertexNormals.size());
    for (unsigned int i = 0; i < vertexNormals.size(); i++) {
        printf("  %2d:   %6.3f %6.3f %6.3f\n", i,
               vertexNormals[i].u.g.x,
               vertexNormals[i].u.g.y,
               vertexNormals[i].u.g.z);
    }
    printf("%d texture vertices:\n", (int) textureCoordinates.size());
    for (unsigned int i = 0; i < textureCoordinates.size(); i++) {
        printf("  %2d:   %6.3f %6.3f\n", i,
               textureCoordinates[i].u.g.x,
               textureCoordinates[i].u.g.y);
    }
    printf("%d faces: (before replacing defaults)\n", (int) faces.size());
    for (unsigned int i = 0; i < faces.size(); i++) {
        printf("  face %d:\n", i);
        printf("  %2d:  %d %d %d\n", 0,
               faces[i].faceVertex0.positionIndex,
               faces[i].faceVertex0.textureIndex,
               faces[i].faceVertex0.normalIndex);
        printf("  %2d:  %d %d %d\n", 1,
               faces[i].faceVertex1.positionIndex,
               faces[i].faceVertex1.textureIndex,
               faces[i].faceVertex1.normalIndex);
        printf("  %2d:  %d %d %d\n", 2,
               faces[i].faceVertex2.positionIndex,
               faces[i].faceVertex2.textureIndex,
               faces[i].faceVertex2.normalIndex);
    }
    getUnspecifiedVertexNormals(faces, nVertices, vertexNormals);
    printf("%d faces: (after replacing defaults)\n", (int) faces.size());
    for (unsigned int i = 0; i < faces.size(); i++) {
        printf("  face %d:\n", i);
        printf("  %2d:  %d %d %d\n", 0,
               faces[i].faceVertex0.positionIndex,
               faces[i].faceVertex0.textureIndex,
               faces[i].faceVertex0.normalIndex);
        printf("  %2d:  %d %d %d\n", 1,
               faces[i].faceVertex1.positionIndex,
               faces[i].faceVertex1.textureIndex,
               faces[i].faceVertex1.normalIndex);
        printf("  %2d:  %d %d %d\n", 2,
               faces[i].faceVertex2.positionIndex,
               faces[i].faceVertex2.textureIndex,
               faces[i].faceVertex2.normalIndex);
    }
    printf("%d vertex normals: (after replacing defaults)\n",
           (int) vertexNormals.size());
    for (unsigned int i = 0; i < vertexNormals.size(); i++) {
        printf("  %2d:   %6.3f %6.3f %6.3f\n", i,
               vertexNormals[i].u.g.x,
               vertexNormals[i].u.g.y,
               vertexNormals[i].u.g.z);
    }
}

#endif // TEST
