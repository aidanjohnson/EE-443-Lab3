function centerFrequency = GetCenterFrequency(filterBand)
	centerFrequency = 0.0;

	if(filterBand == 0)
		centerFrequency = 0;
    elseif(filterBand >= 1 && filterBand <= 14)
		centerFrequency = (200.0 * filterBand) / 3.0;
    else
		exponent = filterBand - 14.0;
		centerFrequency = 1.0711703^exponent;
		centerFrequency = centerFrequency * 1073.4;
    end
	
end