#include "spectrum.h"

#include <vector>
#include <cmath>
#include <algorithm>

#include <iostream>

// todo: float version
// todo: long double version
// todo: quad precision version etc


namespace Sndspec {

// todo: this is only good for doubles: specialize for FloatType
Spectrum::Spectrum(int fftLength) : fftLength(fftLength), spectrumLength(fftLength / 2)
{
	tdBuf = static_cast<double*>(fftw_malloc(sizeof(double) * static_cast<size_t>(fftLength)));
	fdBuf = static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * static_cast<size_t>(fftLength)));
	plan = fftw_plan_dft_r2c_1d(fftLength, tdBuf, fdBuf, FFTW_MEASURE | FFTW_PRESERVE_INPUT);
}

Spectrum::~Spectrum()
{
	fftw_destroy_plan(plan);
	fftw_free(tdBuf);
	fftw_free(fdBuf);
}

void Spectrum::exec()
{
	fftw_execute(plan);
}

int Spectrum::getFftLength() const
{
	return fftLength;
}

int Spectrum::getSpectrumLength() const
{
	return spectrumLength;
}

//std::pair<double, int> Spectrum::getFdPeak() const
//{
//	auto it = std::max_element(mag.cbegin(), mag.cend());
//	return {*it, std::distance(mag.cbegin(), it)};
//}

double* Spectrum::getTdBuf() const
{
	return tdBuf;
}

const fftw_complex *Spectrum::getFdBuf() const
{
	return fdBuf;
}

void Spectrum::getMag(std::vector<double>& buf)
{
	for(int b = 0; b < spectrumLength; b++) {
		double re = fdBuf[b][0];
		double im = fdBuf[b][1];
		buf[static_cast<std::vector<double>::size_type>(b)] = std::sqrt(re * re + im * im);
	}
}

void Spectrum::getPhase(std::vector<double>& buf)
{
	for(int b = 0; b < spectrumLength; b++) {
		double re = fdBuf[b][0];
		double im = fdBuf[b][1];
		buf[static_cast<std::vector<double>::size_type>(b)] = std::atan2(im, re);
	}
}

} // namespace Sndspec
