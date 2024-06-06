#include "../../include/optionpricing/optionpricer.hpp"

  // Function that embeds multiple methods that are used to compute
  // the option price using the Monte Carlo method
void financeComputation()
{
    std::vector<Asset> assets;

    OptionType option_type = getOptionTypeFromUser();
    if (option_type == OptionType::Invalid)
    {
        std::cerr << "\nInvalid option type" << std::endl;
        exit(1);
    }

    AssetCountType asset_count_type = getAssetCountTypeFromUser();
    if (asset_count_type == AssetCountType::Invalid)
    {
        std::cerr << "\nInvalid asset count type" << std::endl;
        exit(1);
    }

      // Load the assets from the CSV files
    std::cout << "\nLoading assets from csv..." << std::endl;

    LoadAssetError load_result = loadAssets("../data/", assets, asset_count_type);
    switch (load_result)
    {
    case LoadAssetError::Success: 
        std::cout << "The assets have been loaded successfully.\n" << std::endl;
        break;
    case LoadAssetError::DirectoryOpenError: 
        exit(1);
    case LoadAssetError::NoValidFiles: 
        std::cout << "No valid files found in the directory\n" << std::endl;
        exit(1);
    case LoadAssetError::FileReadError: 
        exit(1);
    }

    std::vector<const Asset *> assetPtrs;
    assetPtrs.reserve(assets.size());
    for (const auto &asset : assets)
    {
        assetPtrs.emplace_back(&asset);
    }

    size_t num_iterations         = 10;
    size_t num_simulations        = (option_type == OptionType::European) ? 1e6 : 1e5;
    double strike_price           = calculateStrikePrice(assets);
    size_t months                 = assetPtrs[0]->getDailyReturnsSize() / 21;
    const  uint std_dev_from_mean = 24 * months;
    double variance               = 0.0;
    double variance_temp          = 0.0;
    double standard_error         = 0.0;
    std::pair<double, double> result;
    std::pair<double, double> result_temp;
    result.first  = 0.0;
    result.second = 0.0;
    MonteCarloError error;

      // Create the payoff function.
      // The payoff function describes the financial beahviour of the option,
      // and it is required to calculate the price of the option.
    auto function_pair = createPayoffFunction(strike_price, assets);
    auto function      = function_pair.first;
    auto coefficients  = function_pair.second;

    std::vector<double> predicted_assets_prices;
    predicted_assets_prices.resize(assets.size());
    std::vector<double> integration_bounds;
    integration_bounds.resize(assets.size() * 2);

      // Set the integration bounds based on the assets, on which the domain of the hyperrectangle is based.
      // The integration bounds are required in order to apply the Monte Carlo method for the option pricing,
      // and they are calculated based on the standard deviation from the mean of the assets.
    if (setIntegrationBounds(integration_bounds, assets, std_dev_from_mean) == -1)
    {
        std::cout << "\nError setting the integration bounds" << std::endl;
        exit(1);
    }

    HyperRectangle hyperrectangle(assetPtrs.size(), integration_bounds);

    // for (size_t i = 0; i < assetPtrs.size(); i++)
    // {
    //     std::cout << "mean and std dev of " << (*assetPtrs[i]).getName() << " are " << (*assetPtrs[i]).getReturnMean()
    //               << " and " << (*assetPtrs[i]).getReturnStdDev() << std::endl;
    // }
    std::cout << "Calculating the price of the option...\n" << std::endl;

      // Apply the Monte Carlo method to calculate the price of the option
    for (size_t j = 0; j < num_iterations; ++j)
    {
        result_temp = monteCarloPricePrediction(num_simulations,
                                                function,
                                                hyperrectangle,
                                                assetPtrs,
                                                std_dev_from_mean,
                                                variance_temp,
                                                coefficients,
                                                strike_price,
                                                predicted_assets_prices,
                                                option_type,
                                                error);

        if (error != MonteCarloError::Success)
        {
            std::cerr << "Error in Monte Carlo simulation" << std::endl;
            exit(1);
        }

        result.first   += result_temp.first;
        result.second  += result_temp.second;
        variance       += variance_temp;
        standard_error += std::sqrt(variance_temp / static_cast<double>(num_simulations));

        double progress = static_cast<double>(j + 1) / static_cast<double>(num_iterations) * 100;
        std::cout << "Process at " << progress << "% ..." << std::endl;
    }

    result.first   /= num_iterations;
    variance       /= num_iterations;
    standard_error /= num_iterations;

    for (size_t i = 0; i < assetPtrs.size(); ++i)
    {
        predicted_assets_prices[i] /= (num_iterations * num_simulations);
    }

    std::cout << std::endl;
    if (option_type == OptionType::European && asset_count_type == AssetCountType::Single)
    {
        double BS_option_price = computeBlackScholesOptionPrice(assetPtrs, strike_price);
        std::cout << "The option expected payoff calculated via Black-Scholes model is " << BS_option_price << std::endl;
    }

    std::cout << "The option expected payoff calculated via Monte Carlo method is " << result.first << std::endl;

    writeResultsToFile(assets, result, standard_error, function, num_simulations, option_type);

    std::cout << "\nThe integral has been calculated successfully " << num_iterations << " times for " << num_simulations << " points." << std::endl;
    std::cout << "The resulting expected discounted option payoff is the average of the " << num_iterations << " iterations.\n";
    std::cout << "\nThe results have been saved to output.txt\n"
              << std::endl;

    for (size_t i = 0; i < assets.size(); ++i)
    {
        std::cout << "The predicted future prices (one year) of one " << assets[i].getName() << " stock is " << predicted_assets_prices[i] << std::endl;
    }
}
