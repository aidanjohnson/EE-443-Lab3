function magnitudeFactor = GetMagnitudeFactor(filterBand)
	magnitudeFactor = 0.0;
	
	if(filterBand >= 1 && filterBand <= 14)
		magnitudeFactor = 0.015;
    elseif(filterBand >= 15 && filterBand <= 48)
		magnitudeFactor = 2.0 / (GetCenterFrequency(filterBand + 1) - GetCenterFrequency(filterBand -1));
    end

end