function coef = GetCoefficient(spectralData, samplingRate, NumFilters, binSize, m)
    result = 0;
    outerSum = 0;
    innerSum = 0;
    if(m>=NumFilters)
        coef = 0;
    else
        result = NormalizationFactor(NumFilters, m);
        for l = 1:NumFilters
            innerSum = 0;
            for k = 0:binSize-2
                innerSum = innerSum + abs(spectralData(k+1)*GetFilterParameter(samplingRate, binSize, k, l));
            end
            if(innerSum>0)
                innerSum = log(innerSum);
            end
            innerSum = innerSum * cos(m*pi/NumFilters*(l-0.5));
            outerSum = outerSum + innerSum;
        end
        result = result*outerSum;
        coef = result;
    end
end