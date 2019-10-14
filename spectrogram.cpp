#include "spectrogram.h"
#include "window.h"
#include "reader.h"
#include "spectrum.h"
#include "uglyplot.h"
#include "renderer.h"

#include <iostream>
#include <iomanip>

#include <cassert>

void Sndspec::Spectrogram::makeSpectrogram(const Sndspec::Parameters &parameters)
{
	auto fftSize = Spectrum::selectBestFFTSizeFromSpectrumSize(parameters.getImgHeight());
	auto spectrumSize = Spectrum::convertFFTSizeToSpectrumSize(fftSize);

	std::cout << "fft size " << fftSize << " spectrum size " << spectrumSize << std::endl;

	// make a suitable Kaiser window
	Sndspec::KaiserWindow<double> k;
	k.setBeta(Sndspec::KaiserWindow<double>::betaFromDecibels(parameters.getDynRange()));
	k.generate(fftSize);

	for(const std::string& inputFilename : parameters.getInputFiles()) {
		std::cout << "Opening input file: " << inputFilename << " ... ";
		Sndspec::Reader<double> r(inputFilename, fftSize, parameters.getImgWidth());

		if(r.getSndFileHandle() == nullptr || r.getSndFileHandle()->error() != SF_ERR_NO_ERROR) {
			std::cout << "couldn't open file !" << std::endl;
		} else {
			std::cout << "ok" << std::endl;
			r.setWindow(k.getData());

			// create output storage
			SpectrogramResults<double> spectrogram(r.getNChannels(), std::vector<std::vector<double>>(parameters.getImgWidth(), std::vector<double>(spectrumSize, 0.0)));

			// create a spectrum analyzer for each channel
			std::vector<std::unique_ptr<Spectrum>> analyzers;
			for(int ch = 0; ch < r.getNChannels(); ch ++) {
				analyzers.emplace_back(new Spectrum(fftSize));

				// give the reader direct write-access to the analyzer input buffer
				r.setChannelBuffer(ch, analyzers.back()->getTdBuf());
			}

			// create a callback function to execute spectrum analysis for each block read
			r.setProcessingFunc([&analyzers, &spectrogram](int pos, int channel, const double* data) -> void {
				Spectrum* analyzer = analyzers.at(channel).get();
				assert(data == analyzer->getTdBuf());
				analyzer->exec();
				analyzer->getMagSquared(spectrogram[channel][pos]); // magSquared saves having do to square root
			});

			r.readDeinterleaved();
			scaleMagnitudeRelativeDb(spectrogram, /* magSquared = */ true);
			std::cout << "Rendering ... ";
			Renderer renderer(parameters.getImgWidth(), spectrumSize /*parameters.getImgHeight()*/);
			renderer.Render(parameters, spectrogram);
			std::cout << "Done\n";

			std::string outFile = replaceFileExt(inputFilename, "png");
			if(!outFile.empty()) {
				std::cout << "Saving to " << outFile << std::flush;
				if(renderer.writeToFile(outFile)) {
					std::cout << " ... OK" << std::endl;
				} else {
					std::cout << " ... ERROR" << std::endl;
				}
			}

		} // ends successful file-open
	} // ends loop over files
}

void Sndspec::Spectrogram::scaleMagnitudeRelativeDb(SpectrogramResults<double> &s, bool fromMagSquared)
{
	int numChannels = s.size();
	int numSpectrums = s.at(0).size();
	int numBins = s.at(0).at(0).size();

	std::vector<double> peak(numChannels, 0.0);

	for(int c = 0; c < numChannels; c++) {

		// find peak
		double peak{0.0};
		for(int x = 0; x < numSpectrums; x++) {
			for(int b = 0; b < numBins; b++) {
				peak = std::max(peak, s[c][x][b]);
			}
		}

		if(std::fpclassify(peak) != FP_ZERO) {

			// function to convert to dB
			auto scaleFunc = [scale = 1.0 / peak, dBMult = fromMagSquared ? 10.0 : 20.0] (double v) -> double {
				return dBMult * std::log10(scale * v);
			};

			// scale the data
			for(int x = 0; x < numSpectrums; x++) {
				std::transform (s[c][x].begin(), s[c][x].end(), s[c][x].begin(), scaleFunc);
			}
		}
	}
}

std::string Sndspec::Spectrogram::replaceFileExt(const std::string& filename, const std::string &newExt)
{

	auto lastDot = filename.rfind('.', filename.length());

	if(lastDot != std::string::npos) {
		std::string _fn{filename};
		_fn.replace(lastDot + 1, newExt.length(), newExt);
		return _fn;
	}

	return {};
}

