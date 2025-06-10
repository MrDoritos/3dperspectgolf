#include "advancedConsole.h"
#include <math.h>

bool isSolid(int x, int y) {
	//return ((sin(x * 0.75f) + sin(y * 0.75f)) * 0.5f) > 0.0f;
	return (((sin(x * 0.75f) + sin(y * 0.75f))+ (sin(x * 0.375f) + sin(y * 0.375f))) * 0.25f) > 0.0f;
}

float playerX, playerY, playerA;

int wmain() {
	while (!adv::ready) console::sleep(10);
	
	playerX = -2.5f; playerY = -2.5f; playerA = 0.0f;
	int cwidth = adv::width;
	int cheight = adv::height;
	
	while (true) {
		console::clear();
		
		float step = float(cwidth) / 1.5708f;
		for (float theta = -0.7854f; theta <= 0.7854f; theta += 1/step) {
			float castX = sinf(playerA + theta), castY = -cosf(playerA + theta);
			for (float testRay = 0.0f; testRay < 10.0f; testRay += 0.1f) {
				char characters[] = { "#&!!/*;',.." };
				if (isSolid(testRay * castX + playerX, testRay * castY + playerY)) {
					int floor = float(cheight * 0.5f) - cheight / float(testRay);
					for (int y = floor; y < cheight - floor; y++) {
						adv::write((theta + 0.7854f) * step, y, characters[int(testRay)], FRED | BBLACK);
					}
					break;
				}
			}
		}
		
		//debug
		for (int x = 0; x < 11; x++) {
			for (int y = 0; y < 11; y++) {
				adv::write(x, y, isSolid(x + playerX - 5.5f,y + playerY - 5.5f) ? '#' : ' ', FWHITE | BBLACK);
			}
		}	
		adv::write(5,5,'o',FRED|BBLACK);
		adv::write(5.5f + sinf(playerA), 5.5f - cosf(playerA), '+', FGREEN|BBLACK);
		
		//input
		switch (NOMOD(console::readKey())) {
			case 'a':
				playerX += sinf(playerA - 1.5708) * 0.5f;
				playerY -= cosf(playerA - 1.5708) * 0.5f;
				break;
			case 'd':
				playerX += sinf(playerA + 1.5708) * 0.5f;
				playerY -= cosf(playerA + 1.5708) * 0.5f;
				break;
			case 's':
				playerX -= sinf(playerA) * 0.5f;
				playerY += cosf(playerA) * 0.5f;
				break;
			case 'w':
				playerX += sinf(playerA) * 0.5f;
				playerY -= cosf(playerA) * 0.5f;
				break;
			case VK_LEFT:
				playerA -= 3.1416f / 8.0f;
				break;
			case VK_RIGHT:
				playerA += 3.1416f / 8.0f;
				break;
			case VK_ESCAPE:
				return 0;
				break;
		}
	}
	
	return 0;
}