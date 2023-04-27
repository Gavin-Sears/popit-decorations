//--------------------------------------------------
// Author: Gavin Sears
// Date: Thursday, March 2
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include "plymesh.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace glm;

namespace agl {

   PLYMesh::PLYMesh(const std::string& filename) {
      load(filename);
   }

   PLYMesh::PLYMesh() {
      _minBounds = vec3(NULL, NULL, NULL);
      _maxBounds = vec3(NULL, NULL, NULL);
      _positions.clear();
      _normals.clear();
      _faces.clear();
      _texCoords.clear();
   }

   void PLYMesh::init() {
      assert(_positions.size() != 0);
      initBuffers(&_faces, &_positions, &_normals, &_texCoords);
   }

   PLYMesh::~PLYMesh() {
      _positions.clear();
      _normals.clear();
      _faces.clear();
      _texCoords.clear();
   }

   bool PLYMesh::load(const std::string& filename) {
      if (_positions.size() != 0) {
         std::cout << "WARNING: Cannot load different files with the same PLY mesh\n";
         return false;
      }
      _positions.clear();
      _normals.clear();
      _faces.clear();
      _texCoords.clear();
      _minBounds = vec3(NULL, NULL, NULL);
      _maxBounds = vec3(NULL, NULL, NULL);

      // Placeholders for reader
      int numPos;
      int numFaces;
      string myText;
      int lineNum = 0;
      bool startRead = false;
      int temp;
      float tempf;
      float tempf2;
      vec3 normFind1;
      vec3 normFind2;
      vec3 normFind3;
      vec3 oneTwo;
      vec3 oneThree;
      vec3 norm;
      
      // Set to NULL because that's how they should begin
      _minBounds = vec3(NULL, NULL, NULL);
      _maxBounds = vec3(NULL, NULL, NULL);

      ifstream PLYFile(filename);
      while (getline (PLYFile, myText))
      {
         if (!startRead)
         {
            // When element vertex count is found
            if (myText.find("element vertex") != -1)
            {
               numPos = stoi(myText.substr(15, (myText.length() - 1)));
            }
            // When element face count is found
            if (myText.find("element face") != -1)
            {
               numFaces = stoi(myText.substr(13, (myText.length() - 1)));
            }

         }
         else
         {
            // Only if vertices have not been iterated through
            if (numPos > 0)
            {
               // Point 1 and bounds updates
               tempf = stof(myText.substr(0, myText.find(" ") + 1));
               _positions.push_back(tempf);

               if (!_minBounds.x || tempf < _minBounds.x)
               {
                  _minBounds = vec3(tempf, _minBounds.y, _minBounds.z);
               }
               if (!_maxBounds.x || tempf > _maxBounds.x)
               {
                  _maxBounds = vec3(tempf, _maxBounds.y, _maxBounds.z);
               }
               myText.erase(0, myText.find(" ") + 1);

               // Point 2 and bounds updates
               tempf = stof(myText.substr(0, myText.find(" ") + 1));
               _positions.push_back(tempf);

               if (!_minBounds.y || tempf < _minBounds.y)
               {
                  _minBounds = vec3(_minBounds.x, tempf, _minBounds.z);
               }
               if (!_maxBounds.y || tempf > _maxBounds.y)
               {
                  _maxBounds = vec3(_maxBounds.x, tempf, _maxBounds.z);
               }
               myText.erase(0, myText.find(" ") + 1);

               // Point 3 and bounds updates
               tempf = stof(myText.substr(0, myText.find(" ") + 1));
               _positions.push_back(tempf);

               if (!_minBounds.z || tempf < _minBounds.z)
               {
                  _minBounds = vec3(_minBounds.x, _minBounds.y, tempf);
               }
               if (!_maxBounds.z || tempf > _maxBounds.z)
               {
                  _maxBounds = vec3(_maxBounds.x, _maxBounds.y, tempf);
               }
               myText.erase(0, myText.find(" ") + 1);

               // Normal 1
               tempf = stof(myText.substr(0, myText.find(" ") + 1));
               _normals.push_back(tempf);
               myText.erase(0, myText.find(" ") + 1);

               // Normal 2
               tempf = stof(myText.substr(0, myText.find(" ") + 1));
               _normals.push_back(tempf);
               myText.erase(0, myText.find(" ") + 1);

               // Normal 3
               tempf = stof(myText.substr(0, myText.find(" ")));
               _normals.push_back(tempf);
               myText.erase(0, myText.find(" ") + 1);

               // texCoords 1
               tempf = stof(myText.substr(0, myText.find(" ")));
               _texCoords.push_back(tempf);
               myText.erase(0, myText.find(" ") + 1);

               // texCoords 2
               tempf = stof(myText.substr(0, myText.find(" ")));
               _texCoords.push_back(-tempf);
               myText.erase(0, myText.find(" ") + 1);

               std::cout << tempf << std::endl;

               numPos--;
            }
            // Only if faces have not been iterated through
            else if (numFaces > 0)
            {
               myText.erase(0, myText.find(" ") + 1);
               
               // Finding first vector index
               temp = stoi(myText.substr(0, myText.find(" ") + 1));
               _faces.push_back(temp);
               normFind1 = vec3(_positions[temp * 3], _positions[temp * 3 + 1], _positions[temp * 3 + 2]);
               myText.erase(0, myText.find(" ") + 1);

               // Finding second vector index
               temp = stoi(myText.substr(0, myText.find(" ") + 1));
               _faces.push_back(temp);
               normFind2 = vec3(_positions[temp * 3], _positions[temp * 3 + 1], _positions[temp * 3 + 2]);
               myText.erase(0, myText.find(" ") + 1);

               // Finding third vector index
               temp = stoi(myText.substr(0, myText.find(" ")));
               _faces.push_back(temp);
               normFind3 = vec3(_positions[temp * 3], _positions[temp * 3 + 1], _positions[temp * 3 + 2]);
               myText.erase(0, myText.find(" "));

               numFaces--;
            }
         }

         // Checker for when the header is over
         if (myText == "end_header")
         {
            startRead = true;
         }
         lineNum++;
      }

      PLYFile.close();
      return false;
   }

   glm::vec3 PLYMesh::minBounds() const {

      return _minBounds;
   }

   glm::vec3 PLYMesh::maxBounds() const {
      return _maxBounds;
   }

   int PLYMesh::numVertices() const {
      return _positions.size();
   }

   int PLYMesh::numTriangles() const {
      return _faces.size();
   }

   const std::vector<GLfloat>& PLYMesh::positions() const {
      return _positions;
   }

   const std::vector<GLfloat>& PLYMesh::normals() const {
      return _normals;
   }

   const std::vector<GLuint>& PLYMesh::indices() const {
      return _faces;
   }

   const std::vector<GLfloat>& PLYMesh::texCoords() const {
      return _texCoords;
   }
}
