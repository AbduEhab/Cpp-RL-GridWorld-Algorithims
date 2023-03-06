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

// gauss_solve4x4
auto gauss_solve4x4(glm::mat4 &m, glm::vec4 &v) -> void
{
    // forward elimination
    for (size_t i = 0; i < 4; i++)
    {
        // find the row with the largest pivot
        size_t max_row = i;
        for (size_t j = i + 1; j < 4; j++)
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
        for (size_t j = i + 1; j < 4; j++)
        {
            float f = m[j][i] / m[i][i];
            for (size_t k = i + 1; k < 4; k++)
            {
                m[j][k] -= m[i][k] * f;
            }
            v[j] -= v[i] * f;
        }
    }

    // back substitution
    for (size_t i = 4 - 1; i > 0; i--)
    {
        for (size_t j = i - 1; j > 0; j--)
        {
            float f = m[j][i] / m[i][i];
            m[j][i] -= m[i][i] * f;
            v[j] -= v[i] * f;
        }
    }

    // normalize the diagonal
    for (size_t i = 0; i < 4; i++)
    {
        v[i] /= m[i][i];
        m[i][i] /= m[i][i];
    }
}

auto gauss_solve(double *matrix, double *vector, size_t mat_size)
{
    // forward elimination
    for (size_t i = 0; i < mat_size; i++)
    {
        // find the row with the largest pivot
        size_t max_row = i;
        for (size_t j = i + 1; j < mat_size; j++)
        {
            if (std::abs(matrix[j * mat_size + i]) > std::abs(matrix[max_row * mat_size + i]))
            {
                max_row = j;
            }
        }

        // swap the rows
        for (size_t k = 0; k < mat_size; k++)
        {
            std::swap(matrix[i * mat_size + k], matrix[max_row * mat_size + k]);
        }
        std::swap(vector[i], vector[max_row]);

        // eliminate the column
        for (size_t j = i + 1; j < mat_size; j++)
        {
            float f = matrix[j * mat_size + i] / matrix[i * mat_size + i];
            for (size_t k = i + 1; k < mat_size; k++)
            {
                matrix[j * mat_size + k] -= matrix[i * mat_size + k] * f;
            }
            vector[j] -= vector[i] * f;
        }
    }

    // back substitution
    for (size_t i = mat_size - 1; i > 0; i--)
    {
        for (size_t j = i - 1; j > 0; j--)
        {
            float f = matrix[j * mat_size + i] / matrix[i * mat_size + i];
            matrix[j * mat_size + i] -= matrix[i * mat_size + i] * f;
            vector[j] -= vector[i] * f;
        }
    }

    // normalize the diagonal
    for (size_t i = 0; i < mat_size; i++)
    {
        vector[i] /= matrix[i * mat_size + i];
        matrix[i * mat_size + i] /= matrix[i * mat_size + i];
    }
}
