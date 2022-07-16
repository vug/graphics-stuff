#pragma once

#include "Verlet.h"

#include <glm/vec2.hpp>
#include <implot.h>

#include <algorithm>
#include <vector>

class PlotBuffer
{
private:
  size_t maxSize = 1024;
  size_t offset = 0;
  std::vector<glm::vec2> data;

public:
  PlotBuffer()
  {
    data.reserve(maxSize);
  }

  PlotBuffer(size_t maxSize) : maxSize(maxSize)
  {
    data.reserve(maxSize);
  }

  void addPoint(const glm::vec2 &p)
  {
    if (data.size() < maxSize)
      data.push_back(p);
    else
    {
      data[offset] = p;
      offset = (offset + 1) % maxSize;
    }
  }

  const std::vector<glm::vec2> &getData() const
  {
    return data;
  }

  const size_t &getOffset() const
  {
    return offset;
  }
};

class EnergiesPlot
{
private:
  PlotBuffer potentials;
  PlotBuffer kinetics;
  PlotBuffer totals;
  float yMin{}, yMax{};
  float tMin{}, tMax{};

public:
  EnergiesPlot(size_t numPoints)
      : potentials(numPoints), kinetics(numPoints), totals(numPoints) {}
  void
  addEnergyPoints(float time, float potential, float kinetic, float total)
  {
    potentials.addPoint({time, potential});
    kinetics.addPoint({time, kinetic});
    totals.addPoint({time, total});

    const bool allTime = false;
    if (allTime)
    {
      yMin = std::min(std::min(std::min(yMin, potential), kinetic), total);
      yMax = std::max(std::max(std::max(yMax, potential), kinetic), total);
    }
    else
    {
      static const auto comp = [](const glm::vec2 &a, const glm::vec2 &b)
      { return a.y < b.y; };
      const auto [potMin, potMax] = std::minmax_element(potentials.getData().begin(), potentials.getData().end(), comp);
      const auto [kinMin, kinMax] = std::minmax_element(kinetics.getData().begin(), kinetics.getData().end(), comp);
      const auto [totMin, totMax] = std::minmax_element(totals.getData().begin(), totals.getData().end(), comp);
      yMin = std::min(std::min(potMin->y, kinMin->y), totMin->y);
      yMax = std::max(std::max(potMax->y, kinMax->y), totMax->y);
    }

    {
      static const auto comp = [](const glm::vec2 &a, const glm::vec2 &b)
      { return a.x < b.x; };
      const auto [mn, mx] = std::minmax_element(potentials.getData().begin(), potentials.getData().end(), comp);
      tMin = mn->x;
      tMax = mx->x;
    }
  }

  void plot(const ImVec2 &size)
  {
    if (ImPlot::BeginPlot("Energies", size))
    {
      const auto &pData = potentials.getData();
      const auto &kData = kinetics.getData();
      const auto &tData = totals.getData();
      ImPlot::SetupAxes("time", "energy", ImPlotAxisFlags_None, ImPlotAxisFlags_None);

      ImPlot::SetupAxis(ImAxis_X1, "time", ImPlotAxisFlags_AuxDefault);
      ImPlot::SetupAxisLimits(ImAxis_X1, tMin, tMax, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, yMin - 0.1f, yMax + 0.1f, ImGuiCond_Always);

      ImPlot::PlotLine("Potential", &pData[0].x, &pData[0].y, static_cast<int>(pData.size()), 0, static_cast<int>(potentials.getOffset()), 2 * sizeof(float));
      ImPlot::PlotLine("Kinetic", &kData[0].x, &kData[0].y, static_cast<int>(kData.size()), 0, static_cast<int>(kinetics.getOffset()), 2 * sizeof(float));
      ImPlot::PlotLine("Total", &tData[0].x, &tData[0].y, static_cast<int>(tData.size()), 0, static_cast<int>(totals.getOffset()), 2 * sizeof(float));
      ImPlot::EndPlot();
    }
  }
};

void plotOriginalAndSoftenedGravitationalForces(InterPotential original, InterPotential softened, float xMax = 5.0f, float yMin = -5.0f)
{
  const size_t numPoints = 1024;
  static std::vector<float> xs(numPoints);
  static std::vector<float> ysOriginal(numPoints);
  static std::vector<float> ysSoftened(numPoints);

  if (ImPlot::BeginPlot("Original & Softened", {250, 250}))
  {
    ImPlot::SetupAxes("distance", "potential", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
    ImPlot::SetupAxisLimits(ImAxis_X1, 0.001f, xMax, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, 0.0f, ImGuiCond_Always);
    VerletObject o1, o2;
    o1.pos = {};
    for (size_t i = 0; i < numPoints; ++i)
    {
      xs[i] = static_cast<float>(i) / numPoints * 5.0f + 0.001f;
      o2.pos = {0, xs[i]};
      ysOriginal[i] = original(o1, o2);
      ysSoftened[i] = softened(o1, o2);
    }
    ImPlot::PlotLine("Original", xs.data(), ysOriginal.data(), static_cast<int>(ysOriginal.size()), 0, 0, sizeof(float));
    ImPlot::PlotLine("Softened", xs.data(), ysSoftened.data(), static_cast<int>(ysSoftened.size()), 0, 0, sizeof(float));
    ImPlot::EndPlot();
  }
}