#include "iostream"
#include "time.h"
#include "math.h"
#include "stdlib.h"
#include "pspkernel.h"
#include "pspdebug.h"
#include "pspdisplay.h"
#include "pspctrl.h"
#include "./lib/glib2d.h"
#include "./common/callback.h"
#include "./common/helpers.h"

using namespace std;

#define screenWidth 480
#define screenHeight 272

#define textureSize 64

#define mapWidth 24
#define mapHeight 24

#define numSprites 19

#define WALLS
#define FLOORANDCELLING
#define SPRITES

PSP_MODULE_INFO("Raycaster", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_MAX();

int worldMap[mapWidth][mapHeight] =
	{
		{8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6, 4},
		{8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
		{8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
		{8, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
		{8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
		{8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 4, 6},
		{8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6},
		{7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6},
		{7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6},
		{7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4},
		{7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6},
		{7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6, 0, 6, 6, 6},
		{7, 7, 7, 7, 0, 7, 7, 7, 7, 8, 8, 4, 0, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3},
		{2, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3},
		{2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
		{2, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
		{1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3},
		{2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 6, 6, 0, 0, 5, 0, 5, 0, 5},
		{2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
		{2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
		{2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
		{2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
		{2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5}};

struct Sprite
{
	float x;
	float y;
	int texture;
};

Sprite sprite[numSprites] =
	{
		// Green Lights
		{20.5f, 11.5f, 10},
		{18.5f, 4.5f, 10},
		{10.0f, 4.5f, 10},
		{10.0f, 12.5f, 10},
		{3.5f, 6.5f, 10},
		{3.5f, 20.5f, 10},
		{3.5f, 14.5f, 10},
		{14.5f, 20.5f, 10},

		// Pillars
		{18.5f, 10.5f, 9},
		{18.5f, 11.5f, 9},
		{18.5f, 12.5f, 9},

		// Barrels
		{21.5f, 1.5f, 8},
		{15.5f, 1.5f, 8},
		{16.0f, 1.8f, 8},
		{16.2f, 1.2f, 8},
		{3.5f, 2.5f, 8},
		{9.5f, 15.5f, 8},
		{10.0f, 15.1f, 8},
		{10.5f, 15.8f, 8},
};

// 1D ZBuffer
float ZBuffer[screenWidth];

// Arrays used to sort the sprites
int spriteOrder[numSprites];
int spriteDistance[numSprites];

// Render texture
g2dTexture *tex = g2dTexCreate(screenWidth, screenHeight);

int main()
{
	setupCallbacks();

	// X and Y player position
	float posX = 22.0f, posY = 11.5f;
	// Look direction
	float dirX = -1.0f, dirY = 0.0f;
	// Camera plane
	float planeX = 0.0f, planeY = 0.65f;

	clock_t t;
	clock_t told;

	// Load textures
	g2dTexture *texture[11];
	texture[0] = g2dTexLoad("res/gfx/textures/eagle.png", G2D_NO_SWIZZLE);
	texture[1] = g2dTexLoad("res/gfx/textures/redbrick.png", G2D_NO_SWIZZLE);
	texture[2] = g2dTexLoad("res/gfx/textures/purplestone.png", G2D_NO_SWIZZLE);
	texture[3] = g2dTexLoad("res/gfx/textures/greystone.png", G2D_NO_SWIZZLE);
	texture[4] = g2dTexLoad("res/gfx/textures/bluestone.png", G2D_NO_SWIZZLE);
	texture[5] = g2dTexLoad("res/gfx/textures/mossy.png", G2D_NO_SWIZZLE);
	texture[6] = g2dTexLoad("res/gfx/textures/wood.png", G2D_NO_SWIZZLE);
	texture[7] = g2dTexLoad("res/gfx/textures/colorstone.png", G2D_NO_SWIZZLE);

	// Load sprites
	texture[8] = g2dTexLoad("res/gfx/textures/barrel.png", G2D_NO_SWIZZLE);
	texture[9] = g2dTexLoad("res/gfx/textures/pillar.png", G2D_NO_SWIZZLE);
	texture[10] = g2dTexLoad("res/gfx/textures/greenlight.png", G2D_NO_SWIZZLE);

	// Swap texture X/Y since they'll be used as vertical stripes
	for (int iT = 0; iT < 8; iT++)
	{
		for (int xT = 0; xT < textureSize; xT++)
		{
			for (int yT = 0; yT < xT; yT++)
			{
				std::swap(texture[iT]->data[textureSize * yT + xT], texture[iT]->data[textureSize * xT + yT]);
			}
		}
	}

	while (isRunning())
	{

// Floor and Celling
#ifdef FLOORANDCELLING
		for (int y = screenHeight; y > screenHeight / 2; y--)
		{
			// rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
			float rayDirX0 = dirX - planeX;
			float rayDirY0 = dirY - planeY;
			float rayDirX1 = dirX + planeX;
			float rayDirY1 = dirY + planeY;

			// Current y position compared to the center of the screen (the horizon)
			int p = y - screenHeight / 2;

			// Vertical position of the camera.
			float posZ = 0.5f * screenHeight;

			// Horizontal distance from the camera to the floor for the current row.
			// 0.5 is the z position exactly in the middle between floor and ceiling.
			float rowDistance = posZ / p;

			// Calculate the real world step vector we have to add for each x (parallel to camera plane)
			// Adding step by step avoids multiplications with a weight in the inner loop
			float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
			float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;

			// Real world coordinates of the leftmost column. This will be updated as we step to the right.
			float floorX = posX + rowDistance * rayDirX0;
			float floorY = posY + rowDistance * rayDirY0;
			for (int x = 0; x < screenWidth; ++x)
			{
				// The cell coord is simply got from the integer parts of floorX and floorY
				int cellX = (int)floorX;
				int cellY = (int)floorY;

				// Get the texture coordinate from the fractional part
				int tx = (int)(textureSize * (floorX - cellX)) & (textureSize - 1);
				int ty = (int)(textureSize * (floorY - cellY)) & (textureSize - 1);

				floorX += floorStepX;
				floorY += floorStepY;

				// Choose texture and draw the pixel
				g2dColor color;

				// Set floor texture
				int checkerBoardPattern = (int(floorX) + int(floorY)) % 2;
				int floorTexture;
				if (checkerBoardPattern == 0)
					floorTexture = 3;
				else
					floorTexture = 5;

				// Set celling texture
				int ceilingTexture = 6;

				int pixelPosition = textureSize * ty + tx;

				// Paint Ceiling
				color = (g2dColor)texture[ceilingTexture]->data[pixelPosition];
				tex->data[x + tex->tw * (screenHeight - y - 1)] = (g2dColor)color;

				// Paint Floor
				color = (g2dColor)texture[floorTexture]->data[pixelPosition];
				tex->data[x + tex->tw * y] = (g2dColor)color;
			}
		}
#endif

// Walls
#ifdef WALLS
		for (int x = 0; x < screenWidth; x++)
		{
			// Calculate ray position and direction
			float cameraX = 2.0f * x / (float)screenWidth - 1;
			float rayDirX = dirX + planeX * cameraX;
			float rayDirY = dirY + planeY * cameraX;

			// Which box of the map we're in
			int mapX = posX;
			int mapY = posY;

			// Length of ray from current position to next x or y-side
			float sideDistX = 0.0f;
			float sideDistY = 0.0f;

			float deltaDistX = (rayDirX == 0.0f) ? 1e30 : fabs(1.0f / rayDirX);
			float deltaDistY = (rayDirY == 0.0f) ? 1e30 : fabs(1.0f / rayDirY);

			float perpWallDist;

			// What direction to step in x or y-direction
			int stepX;
			int stepY;

			// Checking if we hit a wall
			int hit = 0;
			// Hit on NS or a EW wall
			int side;

			// Calculate step and initial sideDist
			if (rayDirX < 0.0f)
			{
				stepX = -1;
				sideDistX = (float)(posX - mapX) * deltaDistX;
			}
			else
			{
				stepX = 1;
				sideDistX = (float)(mapX + 1.0f - posX) * deltaDistX;
			}
			if (rayDirY < 0.0f)
			{
				stepY = -1;
				sideDistY = (float)(posY - mapY) * deltaDistY;
			}
			else
			{
				stepY = 1;
				sideDistY = (float)(mapY + 1.0f - posY) * deltaDistY;
			}

			// Perform DDA
			while (hit == 0)
			{
				// Jump to next map square, either in x-direction, or in y-direction
				if (sideDistX < sideDistY)
				{
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				}
				else
				{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}
				// Check if ray has hit a wall
				if (worldMap[mapX][mapY] > 0)
					hit = 1;
			}

			// Calculate distance projected on camera direction
			if (side == 0)
				perpWallDist = (sideDistX - deltaDistX);
			else
				perpWallDist = (sideDistY - deltaDistY);

			// Calculate height of line to draw on screen
			int lineHeight = (int)(screenHeight / perpWallDist);

			// Calculate lowest and highest pixel to fill in current stripe
			int drawStart = (-lineHeight / 2) + (screenHeight / 2);
			if (drawStart < 0)
				drawStart = 0;
			int drawEnd = (lineHeight / 2) + (screenHeight / 2);
			if (drawEnd >= screenHeight)
				drawEnd = screenHeight - 1;

			// Texturing calculations
			// -1 for texture 0 usage
			int texNum = worldMap[mapX][mapY] - 1;

			// Calculate value of wallX
			float wallX = 0.0f;
			if (side == 0)
				wallX = posY + perpWallDist * rayDirY;
			else
				wallX = posX + perpWallDist * rayDirX;
			wallX -= floorf(wallX);

			// X coordinate on the texture
			int texX = (int)(wallX * (float)textureSize);
			if (side == 0 && rayDirX > 0.0f)
				texX = textureSize - texX - 1;
			if (side == 1 && rayDirY < 0.0f)
				texX = textureSize - texX - 1;

			// How much to increase the texture coordinate per screen pixel
			float step = 1.0f * textureSize / lineHeight;
			// Starting texture coordinate
			float texPos = (drawStart - ((float)screenHeight / 2.0f) + ((float)lineHeight / 2.0f)) * step;

			for (int y = drawStart; y < drawEnd; y++)
			{
				int texY = (int)texPos & (textureSize - 1);
				texPos += step;
				g2dColor color = (g2dColor)texture[texNum]->data[textureSize * texX + texY];
				
				// Give x and y sides different brightness
				if (side == 1)
					color = G2D_RGBA(G2D_GET_R(color) / 2, G2D_GET_G(color) / 2, G2D_GET_B(color) / 2, 255);
				tex->data[tex->tw * y + x] = (g2dColor)color;
			}

			// Seting the ZBuffer for Sprite Casting
			ZBuffer[x] = perpWallDist;
		}
#endif

// SPRITE CASTING
#ifdef SPRITES
		// Sort sprites from far to close
		for (int i = 0; i < numSprites; i++)
		{
			spriteOrder[i] = i;
			spriteDistance[i] = ((posX - sprite[i].x) * (posX - sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y)); // sqrt not taken, unneeded
		}
		sortSprites(spriteOrder, (int *)spriteDistance, numSprites);

		// After sorting the sprites, do the projection and draw them
		for (int i = 0; i < numSprites; i++)
		{
			// Translate sprite position to relative to camera
			float spriteX = sprite[spriteOrder[i]].x - posX;
			float spriteY = sprite[spriteOrder[i]].y - posY;

			// Transform sprite with the inverse camera matrix
			// Required for correct matrix multiplication
			float invDet = 1.0f / (planeX * dirY - dirX * planeY);

			float transformX = invDet * (dirY * spriteX - dirX * spriteY);
			// Depth (Z) inside the screen
			float transformY = invDet * (-planeY * spriteX + planeX * spriteY);

			int spriteScreenX = int((screenWidth / 2) * (1 + transformX / transformY));

			// Calculate height of the sprite on screen
			// Using 'transformY' instead of the real distance prevents fisheye
			int spriteHeight = abs(int(screenHeight / (transformY)));
			// Calculate lowest and highest pixel to fill in current stripe
			int drawStartY = -spriteHeight / 2 + screenHeight / 2;
			if (drawStartY < 0)
				drawStartY = 0;
			int drawEndY = spriteHeight / 2 + screenHeight / 2;
			if (drawEndY >= screenHeight)
				drawEndY = screenHeight - 1;

			// Calculate width of the sprite
			int spriteWidth = abs(int(screenHeight / (transformY)));
			int drawStartX = -spriteWidth / 2 + spriteScreenX;
			if (drawStartX < 0)
				drawStartX = 0;
			int drawEndX = spriteWidth / 2 + spriteScreenX;
			if (drawEndX >= screenWidth)
				drawEndX = screenWidth - 1;

			// Loop through every vertical stripe of the sprite on screen
			for (int stripe = drawStartX; stripe < drawEndX; stripe++)
			{
				int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * textureSize / spriteWidth) / 256;
				// The conditions in the if are:
				// 1) it's in front of camera plane so you don't see things behind you
				// 2) it's on the screen (left)
				// 3) it's on the screen (right)
				// 4) ZBuffer, with perpendicular distance
				if (transformY > 0 && stripe > 0 && stripe < screenWidth && transformY < ZBuffer[stripe])
					for (int y = drawStartY; y < drawEndY; y++) // for every pixel of the current stripe
					{
						// 256 and 128 factors to avoid floats
						int d = (y)*256 - screenHeight * 128 + spriteHeight * 128;
						int texY = ((d * textureSize) / spriteHeight) / 256;
						// Get current color from the texture
						g2dColor color = (g2dColor)texture[sprite[spriteOrder[i]].texture]->data[textureSize * texY + texX];

						// Paint pixel if it isn't black, black is the invisible color
						if ((color & BLACK) != 0)
							tex->data[tex->tw * y + stripe] = (g2dColor)color;
					}
			}
		}
#endif
		// Timing for input
		told = t;
		t = clock();
		float frameTime = (t - told) / 1000.0f;

		// The constant value is in squares/second
		float moveSpeed = frameTime * 0.005f;
		// The constant value is in radians/second
		float rotSpeed = frameTime * 0.002f;

		SceCtrlData ctrldata;
		sceCtrlReadBufferPositive(&ctrldata, 1);

		// Move forward if no wall in front of you
		if (ctrldata.Buttons & PSP_CTRL_UP)
		{
			if (worldMap[(int)(posX + dirX * moveSpeed)][(int)(posY)] == false)
				posX += dirX * moveSpeed;
			if (worldMap[(int)(posX)][(int)(posY + dirY * moveSpeed)] == false)
				posY += dirY * moveSpeed;
		}
		// Move backwards if no wall behind you
		if (ctrldata.Buttons & PSP_CTRL_DOWN)
		{
			if (worldMap[(int)(posX - dirX * moveSpeed)][(int)(posY)] == false)
				posX -= dirX * moveSpeed;
			if (worldMap[(int)(posX)][(int)(posY - dirY * moveSpeed)] == false)
				posY -= dirY * moveSpeed;
		}
		// Rotate to the right
		if (ctrldata.Buttons & PSP_CTRL_RIGHT)
		{
			// both camera direction and camera plane must be rotated
			float oldDirX = dirX;
			dirX = dirX * cosf(-rotSpeed) - dirY * sinf(-rotSpeed);
			dirY = oldDirX * sinf(-rotSpeed) + dirY * cosf(-rotSpeed);
			float oldPlaneX = planeX;
			planeX = planeX * cosf(-rotSpeed) - planeY * sinf(-rotSpeed);
			planeY = oldPlaneX * sinf(-rotSpeed) + planeY * cosf(-rotSpeed);
		}
		// Rotate to the left
		if (ctrldata.Buttons & PSP_CTRL_LEFT)
		{
			// Both camera direction and camera plane must be rotated
			float oldDirX = dirX;
			dirX = dirX * cosf(rotSpeed) - dirY * sinf(rotSpeed);
			dirY = oldDirX * sinf(rotSpeed) + dirY * cosf(rotSpeed);
			float oldPlaneX = planeX;
			planeX = planeX * cosf(rotSpeed) - planeY * sinf(rotSpeed);
			planeY = oldPlaneX * sinf(rotSpeed) + planeY * cosf(rotSpeed);
		}

		// Draw the render texture on the screen
		g2dBeginRects(tex);
		g2dSetCoordMode(G2D_UP_LEFT);
		g2dSetCoordXY(0.0f, 0.0f);
		g2dAdd();
		g2dEnd();

		g2dFlip(G2D_VSYNC);
	}

	sceKernelExitGame();
	return 0;
}
