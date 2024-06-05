#ifndef FINANCE_ENUMS_HPP
#define FINANCE_ENUMS_HPP

// Enum for asset loading errors
enum class LoadAssetError
{
    Success,           /**< Indicates successful asset loading */
    DirectoryOpenError,/**< Error opening the directory */
    NoValidFiles,      /**< No valid files found in the directory */
    FileReadError      /**< Error reading the file */
};

// Enum for option types
enum class OptionType {
    European = 1,
    Asian,
    Invalid
};

// Enum for asset count types
enum class AssetCountType {
    Single = 1,
    Multiple,
    Invalid
};

enum class CovarianceError {
    Success,
    Failure
};

enum class MonteCarloError {
    Success,
    PointGenerationFailed,
};

#endif 