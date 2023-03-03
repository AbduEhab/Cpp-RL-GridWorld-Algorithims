#pragma once

#include "glm/glm.hpp"
#include <vector>

auto gauss_solve3x3(glm::mat3 &m, glm::vec3 &v) -> void
{
    // forward elimination
    for (size_t i = 0; i < 3; i++)
    {
        // find the row with the largest pivot
        size_t max_row = i;
        for (size_t j = i + 1; j < 3; j++)
        {
            if (std::abs(m[j][i]) > std::abs(m[max_row][i]))
            {
                max_row = j;
            }
        }

        // swap the rows
        std::swap(m[i], m[max_row]);
        std::swap(v[i], v[max_row]);

        // eliminate the column
        for (size_t j = i + 1; j < 3; j++)
        {
            float f = m[j][i] / m[i][i];
            for (size_t k = i + 1; k < 3; k++)
            {
                m[j][k] -= m[i][k] * f;
            }
            v[j] -= v[i] * f;
        }
    }

    // back substitution
    for (size_t i = 3 - 1; i > 0; i--)
    {
        for (size_t j = i - 1; j > 0; j--)
        {
            float f = m[j][i] / m[i][i];
            m[j][i] -= m[i][i] * f;
            v[j] -= v[i] * f;
        }
    }

    // normalize the diagonal
    for (size_t i = 0; i < 3; i++)
    {
        v[i] /= m[i][i];
        m[i][i] /= m[i][i];
    }
}

auto gauss_solve(std::vector<std::vector<float>> &mat, std::vector<float> &vec) -> void
{
    const size_t n = mat.size();

    // forward elimination
    for (size_t i = 0; i < n; i++)
    {
        // find the row with the largest pivot
        size_t max_row = i;
        for (size_t j = i + 1; j < n; j++)
        {
            if (std::abs(mat[j][i]) > std::abs(mat[max_row][i]))
            {
                max_row = j;
            }
        }

        // swap the rows
        std::swap(mat[i], mat[max_row]);
        std::swap(vec[i], vec[max_row]);

        // eliminate the column
        for (size_t j = i + 1; j < n; j++)
        {
            float f = mat[j][i] / mat[i][i];
            for (size_t k = i + 1; k < n; k++)
            {
                mat[j][k] -= mat[i][k] * f;
            }
            vec[j] -= vec[i] * f;
        }
    }

    // back substitution
    for (int i = n - 1; i >= 0; i--)
    {
        for (int j = i - 1; j >= 0; j--)
        {
            float f = mat[j][i] / mat[i][i];
            mat[j][i] -= mat[i][i] * f;
            vec[j] -= vec[i] * f;
        }
    }

    // normalize the diagonal
    for (size_t i = 0; i < n; i++)
    {
        vec[i] /= mat[i][i];
        mat[i][i] /= mat[i][i];
    }
}