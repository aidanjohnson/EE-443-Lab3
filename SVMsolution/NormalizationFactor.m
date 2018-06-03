function result = NormalizationFactor(NumFilters, m)
    normalizationFactor = 0;
    if(m==0)
        normalizationFactor = sqrt(1/NumFilters);
    else
        normalizationFactor = sqrt(2/NumFilters);
    end
    result = normalizationFactor;
end