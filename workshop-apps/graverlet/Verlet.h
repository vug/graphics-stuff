#pragma once

#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

#include <array>
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

// TODO: how to deal with stray planents (with high index) -> put them into some maximum value bucket
// TODO: "optimized Verlet" that uses spatialAccelration structure
// TODO: list of objects that won't be included in cells
// TODO: use hash_pair_simple and compare perf
class SpatialAccelarator
{
public:
  using PositionIndex = std::pair<int, int>;

  PositionIndex getPosIndex(const VerletObject &obj)
  {
    const int i = static_cast<int>(std::floor(obj.pos.x / cellSize));
    const int j = static_cast<int>(std::floor(obj.pos.y / cellSize));
    return std::make_pair(i, j);
  }

  template <class T>
  static inline void hash_combine(std::size_t &seed, const T &v)
  {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  struct hash_pair
  {
    std::size_t operator()(const PositionIndex &p) const
    {
      std::size_t h = 0;
      hash_combine(h, p.first);
      hash_combine(h, p.second);
      return h;
    }
  };

  struct hash_pair_simple
  {
    std::size_t operator()(const std::pair<int, int> &p) const
    {
      return p.first * 10'000 + p.second;
    }
  };

  using PositionHashMap = std::unordered_map<PositionIndex, std::vector<std::reference_wrapper<const VerletObject>>, hash_pair>;
  PositionHashMap cache;
  std::unordered_map<PositionIndex, VerletObject, hash_pair> cellAverages;

  float cellSize{0.1f};

  using ObjRefIter = std::vector<std::reference_wrapper<const VerletObject>>::iterator;
  using CellIter = std::vector<PositionIndex>::iterator;

  struct NeighboringObjectsIterator
  {
    NeighboringObjectsIterator(SpatialAccelarator &acc, const CellIter &cellIter, const ObjRefIter &objRefIter, const CellIter &lastCellIter)
        : acc{acc}, cellIter{cellIter}, objRefIter{objRefIter}, lastCellIter{lastCellIter} {}

    // or `friend bool operator!=(const Iter& left, const Iter& right)
    bool operator!=(const NeighboringObjectsIterator &other)
    {
      return cellIter != other.cellIter || objRefIter != other.objRefIter;
    }

    NeighboringObjectsIterator &operator++()
    {
      const auto &currentCell = acc.cache[*cellIter];
      ++objRefIter;
      if (objRefIter == currentCell.end() && cellIter != lastCellIter)
      {
        ++cellIter;
        objRefIter = acc.cache[*cellIter].begin();
      }
      return *this;
    }

    const VerletObject &operator*() const
    {
      return objRefIter->get();
    }

  private:
    SpatialAccelarator &acc;
    CellIter cellIter;
    ObjRefIter objRefIter;
    const CellIter &lastCellIter;
  };

  struct NeighboringObjectsRange
  {
    NeighboringObjectsRange(SpatialAccelarator &acc, const PositionIndex &posIdx) : acc{acc}
    {
      std::array<PositionIndex, 9> relIdxs = {PositionIndex{0, 0}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
      for (const PositionIndex &relIx : relIdxs)
      {
        const PositionIndex neighPosIx = {posIdx.first + relIx.first, posIdx.second + relIx.second};
        if (acc.cache.contains(neighPosIx))
          neighborCellIdxs.push_back(neighPosIx);
      }

      assert(neighborCellIdxs.size() > 0);
      cellsBeginIter = neighborCellIdxs.begin();
      objBeginIter = acc.cache[*cellsBeginIter].begin();

      cellsLastIter = neighborCellIdxs.end() - 1;
      objEndIter = acc.cache[*cellsLastIter].end();
    }

    // NeighboringObjectsRange(SpatialAccelarator &acc, const VerletObject &obj) : NeighboringObjectsRange(acc, acc.getPosIndex(obj)) {}

    NeighboringObjectsIterator begin() const
    {
      return NeighboringObjectsIterator(acc, cellsBeginIter, objBeginIter, cellsLastIter);
    }

    NeighboringObjectsIterator end() const
    {
      return NeighboringObjectsIterator(acc, cellsLastIter, objEndIter, cellsLastIter);
    }

  private:
    SpatialAccelarator &acc;
    std::vector<PositionIndex> neighborCellIdxs;
    CellIter cellsBeginIter;
    CellIter cellsLastIter; // not end!
    ObjRefIter objBeginIter;
    ObjRefIter objEndIter;
  };

  SpatialAccelarator(const std::vector<VerletObject> &objects, const float cellSize = 0.1f)
      : cellSize(cellSize)
  {
    for (const auto &obj : objects)
      cache[getPosIndex(obj)].push_back(obj);

    for (const auto &[pIx, vec] : cache)
    {
      auto &avgObj = cellAverages[pIx];
      avgObj.pos = {pIx.first * cellSize + cellSize * 0.5f, pIx.second * cellSize + cellSize * 0.5f};
      for (const auto &obj : vec)
        avgObj.mass += obj.get().mass;
    }
  }

  NeighboringObjectsRange neighborsOf(const PositionIndex &posIdx)
  {
    return NeighboringObjectsRange(*this, posIdx);
  }

  NeighboringObjectsRange neighborsOf(const VerletObject &obj)
  {
    return NeighboringObjectsRange(*this, getPosIndex(obj));
  }

  void debugPrint()
  {
    for (auto &[key, v] : cache)
    {
      printf("[%d, %d]: %zu\n", key.first, key.second, v.size());
    }
    printf("******************\n\n");
  }
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
    // http://itf.fys.kuleuven.be/~enrico/Teaching/molecular_dynamics.pdf
    // "Note that the [Velocity Verlet] algorithm is identical to that of Eqs. (8) and (9).
    // The difference is that Eq. (9) [this one] is implemented in two steps.
    // The new implementation requires less memory: there is no need to store data at two different time steps."
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