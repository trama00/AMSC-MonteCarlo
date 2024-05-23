#include "../include/project/financeinputmanager.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <filesystem>

int getIntegrationBounds(std::vector<double> &integration_bounds, const std::vector<Asset> &assets, int std_dev_from_mean /* = 24 */)
{
    try
    {
        int j = 0;
        for (size_t i = 0; i < assets.size() * 2 - 1; i += 2)
        {
            integration_bounds[i]     = assets[j].getReturnMean() - std_dev_from_mean * assets[j].getReturnStdDev() + 1.0;
            integration_bounds[i + 1] = assets[j].getReturnMean() + std_dev_from_mean * assets[j].getReturnStdDev() + 1.0;
            j++;
        }
    }
    catch (const std::exception &e)
    {
        return -1;
    }
    return 0;
}

int loadAssets(const std::string &directory, std::vector<Asset> &assets)
{
    try
    {
          // Iterate over each entry in the directory "../data/"
        for (const auto &entry : std::filesystem::directory_iterator(directory))
        {
              // Check if the entry is a regular file and has the ".csv" extension
            if (entry.is_regular_file() && entry.path().extension() == ".csv")
            {
                Asset asset;
                  // Get the filename without the extension
                std::string filename = entry.path().stem().string();
                  // Set the name of the asset to the filename
                asset.setName(filename);
                  // Read the CSV file and populate the asset object
                int csv_result = extrapolateCsvData(entry.path().string(), &asset);
                  // Check if there was an error reading the CSV file
                if (csv_result == -1)
                {
                    std::cout << "Error reading the file " << filename << std::endl;
                    continue;  // Skip to the next file
                }
                  // Add the asset to the vector of assets
                assets.push_back(asset);
            }
        }
    }
    catch (std::filesystem::filesystem_error &e)
    {
          // Handle the case where the directory couldn't be opened
        std::cout << "Could not open directory: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

int extrapolateCsvData(const std::string &filename, Asset *asset_ptr)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return -1;
    }

    std::string line;
      // Skip the header line
    std::getline(file, line);

    double total_return_percentage = 0.0;
    int    counter                 = 0;
    double squared_deviation       = 0.0;
    double return_std_dev          = 0.0;
    double closing_price           = 0.0;
    std::vector<double> daily_returns;

      // Process each line of the file
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string trash;       // Variable to store unused fields
        std::string temp_open;   // Variable to store the open price
        std::string temp_close;  // Variable to store the close price

          // Extract and discard date
        std::getline(ss, trash, ',');

          // Extract and store the open price
        std::getline(ss, temp_open, ',');

          // Extract and discard high and low prices
        std::getline(ss, trash, ',');
        std::getline(ss, trash, ',');

          // Extract and store the close price
        std::getline(ss, temp_close, ',');
        closing_price = std::stod(temp_close);
        daily_returns.push_back((std::stod(temp_close) - std::stod(temp_open)) / std::stod(temp_open));
        total_return_percentage += daily_returns[counter];
        counter++;
    }

      // Close the file
    file.close();

      // Calculate the mean return
    double return_mean_percentage = total_return_percentage / static_cast<double>(counter);

      // Calculate variance
    for (int i = 0; i < counter; i++)
    {
        squared_deviation += (daily_returns[i] - return_mean_percentage) * (daily_returns[i] - return_mean_percentage);
    }
    return_std_dev = std::sqrt(squared_deviation / static_cast<double>(counter));

      // Update the Asset object with the accumulated values
    if (asset_ptr)
    {
        asset_ptr->setReturnMean(return_mean_percentage);
        asset_ptr->setReturnStdDev(return_std_dev);
        asset_ptr->setLastRealValue(closing_price);
    }
      // Return success
    return 0;
}