#include "new_video_writer.h"

// https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72
void HSVtoRGB(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV) {
  float fC = fV * fS; // Chroma
  float fHPrime = fmod(fH / 60.0, 6);
  float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
  float fM = fV - fC;
  
  if(0 <= fHPrime && fHPrime < 1) {
    fR = fC;
    fG = fX;
    fB = 0;
  } else if(1 <= fHPrime && fHPrime < 2) {
    fR = fX;
    fG = fC;
    fB = 0;
  } else if(2 <= fHPrime && fHPrime < 3) {
    fR = 0;
    fG = fC;
    fB = fX;
  } else if(3 <= fHPrime && fHPrime < 4) {
    fR = 0;
    fG = fX;
    fB = fC;
  } else if(4 <= fHPrime && fHPrime < 5) {
    fR = fX;
    fG = 0;
    fB = fC;
  } else if(5 <= fHPrime && fHPrime < 6) {
    fR = fC;
    fG = 0;
    fB = fX;
  } else {
    fR = 0;
    fG = 0;
    fB = 0;
  }
  
  fR += fM;
  fG += fM;
  fB += fM;
}

void fillbgrframe(uint8_t* data)
{
	// horizontal rainbow pattern from left to right
	// this is BGR
	for (int y = 0; y < 1080; y++) {
		for (int x = 0; x < 1920; x++) {
			float h = (float)x / 1920.0 * 360.0;
			float s = 1.0;
			float v = 1.0;
			float r, g, b;
			HSVtoRGB(r, g, b, h, s, v);
			data[y*1920*3 + x*3 + 0] = (uint8_t)(b * 255.0);
			data[y*1920*3 + x*3 + 1] = (uint8_t)(g * 255.0);
			data[y*1920*3 + x*3 + 2] = (uint8_t)(r * 255.0);
		}
	}
}

int main(void)
{
	VideoWriter writer("vwtest.mp4", 30, 1920, 1080, true);
	uint8_t data[1920*1080*3];
	fillbgrframe(&data[0]);
	//printf("%d %d %d %d\n", data[0], data[1], data[2], data[3]);
	for (int i=0; i<300; i++)
		writer.write(&data[0]);
	printf("Finished writing\n");
	return 0;
}