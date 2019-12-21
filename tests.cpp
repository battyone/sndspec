#include "tests.h"
#include "window.h"

bool tests::testWindow()
{
	Sndspec::Window<double> w;
	w.generate("triangular", 9, 20);
//	w.generateWindow1(8);
	for(double v : w.getData())
	{
		std::cout << v << std::endl;
	}
}
