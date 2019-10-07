#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include "parameters.h"

namespace Sndspec {

template <typename T>
using SpectrogramResults = std::vector<std::vector<std::vector<T>>>; // (channels x spectrums x numbins)

class Spectrogram {
public:
	static void makeSpectrogram(const Parameters& parameters);
	static void scaleMagnitudeRelativeDb(SpectrogramResults<double>& s, bool fromMagSquared = true);
};

} // namespace Sndspec


#endif
