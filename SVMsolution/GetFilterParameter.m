function filterParameter = GetFilterParameter(samplingRate, binSize, frequencyBand, filterBand)
	filterParameter = 0.0;

	boundary = (frequencyBand * samplingRate) / binSize;		% k * Fs / N
	prevCenterFrequency = GetCenterFrequency(filterBand - 1);		% fc(l - 1) etc.
	thisCenterFrequency = GetCenterFrequency(filterBand);
	nextCenterFrequency = GetCenterFrequency(filterBand + 1);

	if(boundary >= 0 && boundary < prevCenterFrequency)
		filterParameter = 0.0;
    elseif(boundary >= prevCenterFrequency && boundary < thisCenterFrequency)
		filterParameter = (boundary - prevCenterFrequency) / (thisCenterFrequency - prevCenterFrequency);
		filterParameter = filterParameter * GetMagnitudeFactor(filterBand);
    elseif(boundary >= thisCenterFrequency && boundary < nextCenterFrequency)
		filterParameter = (boundary - nextCenterFrequency) / (thisCenterFrequency - nextCenterFrequency);
		filterParameter = filterParameter * GetMagnitudeFactor(filterBand);
    elseif(boundary >= nextCenterFrequency && boundary < samplingRate)
		filterParameter = 0.0;
    end
        
end