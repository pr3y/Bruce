#include <TFT_eSPI.h>

class QRcode
{
	private:
		TFT_eSPI *tft;
		void render(int x, int y, int color);

	public:
		QRcode(TFT_eSPI *display);
		void init();
		void create(String message);	
};
