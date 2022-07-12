#pragma once

#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

#include <functional>
#include <vector>

struct VerletObject
{
  glm::vec2 pos{};
  glm::vec2 vel{};
  float mass = 1.0f;
  float radius = 0.1f;
  glm::vec2 acc{};
};

using InterForce = std::function<glm::vec2(const VerletObject &obj1, const VerletObject &obj2)>;
using InterPotential = std::function<float(const VerletObject &obj1, const VerletObject &obj2)>;

class Solver
{
public:
  std::vector<VerletObject> &objects;
  InterForce interObjectForce;
  InterPotential interObjectPotential;
  float potential{};
  float kinetic{};

public:
  Solver(std::vector<VerletObject> &objects, InterForce interObjectForce, InterPotential interObjectPotential = nullptr)
      : objects(objects), interObjectForce(interObjectForce), interObjectPotential(interObjectPotential)
  {
    // calculate initial acc
    for (size_t i = 0; i < objects.size(); ++i)
    {
      for (size_t j = i + 1; j < objects.size(); ++j)
      {
        VerletObject &o1 = objects[i];
        VerletObject &o2 = objects[j];
        const glm::vec2 f = interObjectForce(o1, o2);
        o1.acc -= f / o1.mass;
        o2.acc += f / o2.mass;
      }
    }
  }

  void update(float period, int numIter)
  {
    period /= numIter;
    potential = 0.0f;
    kinetic = 0.0f;
    for (int n = 0; n < numIter; ++n)
    {
      // p[t + dt] = p[t] + v[t] dt + 1/2 a dt^2
      for (VerletObject &obj : objects)
        obj.pos += obj.vel * period + obj.acc * (period * period * 0.5f);

      // v[t + dt / 2] = v[t] + 1/2 a[t] dt
      for (VerletObject &obj : objects)
      {
        obj.vel += 0.5f * obj.acc * period;
        // after using acc reset it for the next computation/accumulation
        obj.acc = {};
      }

      // a[t + dt] = 1/m f(p[t + dt])
      for (size_t i = 0; i < objects.size(); ++i)
      {
        for (size_t j = 0; j < objects.size(); ++j)
        {
          VerletObject &o1 = objects[i];
          VerletObject &o2 = objects[j];
          o1.acc -= interObjectForce(o1, o2) / o1.mass;
          if (interObjectPotential && n == numIter - 1)
            potential += interObjectPotential(o1, o2);
        }
      }

      // v[t + dt] = v[t + dt / 2] + 1/2 a[t + dt] dt
      for (VerletObject &obj : objects)
      {
        obj.vel += 0.5f * obj.acc * period;
        if (n == numIter - 1)
          kinetic += 0.5f * obj.mass * glm::dot(obj.vel, obj.vel);
      }
    }
  }
};