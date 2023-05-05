// Bryn Mawr College, alinen, 2020
//

#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include <glm/glm.hpp>
#include "plymesh.h"

using namespace std;
using namespace glm;
using namespace agl;

/*
* Decorators store information for a placed decoration. 
* Not all info is needed depending on where it is used:
*
* isModel - whether the decoration is a custom mesh, or a built in function
* ply - the name of the mesh or shape of the decoration
* pos - position of decoration
* rot - x, y, and z euler angles respectively
* scale - uniform scale
* min/max - used for boxes, provides intersection info by using minimum and maximum corners
* color - decoration color
*/
struct decorator
{
  // Needed for all decorations
  bool isModel;
  string ply;

  vec3 pos;
  float rotx;
  float roty;
  float rotz;
  vec3 scale;
  vec3 color;

  // Only needed for box intersection
  vec3 min;
  vec3 max;
};

/* 
* Info for a given box intersection. Could be given greater efficiency 
* by calculating position after we find the correct intersection, 
* but no performance issues have occured, and this feature could be used for other things.
*/
struct sect
{
  vec3 pos;
  bool hit;
  float t;
  vec3 norm;
};

class Viewer : public Window {
public:

  Viewer() : Window() 
  {
  }

  void setup() 
  {
    setWindowSize(_width, _height);

    // Load all meshes to respective variables
    _eyeMesh.load("../models/eye.ply");
    _hornMesh.load("../models/horn.ply");
    _duckMesh.load("../models/rubberDucky.ply");
    _noseMesh.load("../models/nose.ply");
    _mouthMesh.load("../models/mouth.ply");

    // Put strings in vector corresponding to meshes (DETERMINES ORDER)
    _meshes.push_back("eye");
    _meshes.push_back("horn");
    _meshes.push_back("nose");
    _meshes.push_back("duck");
    _meshes.push_back("mouth");
    _meshes.push_back("cube");

    // Loading textures for respective meshes
    renderer.loadTexture("eye", "../textures/eye.png", 0);
    renderer.loadTexture("horn", "../textures/horn.png", 0);
    renderer.loadTexture("mouth", "../textures/mouth.png", 0);
    renderer.loadTexture("duck", "../textures/duck_texture.png", 0);

    // Loading the shader
    renderer.loadShader("phong-pixel", "../shaders/phong-pixel.vs", "../shaders/phong-pixel.fs");
  }

  // Takes screen position, returns world position
  vec3 screenToWorld(const vec2& screen)
  {
    vec4 screenpos = vec4(screen, 1, 1);

    // flip y coordinate because positive Y goes down instead of up
    screenpos.y = _height - screenpos.y;

    // convert to canonical view coordinates
    screenpos.x = 2.0f*((screenpos.x / _width) - 0.5);
    screenpos.y = 2.0f*((screenpos.y / _height) - 0.5);

    // Convert the particle position to world coords
    mat4 projection = renderer.projectionMatrix();
    mat4 view = renderer.viewMatrix();
    vec4 worldpos = inverse(projection * view) * screenpos;

    // convert from homogenous to ordinary coordinates
    worldpos.x /= worldpos.w;
    worldpos.y /= worldpos.w;
    worldpos.z /= worldpos.w;

    return vec3(worldpos);
  }

  // Raycasts from mouse pos, returns true if given sphere intersects
  bool sphereIntersection(const vec3& ro, const vec3& rd, const vec3& center, float radius)
  {
    vec3 el = center - ro;
    float len = glm::length(rd);
    vec3 d = rd / len;
    float s = dot(el, d);
    float elSqr = dot(el, el);
    float rSqr = radius * radius;
    if (s < 0 && elSqr > rSqr) return false;

    float mSqr = elSqr - s * s;
    if (mSqr > rSqr) return false;

    return true;
  }

  // Raycasts from mouse pos, returns intersection info if plane intersects
  sect planeIntersection(const vec3& ro, const vec3& rd, const vec3& point, const vec3& nd)
  {
    float t;
    sect p;

    if (dot(rd, nd) == 0)
    {
      // return value for no intersection
      p.hit = false;
      p.pos = vec3(5.0, 5.0, 5.0);
      return p;
    }
    else
    {
      t = (dot(point, nd) - dot(ro, nd)) / dot(rd, nd);
      if (dot(((ro + t * rd) - point), nd) <= 0.00001)
      {
        p.pos = vec3(ro + t * rd);
        p.t = t;
        p.hit = true;
        p.norm = nd;
        return p;
      }
      else
        // return value for no intersection
        p.hit = false;
      return p;

    }
  }

  // Uses plane intersection to calculate info about a potential intersectionn with a cube
  sect cubeIntersection(const vec3& ro, const vec3& rd, const vec3& min, const vec3& max)
  {
    // Here are our intersection points
    sect tup = planeIntersection(ro, rd, max, vec3(0.0f, 1.0f, 0.0f));
    sect tdown = planeIntersection(ro, rd, min, vec3(0.0f, -1.0f, 0.0f));

    sect tleft = planeIntersection(ro, rd, min, vec3(-1.0f, 0.0f, 0.0f));
    sect tright = planeIntersection(ro, rd, max, vec3(1.0f, 0.0f, 0.0f));

    sect tfront = planeIntersection(ro, rd, max, vec3(0.0f, 0.0f, 1.0f));
    sect tback = planeIntersection(ro, rd, min, vec3(0.0f, 0.0f, -1.0f));

    
    // txmin is the lowest t value of tup and tdown, granted they both hit
    sect txmin;
    sect txmax;
    txmin.t = -INFINITY;
    txmax.t = INFINITY;
    if (tup.hit && tdown.hit)
    {
      if (tup.t < tdown.t)
      {
        txmin = tup;
        txmax = tdown;
      }
      else
      {
        txmin = tdown;
        txmax = tup;
      }
    }
    else if (tup.hit)
    {
      txmin = tup;
      txmax = tup;
    }
    else if (tdown.hit)
    {
      txmin = tdown;
      txmax = tdown; 
    }
    

    
    // tymin is the lowest t value of tleft and tright, granted they both hit
    sect tymin;
    sect tymax;
    tymin.t = -INFINITY;
    tymax.t = INFINITY;
    if (tleft.hit && tright.hit)
    {
      if (tleft.t < tright.t)
      {
        tymin = tleft;
        tymax = tright;
      }
      else
      {
        tymin = tright;
        tymax = tleft;
      }
    }
    else if (tleft.hit)
    {
      tymin = tleft;
      tymax = tleft;
    }
    else if (tright.hit)
    {
      tymin = tright;
      tymax = tright;
    }
    
    // tzmin is the lowest t value of tfront and tback, granted they both hit
    sect tzmin;
    sect tzmax;
    tzmin.t = -INFINITY;
    tzmax.t = INFINITY;
    if (tfront.hit && tback.hit)
    {
      if (tfront.t < tback.t)
      {
        tzmin = tfront;
        tzmax = tback;
      }
      else
      {
        tzmin = tback;
        tzmax = tfront;
      }
    }
    else if (tfront.hit)
    {
      tzmin = tfront;
      tzmax = tfront;
    }
    else if (tback.hit)
    {
      tzmin = tback;
      tzmax = tback;
    }
    
    sect tmin;
    sect tmax;
    
    // Finding tmin
    if (txmin.t > tymin.t)
    {
      if (txmin.t > tzmin.t)
        tmin = txmin;
      else
        tmin = tzmin;
    }
    else
    {
      if (tymin.t > tzmin.t)
        tmin = tymin;
      else
        tmin = tzmin;
    }
    
    // Finding tmax
    if (txmax.t < tymax.t)
    {
      if (txmax.t < tzmax.t)
        tmax = txmax;
      
      else
        tmax = tzmax;
    }
    else
    {
      if (tymax.t < tzmax.t)
        tmax = tymax;
      else
        tmax = tzmax;
    }
    

    if (tmin.t < tmax.t)
    {
      tmin.hit = true;
      return tmin;
    }
    tmin.hit = false;
    return tmin;
  }


  void mouseMotion(int x, int y, int dx, int dy) 
  {
    vec2 mousePos = mousePosition();
    vec3 worldPos = screenToWorld(mousePos);
    vec3 rayDir = normalize(worldPos - _eyePos);
    sect p = cubeIntersection(_eyePos, rayDir, _minOrig, _maxOrig);
    sect q;
    decorator c;
    sect mint;
    mint.t = INFINITY;
    for (int i = 0; i < _cubes.size(); i++)
    {
        c = _cubes[i];
        q = cubeIntersection(_eyePos, rayDir, c.min, c.max);

        if ((q.t < mint.t) && q.hit)
        {
          mint = q;
        }
    }
    
    if (((mint.t < p.t) && mint.hit) || !p.hit)
    {
      p = mint;
    }

    _showPrev = p.hit;
    vec3 newPos = vec3(
      p.pos.x,
      p.pos.y,
      p.pos.z
    );
    if (p.hit)
    {
      _posPrev = newPos;
      _normPrev = p.norm;
      if (_isModelPrev)
      {
        // Setting rotation of preview mesh, will be set for decorators
        if (_normPrev.z < -0.5)
        {
          _rotzPrev = - atan2(sqrt(pow(p.norm.y, 2) + pow(p.norm.z, 2)), p.norm.x);
        }
        else
        {
          _rotzPrev = atan2(sqrt(pow(p.norm.y, 2) + pow(p.norm.z, 2)), p.norm.x);
        }

        if (_normPrev.x < -0.5)
        {
          _rotxPrev = - atan2(sqrt(pow(p.norm.y, 2) + pow(p.norm.z, 2)), p.norm.x) + M_PI_2;
        }
        else
        {
          _rotxPrev = atan2(sqrt(pow(p.norm.y, 2) + pow(p.norm.z, 2)), p.norm.x) - M_PI_2;
        }

        if (_normPrev.y > 0.5)
        {
          _rotzPrev = 0.0;
          _rotxPrev = 0.0;
        }
        else if (_normPrev.y < -0.5)
        {
          _rotzPrev = M_PI;
          _rotxPrev = 0.0;
        }
      }
      else
      {
        _rotxPrev = 0.0f;
        _rotzPrev = 0.0f;
        _rotyPrev = 0.0f;
      }
      
    }
  }

  void mouseDown(int button, int mods) 
  {

    decorator thing;

    thing.pos = _posPrev;
    thing.rotx = _rotxPrev;
    thing.rotz = _rotzPrev;
    thing.roty = _rotyPrev;
    
    thing.scale = _scalePrev;
    thing.color = _colorPrev;
    thing.ply = _meshPrev;
    thing.max = vec3(
        thing.pos.x + (thing.scale.x / 2.0f),
        thing.pos.y + (thing.scale.y / 2.0f),
        thing.pos.z + (thing.scale.z / 2.0f)
    );
    thing.min = vec3(
        thing.pos.x - (thing.scale.x / 2.0f),
        thing.pos.y - (thing.scale.y / 2.0f),
        thing.pos.z - (thing.scale.z / 2.0f)
    );

    if (_showPrev) 
    {
      if(!_isModelPrev)
      {
        _cubes.push_back(thing);
      }
      else
      {
        _decorators.push_back(thing);
      }

    }
  }

  void mouseUp(int button, int mods) 
  {
  }

  void scroll(float dx, float dy) 
  {

    if (keyIsDown(GLFW_KEY_LEFT_CONTROL))
    {
      // Scrolling directly affects radius (zoom)
      _radius += dy * 0.05f;
    }
    else
    {
      // azimuth and elevation are updated via mouse
      azimuth -= dx * 0.05f;
      elevation += dy * 0.05f;
    }

    // _eyePos updated using new radius or angles
    
    _eyePos = vec3(
      _radius * sin(azimuth) * cos(elevation), 
      _radius * sin(elevation), 
      _radius * cos(azimuth) * cos(elevation)
    );

  }

  void keyDown(int key, int mods) 
  {

    if (key == 'w' || key == 'W')
    {
      wDown = true;
    }
    else if (key == 's' || key == 'S')
    {
      sDown = true;
    }
    
    if (key == 'a' || key == 'A')
    {
      aDown = true;
    }
    else if (key == 'd' || key == 'D')
    {
      dDown = true;
    }

    if (key == 'r' || key == 'R')
    {
      rDown = true;
    }
    else if (key == 'g' || key == 'G')
    {
      gDown = true;
    }
    else if (key == 'b' || key == 'B')
    {
      bDown = true;
    }
    if (key == 'i' || key == 'I')
    {
      iDown = true;
    }
    else if (key == 'k' || key == 'K')
    {
      kDown = true;
    }
    if (key == 'e' || key == 'E')
    {
      if (_curOption == _meshes.size() - 1)
      {
        _curOption = 0;
      }
      else
      {
        _curOption++;
      }
    }

  }

  void keyUp(int key, int mods) 
  {
    if (key == 'w' || key == 'W')
    {
      wDown = false;
    }
    if (key == 's' || key == 'S')
    {
      sDown = false;
    }
    if (key == 'a' || key == 'A')
    {
      aDown = false;
    }
    if (key == 'd' || key == 'D')
    {
      dDown = false;
    }
    if (key == 'r' || key == 'R')
    {
      rDown = false;
    }
    if (key == 'g' || key == 'G')
    {
      gDown = false;
    }
    if (key == 'b' || key == 'B')
    {
      bDown = false;
    }
    if (key == 'i' || key == 'I')
    {
      iDown = false;
    }
    if (key == 'k' || key == 'K')
    {
      kDown = false;
    }
  }

  // Function that updates things that require updating while a key is down to create gradual effects
  void srotcol()
  {
    if (wDown)
    {
      _scalePrev = vec3(
        _scalePrev.x + 0.005f,
        _scalePrev.y + 0.005f,
        _scalePrev.z + 0.005f
      );
    }
    else if (sDown)
    {
      if (_scalePrev.x >= 0.1f)
      {
        _scalePrev = vec3(
          _scalePrev.x - 0.005f,
          _scalePrev.y - 0.005f,
          _scalePrev.z - 0.005f
        );
      }
    }
    if (aDown)
    {
      if (_isModelPrev)
      _rotyPrev = _rotyPrev + 0.02f;
    }
    else if (dDown)
    {
      if (_isModelPrev)
      _rotyPrev = _rotyPrev - 0.02f;
    }
    if (rDown)
    {
      _colorPrev = vec3(
        std::min((_colorPrev.r + 0.02f), 1.0f),
        _colorPrev.g,
        _colorPrev.b
      );
    }
    else if (gDown)
    {
      _colorPrev = vec3(
        _colorPrev.r,
        std::min((_colorPrev.g + 0.02f), 1.0f),
        _colorPrev.b
      );
    }
    else if (bDown)
    {
      _colorPrev = vec3(
        _colorPrev.r,
        _colorPrev.g,
        std::min((_colorPrev.b + 0.02f), 1.0f)
      );
    }
    if (iDown)
    {
      _colorPrev = vec3(
        std::min((_colorPrev.r + 0.02f), 1.0f),
        std::min((_colorPrev.g + 0.02f), 1.0f),
        std::min((_colorPrev.b + 0.02f), 1.0f)
      );
    }
    else if (kDown)
    {
      _colorPrev = vec3(
        std::max((_colorPrev.r - 0.02f), 0.0f),
        std::max((_colorPrev.g - 0.02f), 0.0f),
        std::max((_colorPrev.b - 0.02f), 0.0f)
      );
    }
  }

  // Function drawing non-cube decorations
  void drawDecorators()
  {
    for (int i = 0; i < _decorators.size(); i++)
    {
      decorator dec = _decorators[i];
      if (dec.ply == "eye")
      {
        renderer.setUniform("text", true);
        renderer.texture("Image", "eye");
      }
      else if (dec.ply == "horn")
      {
        renderer.setUniform("text", true);
        renderer.texture("Image", "horn");
      }
      else if (dec.ply == "duck")
      {
        renderer.setUniform("text", true);
        renderer.texture("Image", "duck");
      }
      else if (dec.ply == "mouth")
      {
        renderer.setUniform("text", true);
        renderer.texture("Image", "mouth");
      }
      else
      {
        renderer.setUniform("text", false);
      }
      renderer.push();
      renderer.setUniform("diffuseColor", vec4(dec.color, 1.0));
      renderer.identity();

      renderer.translate(dec.pos);
      renderer.rotate(dec.rotx, vec3(0.0, 0.0, 1.0));
      renderer.rotate(dec.roty, vec3(0.0, 1.0, 0.0));
      renderer.rotate(dec.rotz, vec3(1.0, 0.0, 0.0));
      renderer.scale(dec.scale);
      if (dec.ply == "eye")
      {
        renderer.mesh(_eyeMesh);
      }
      else if (dec.ply == "horn")
      {
        renderer.mesh(_hornMesh);
      }
      else if (dec.ply == "nose")
      {
        renderer.mesh(_noseMesh);
      }
      else if (dec.ply == "duck")
      {
        renderer.mesh(_duckMesh);
      }
      else if (dec.ply == "mouth")
      {
        renderer.mesh(_mouthMesh);
      }
      else renderer.cube();
      renderer.pop();
    }

  }

  // Function drawing new cubes
  void drawCubes()
  {
    for (int i = 0; i < _cubes.size(); i++)
    {
      decorator c = _cubes[i];
      renderer.setUniform("text", false);
      renderer.setUniform("diffuseColor", vec4(c.color, 1));
      renderer.push();
      renderer.identity();
      renderer.translate(c.pos);
      renderer.scale(c.scale);
      renderer.cube();
      renderer.pop();
    }
  }

  void draw() 
  {
    // Makes sure if we have a mesh or not
    _meshPrev = _meshes[_curOption];
    if (_meshPrev == "cube")
    {
      _isModelPrev = false;
    }
    else
    _isModelPrev = true;

    // control update
    srotcol();

    // starts with a shader and no texture
    renderer.beginShader("phong-pixel");
    renderer.setUniform("text", false);

    // renderer.setUniform() to set values in shader

    // Currently hard-set perspective since mac width and height is wrong
    float aspect = ((float)width()) / height();
    renderer.perspective(glm::radians(60.0f), 1, 0.5f, 10);

    renderer.lookAt(_eyePos, lookPos, up);

    // sets color to white for origin cube
    renderer.setUniform("diffuseColor", vec4(1,1,1,1));
    renderer.identity();
    renderer.setUniform("text", false);
    renderer.translate(_posOrig);
    renderer.cube();



    // The preview mesh
    renderer.setUniform("diffuseColor", vec4(_colorPrev, 0.5));
    renderer.identity();
    renderer.translate(_posPrev);
    renderer.rotate(_rotxPrev, vec3(0.0, 0.0, 1.0));
    renderer.rotate(_rotzPrev, vec3(1.0, 0.0, 0.0));
    renderer.rotate(_rotyPrev, vec3(0.0, 1.0, 0.0));
    renderer.scale(_scalePrev);
    if (_showPrev)
    {
      renderer.setUniform("text", true);
      if (_meshPrev == "eye")
      {
        renderer.texture("Image", "eye");
      }
      else if (_meshPrev == "horn")
      {
        renderer.texture("Image", "horn");
      }
      else if(_meshPrev == "duck")
      {
        renderer.texture("Image", "duck");
      }
      else if (_meshPrev == "mouth")
      {
        renderer.texture("Image", "mouth");
      }
      else
      {
        renderer.setUniform("text", false);
      }

      renderer.push();
      if (_meshPrev == "eye")
      {
        renderer.mesh(_eyeMesh);
      }
      else if (_meshPrev == "horn")
      {
        renderer.mesh(_hornMesh);
      }
      else if (_meshPrev == "nose")
      {
        renderer.mesh(_noseMesh);
      }
      else if (_meshPrev == "duck")
      {
        renderer.mesh(_duckMesh);
      }
      else if (_meshPrev == "mouth")
      {
        renderer.mesh(_mouthMesh);
      }
      else
      {
        renderer.cube();
      }
      renderer.pop();
    }

    drawCubes();
    drawDecorators();

    renderer.endShader();
  }

protected:

  PLYMesh _eyeMesh;
  PLYMesh _hornMesh;
  PLYMesh _duckMesh;
  PLYMesh _noseMesh;
  PLYMesh _mouthMesh;

  vec3 _eyePos = vec3(0, 0, 5);
  vec3 lookPos = vec3(0, 0, 0);
  vec3 up = vec3(0, 1, 0);
  float elevation = 0.0f;
  float azimuth = 0.0f;
  float _radius = 5;
  bool crtlDown = false;
  bool wDown = false;
  bool sDown = false;
  bool aDown = false;
  bool dDown = false;
  bool rDown = false;
  bool gDown = false;
  bool bDown = false;
  bool iDown = false;
  bool kDown = false;
  int _height = 1000;
  int _width = 1000;

  // Origin cube info
  vec3 _posOrig = vec3(0, 0, 0);
  vec3 _scaleOrig = vec3(1.0f, 1.0f, 1.0f);
  vec3 _maxOrig = vec3((_posOrig.x + (_scaleOrig.x / 2.0f)), 
                    (_posOrig.y + (_scaleOrig.y / 2.0f)), 
                    (_posOrig.z + (_scaleOrig.z / 2.0f)));
  vec3 _minOrig = vec3((_posOrig.x - (_scaleOrig.x / 2.0f)), 
                    (_posOrig.y - (_scaleOrig.y / 2.0f)), 
                    (_posOrig.z - (_scaleOrig.z / 2.0f)));
  // Preview decoration info
  vec3 _posPrev;
  vec3 _scalePrev = vec3(0.1f, 0.1f, 0.1f);
  float _rotxPrev = 0.0f;
  float _rotzPrev = 0.0f;
  float _rotyPrev = 0.0f;
  vec3 _normPrev;
  bool _showPrev;
  vec3 _colorPrev = vec3(0.0f, 0.0f, 0.0f);
  string _meshPrev;
  bool _isModelPrev;

  // vectors of decorators and new cubes
  std::vector<decorator> _decorators;
  std::vector<decorator> _cubes;

  // vector of mesh names. Determines order of meshes when pressing e
  std::vector<string> _meshes;

  int _curOption = 0;
};

int main(int argc, char** argv)
{
  Viewer viewer;
  viewer.run();
  return 0;
}
