#pragma once

namespace sql
{
    /**
     * \brief Specifies whether to bind fixed and/or dynamic parameters.
     */
    enum class BindParameters
    {
        // Don't bind any parameters.
        None = 0,
        // Bind fixed parameters only.
        Fixed = 1,
        // Bind dynamic parameters only.
        Dynamic = 2,
        // Bind fixed and dynamic parameters.
        All = Fixed | Dynamic
    };
}  // namespace sql
