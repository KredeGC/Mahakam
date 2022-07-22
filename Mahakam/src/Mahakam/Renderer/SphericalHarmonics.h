#pragma once
#include <functional>
#include <vector>

#include <glm/glm.hpp>

namespace Mahakam
{
    class SphericalHarmonics
    {
    public:
        enum class CubeMapSides
        {
            Top,
            Bottom,
            Back,
            Forward,
            Left,
            Right,
        };
        struct Sample
        {
            CubeMapSides cubeMapSide;
            size_t x;
            size_t y;
            float coefficients[9];
        };

    private:
        glm::vec3 _coefficients[9];
        std::vector<Sample> _samples;

    public:
        SphericalHarmonics();
        virtual ~SphericalHarmonics();

        static std::shared_ptr<SphericalHarmonics> createForCubeMap(size_t const resolutionX, size_t const resolutionY);
        void sampleCubeMap(std::function<glm::vec3(CubeMapSides const, size_t const, size_t const)> func);
        void evaluateCubeMap(std::function<void(CubeMapSides const, size_t const, size_t const, glm::vec3 const)> func) const;
    };
}