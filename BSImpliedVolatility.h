#ifndef __BSIMPLIEDVOLATILITY_H_INCLUDED__
#define __BSIMPLIEDVOLATILITY_H_INCLUDED__
#include "BlackScholes.h"
#include "Newton.h"
#include "FunctionalUtilities.h"
#include <cmath>

namespace IV {
    template<typename S0, typename K, typename Discount, typename Guess, typename Actual>
    auto getIV(const S0& asset, const K& strike, const Discount& discount, const Actual& callPrice, const Guess& guess){
        const Guess defaultRet=-1.0;//something ridiculous
        if(asset<0||callPrice<0){
            return defaultRet; //something ridiculous
        }
        else if(asset-strike>callPrice){
            return defaultRet;//something ridiculous
        }
        const auto result= newton::zeros(
            [&](const auto& sigma) {
                return BSCall(asset, discount, strike, sigma)-callPrice;
            },
            guess,
            .0000001,
            .0000001,
            20
        );
        if(std::isnan(result)){
            return defaultRet;
        }
        else if(std::isinf(result)){
            return defaultRet;
        }
        else{
            return result;
        }
    }

    template<typename ArrOfAsset, typename ArrOfPrice, typename K, typename Discount, typename Maturity, typename Guess>
    auto getAllIVByAsset(
        const ArrOfAsset& assets, 
        const ArrOfPrice& callPrices, 
        const K& k, 
        const Discount& discount, 
        const Maturity& timeTillMaturity,
        const Guess& guess
    ){
        const Maturity sqrtMat=sqrt(timeTillMaturity);
        return futilities::for_each_parallel_copy(
            assets,    
            [&](
                const auto& asset, 
                const auto& index
            ){
                return getIV(asset, k, discount, callPrices[index], guess)/sqrtMat;
            } 
        );
    }

    template<typename ArrOfStrike, typename ArrOfPrice, typename S0, typename Discount, typename Maturity, typename Guess>
    auto getAllIVByStrike(
        const ArrOfStrike& strikes, 
        const ArrOfPrice& callPrices, 
        const S0& asset, 
        const Discount& discount, 
        const Maturity& timeTillMaturity,
        const Guess& guess
    ){
        const Maturity sqrtMat=sqrt(timeTillMaturity);
        return futilities::for_each_parallel_copy(
            strikes,    
            [&](
                const auto& strike, 
                const auto& index
            ){
                return getIV(asset, strike, discount, callPrices[index], guess)/sqrtMat;
            } 
        );
    }


}


#endif