#include "mhpch.h"
#include "SphericalHarmonics.h"

namespace Mahakam
{
    SphericalHarmonics::SphericalHarmonics()
    {
    }

    SphericalHarmonics::~SphericalHarmonics()
    {
    }

    std::shared_ptr<SphericalHarmonics> SphericalHarmonics::createForCubeMap(size_t const resolutionX, size_t const resolutionY)
    {
        auto result = std::make_shared<SphericalHarmonics>();
        for (size_t side = 0; side < 6; ++side)
        {
            for (size_t y = 0; y < resolutionY; ++y)
            {
                for (size_t x = 0; x < resolutionX; ++x)
                {
                    // Calculate directional vector  
                    // TODO: Correct direction to sample center of texel.  
                    glm::vec3 direction((float)x / (float)(resolutionX - 1) * 2.0f - 1.0f, (float)y / (float)(resolutionY - 1) * 2.0f - 1.0f, -1.0f);
                    direction = glm::normalize(direction);
                    // Rotate to match cubemap side  
                    switch (static_cast<CubeMapSides>(side))
                    {
                    case CubeMapSides::Back:
                    {
                        direction[0] *= -1.0f;
                        direction[2] *= -1.0f;
                    }break;
                    case CubeMapSides::Left:
                    {
                        float const temp = direction[0];
                        direction[0] = direction[2];
                        direction[2] = -temp;
                    } break;
                    case CubeMapSides::Right:
                    {
                        float const temp = direction[0];
                        direction[0] = -direction[2];
                        direction[2] = temp;
                    } break;
                    case CubeMapSides::Top:
                    {
                        float const temp = direction[1];
                        direction[1] = -direction[2];
                        direction[2] = temp;
                    } break;
                    case CubeMapSides::Bottom:
                    {
                        float const temp = direction[1];
                        direction[1] = direction[2];
                        direction[2] = -temp;
                    } break;
                    default:
                        break;
                    }
                    Sample sample;
                    sample.cubeMapSide = static_cast<CubeMapSides>(side);
                    sample.x = x;
                    sample.y = y;
                    sample.coefficients[0] = 0.282095f;
                    sample.coefficients[1] = 0.488603f * direction[1];
                    sample.coefficients[2] = 0.488603f * direction[2];
                    sample.coefficients[3] = 0.488603f * direction[0];
                    sample.coefficients[4] = 1.092548f * direction[0] * direction[1];
                    sample.coefficients[5] = 1.092548f * direction[1] * direction[2];
                    sample.coefficients[6] = 0.315392f * (3.0f * direction[2] * direction[2] - 1.0f);
                    sample.coefficients[7] = 1.092548f * direction[0] * direction[2];
                    sample.coefficients[8] = 0.546274f * (direction[0] * direction[0] - direction[1] * direction[1]);
                    result->_samples.push_back(sample);
                }
            }
        }
        return result;
    }

    void SphericalHarmonics::sampleCubeMap(std::function<glm::vec3(CubeMapSides const, size_t const, size_t const)> func)
    {
        for (size_t i = 0; i < 9; ++i)
        {
            _coefficients[i] = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        for (auto const& sample : _samples)
        {
            auto const value = func(sample.cubeMapSide, sample.x, sample.y);
            for (size_t i = 0; i < 9; ++i)
            {
                _coefficients[i] += value * sample.coefficients[i];
            }
        }
        float const factor = 4.0f * 3.1415f / static_cast<float>(_samples.size());
        for (size_t i = 0; i < 9; ++i)
        {
            _coefficients[i] *= factor;
        }
    }

    void SphericalHarmonics::evaluateCubeMap(std::function<void(CubeMapSides const, size_t const, size_t const, glm::vec3 const)> func) const
    {
        for (auto const& sample : _samples)
        {
            glm::vec3 value(0.0f, 0.0f, 0.0f);
            for (size_t i = 0; i < 9; ++i)
            {
                value += sample.coefficients[i] * _coefficients[i];
            }
            func(sample.cubeMapSide, sample.x, sample.y, value);
        }
    }
}